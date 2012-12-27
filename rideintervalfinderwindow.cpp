#include "rideintervalfinderwindow.h"
#include "datalog.h"
#include "dateselectorwidget.h"
#include "dataprocessing.h"
#include "tcxparser.h"
#include "fitparser.h"
#include "user.h"
#include "logdirectorysummary.h"
#include "latlng.h"
#include "googlemapwindow.h"

#include <cassert>

#include <QDir.h>
#include <QComboBox.h>
#include <QLabel.h>
#include <QBoxLayout.h>
#include <QProgressDialog.h>
#include <QPushButton.h>
#include <QMessageBox.h>
#include <QTreeView.h>
#include <QStandardItemModel.h>

#define PROXIMITY_THD 15.0 // meters

using namespace std;

/******************************************************/
RideIntervalFinderWindow::RideIntervalFinderWindow(
	boost::shared_ptr<GoogleMapWindow> google_map_window, 
	boost::shared_ptr<User> user, 
	DataLog* data_log):
QWidget(),
_google_map_window(google_map_window),
_user(user),
_current_data_log(data_log)
{
	// @todo: clean up this pointer mess!

	setWindowTitle("RideIntervalFinder");
	setWindowIcon(QIcon("./resources/rideviewer_head128x128.ico")); 

	_tcx_parser = new TcxParser();
	_fit_parser = new FitParser();

	// Create the widget for selecting dates
	_date_selector_widget = new DateSelectorWidget();
	
	// Load the user log directory to set dates for the date selector widget
	_log_dir_summary = new LogDirectorySummary(_user->logDirectory());
	_log_dir_summary->readFromFile();
	_date_selector_widget->setRangeDates(_log_dir_summary->firstLog().date(),_log_dir_summary->lastLog().date());

	// Create pushbutton
	QPushButton* find_intervals_button = new QPushButton("Find Intervals");
	connect(find_intervals_button, SIGNAL(clicked()),this,SLOT(findIntervals()));

	// Create the tree view for displaying found intervals
	_tree = new QTreeView(this);
	QStandardItemModel* model = new QStandardItemModel;
	setModelColumnHeadings(*model);
	_tree->setModel(model);
	_tree->show();
	formatTreeView();
	
	// Layout the GUI
	QVBoxLayout* vlayout = new QVBoxLayout(this);
	vlayout->addWidget(_date_selector_widget);
	vlayout->addWidget(find_intervals_button);
	vlayout->addWidget(_tree);

	setMinimumSize(650,400);
}

/******************************************************/
RideIntervalFinderWindow::~RideIntervalFinderWindow()
{}

/******************************************************/
void RideIntervalFinderWindow::setModelColumnHeadings(QStandardItemModel& model) const
{
	model.setHorizontalHeaderItem(0,new QStandardItem(QString("Date")));
	model.setHorizontalHeaderItem(1,new QStandardItem(QString("Time (min)")));
	model.setHorizontalHeaderItem(2,new QStandardItem(QString("Dist (km)")));
	model.setHorizontalHeaderItem(3,new QStandardItem(QString("Mean Speed (km/h)")));
	model.setHorizontalHeaderItem(4,new QStandardItem(QString("Mean HR(bpm)")));
	model.setHorizontalHeaderItem(5,new QStandardItem(QString("Max Speed (km/h)")));
	model.setHorizontalHeaderItem(6,new QStandardItem(QString("Max HR (bpm)")));
}

/******************************************************/
void RideIntervalFinderWindow::formatTreeView()
{
	_tree->setAlternatingRowColors(true);
	_tree->setColumnWidth(0,123);
	_tree->setColumnWidth(1,60);
	_tree->setColumnWidth(2,60);
	_tree->setColumnWidth(3,100);
	_tree->setColumnWidth(4,80);
	_tree->setColumnWidth(5,95);
	_tree->setColumnWidth(6,75);
	_tree->sortByColumn(1,Qt::DescendingOrder);
	_tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

/******************************************************/
bool RideIntervalFinderWindow::parse(const QString filename, DataLog* data_log)
{
	if (filename.contains(".fit"))
	{
		return _fit_parser->parse(filename, *data_log);
	}
	else if (filename.contains(".tcx"))
	{
		return _tcx_parser->parse(filename, *data_log);
	}
	else
	{
		return false; // unknown log type
	}
}

/******************************************************/
void RideIntervalFinderWindow::findIntervals()
{
	// Create a new modle view
	_model = new QStandardItemModel;
	setModelColumnHeadings(*_model);
	_tree->setModel(_model);

	// Retreive the start and end long/lat for the user selection
	int start_index, end_index;
	_google_map_window->getSelectedIndecies(start_index, end_index);

	// Define the start and end lat/long (and the following points)
	const LatLng start_lat_lng(_current_data_log->ltd(start_index), _current_data_log->lgd(start_index), PROXIMITY_THD);
	const LatLng end_lat_lng(_current_data_log->ltd(end_index-1), _current_data_log->lgd(end_index-1), PROXIMITY_THD); // take -1 since we need an extra point to determine direction

	const LatLng start_lat_lng_nxt(_current_data_log->ltd(start_index+1), _current_data_log->lgd(start_index+1), PROXIMITY_THD);
	const LatLng end_lat_lng_nxt(_current_data_log->ltd(end_index), _current_data_log->lgd(end_index), PROXIMITY_THD);

	// Create a small progress bar
	QProgressDialog load_progress("Loading log:", "Cancel load", 0, _log_dir_summary->numLogs()-1, this);
	load_progress.setWindowModality(Qt::WindowModal);
	load_progress.setMinimumDuration(0); //msec
	load_progress.setWindowTitle("RideIntervalFinder");

	// Load new log files in the directory
	QStandardItem *parent_item = _model->invisibleRootItem();
	for (int i=0; i < _log_dir_summary->numLogs(); ++i)
	{
		const QString filename = _log_dir_summary->log(i)._filename;

		load_progress.setValue(i);
		load_progress.setLabelText("Loading log: " + filename);
		if (load_progress.wasCanceled())
			break;

		// Check that the date is within the user selected range
		const QDate date = _log_dir_summary->log(i).date();
		if (date >= _date_selector_widget->minDate() && date <= _date_selector_widget->maxDate())
		{
			DataLog* data_log = new DataLog;	
			if (parse(filename, data_log)) // if the log files is successfully parsed
			{	
				if (data_log->lgdValid() && data_log->ltdValid()) // if we have gps data in this log
				{
					bool start_found = false;
					bool end_found = false;
					bool route_verified = false;

					int found_start_index, found_end_index;

					const int increment = 1;
					for (int pt=0; pt < data_log->numPoints()-increment; pt+=increment)
					{
						const LatLng lat_lng(data_log->ltd(pt), data_log->lgd(pt), PROXIMITY_THD);
						const LatLng lat_lng_nxt(data_log->ltd(pt+1), data_log->lgd(pt+1), PROXIMITY_THD);
					
						// First check if the start point matches
						if (!start_found)
						{
							start_found = arePointsEqual(lat_lng, lat_lng_nxt, start_lat_lng, start_lat_lng_nxt);
							if (start_found)
								found_start_index = pt;
						}
						if (start_found)
						{
							end_found = arePointsEqual(lat_lng, lat_lng_nxt, end_lat_lng, end_lat_lng_nxt);
							if (end_found)
								found_end_index = pt;
						}

						if (start_found && end_found)
						{
							if (verifyRoute(*_current_data_log, start_index, end_index,
											*data_log, found_start_index, found_end_index))
							{
								// Populate the model view
								QList<QStandardItem*> interval_list;
								populateIntervalData(interval_list, *data_log, found_start_index, found_end_index);
								parent_item->appendRow(interval_list);

								_tree->setModel(_model);

								start_found = false;
								end_found = false;
							}
						}
					}
				}
			}
			delete data_log;
		}
	}
}

/******************************************************/
void RideIntervalFinderWindow::populateIntervalData(
	QList<QStandardItem*>& interval_list, 
	DataLog& data_log, 
	int found_start_index, int found_end_index) const
{
		// Data and time
	QString date = data_log.dateString();
	date.chop(3); // remove seconds
	QStandardItem *ride_name = new QStandardItem(date);
	ride_name->setFlags(ride_name->flags() & ~Qt::ItemIsEditable);

	// Interval time length
	const QString time = DataProcessing::minsFromSecs(data_log.time(found_end_index) - data_log.time(found_start_index));
	QStandardItem *interval_time = new QStandardItem(time);
	interval_time->setFlags(interval_time->flags() & ~Qt::ItemIsEditable);

	// Interval distance
	const QString dist = DataProcessing::kmFromMeters(data_log.dist(found_end_index) - data_log.dist(found_start_index));
	QStandardItem *interval_dist = new QStandardItem(dist);
	interval_dist->setFlags(interval_dist->flags() & ~Qt::ItemIsEditable);

	// Interval mean speed
	const QString mean_spd = QString::number(DataProcessing::computeAverage(data_log.speed().begin() + found_start_index, data_log.speed().begin() + found_end_index),'f',1);
	QStandardItem *interval_mean_spd = new QStandardItem(mean_spd);
	interval_mean_spd->setFlags(interval_mean_spd->flags() & ~Qt::ItemIsEditable);

	// Interval mean HR
	const QString mean_hr = QString::number(DataProcessing::computeAverage(data_log.heartRate().begin() + found_start_index, data_log.heartRate().begin() + found_end_index),'f',1);
	QStandardItem *interval_mean_hr = new QStandardItem(mean_hr);
	interval_mean_hr->setFlags(interval_mean_hr->flags() & ~Qt::ItemIsEditable);

	// Interval max speed
	const QString max_spd = QString::number(DataProcessing::computeMax(data_log.speed().begin() + found_start_index, data_log.speed().begin() + found_end_index),'f',1);
	QStandardItem *interval_max_spd = new QStandardItem(max_spd);
	interval_max_spd->setFlags(interval_max_spd->flags() & ~Qt::ItemIsEditable);

	// Interval max HR
	const QString max_hr = QString::number(DataProcessing::computeMax(data_log.heartRate().begin() + found_start_index, data_log.heartRate().begin() + found_end_index),'f',0);
	QStandardItem *interval_max_hr = new QStandardItem(max_hr);
	interval_max_hr->setFlags(interval_max_hr->flags() & ~Qt::ItemIsEditable);

	interval_list << ride_name << interval_time << interval_dist << interval_mean_spd << interval_mean_hr << interval_max_spd << interval_max_hr;
}

/******************************************************/
bool RideIntervalFinderWindow::arePointsEqual(
	const LatLng& pt_a, const LatLng& pt_a_nxt,
	const LatLng& pt_b, const LatLng& pt_b_nxt) const
{
	bool equal = false;
	if (pt_a.approxEqual(pt_b)) // perform quick check if the points are remotely close
	{
		if (pt_a == pt_b) // accurately check that points are the same
		{
			const double bearing_threshold = 20.0; // deg
			if ( abs(pt_a.angle(pt_a_nxt) - pt_b.angle(pt_b_nxt)) < bearing_threshold ) // angle is less than a threshold
			{
				equal = true;
			}
		}
	}
	return equal;
}

/******************************************************/
bool RideIntervalFinderWindow::verifyRoute(
	DataLog& log1, int start_index1, int end_index1,
	DataLog& log2, int start_index2, int end_index2) const
{
	const int num_verification_pts = 10; // we use this many points along route to verify them

	// We select points evenly along route and verify they are all found
	int pts_verified = 0;
	const int increment = (end_index1 - start_index1)/num_verification_pts;
	for (int pt1 = start_index1; pt1 < end_index1; pt1+=increment)
	{
		const LatLng lat_lng1(log1.ltd(pt1), log1.lgd(pt1), PROXIMITY_THD);
		const LatLng lat_lng_nxt1(log1.ltd(pt1+1), log1.lgd(pt1+1), PROXIMITY_THD);
		
		for (int pt2 = start_index2; pt2 < end_index2; ++pt2)
		{
			const LatLng lat_lng2(log2.ltd(pt2), log2.lgd(pt2), PROXIMITY_THD);
			const LatLng lat_lng_nxt2(log2.ltd(pt2+1), log2.lgd(pt2+1), PROXIMITY_THD);

			if (arePointsEqual(lat_lng1, lat_lng_nxt1, lat_lng2, lat_lng_nxt2))
			{
				pts_verified++;
				break;
			}
		}
	}

	const double fraction_verified = (double)pts_verified / (double) num_verification_pts; 

	if ( fraction_verified > 0.7 ) // more than 70% of points found
		return true;
	else
		return false;
}