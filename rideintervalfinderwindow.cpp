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

using namespace std;

/******************************************************/
RideIntervalFinderWindow::RideIntervalFinderWindow(
	GoogleMapWindow* google_map_window):
QWidget(),
_google_map_window(google_map_window)
{
	setWindowTitle("RideIntervalFinder");
	setWindowIcon(QIcon("./resources/rideviewer_head128x128.ico")); 

	_tcx_parser = new TcxParser();
	_fit_parser = new FitParser();
}

/******************************************************/
RideIntervalFinderWindow::~RideIntervalFinderWindow()
{}

/******************************************************/
void RideIntervalFinderWindow::createWindow()
{
	// @todo: clean up this pointer mess!
	assert(_user);
	
	// Load the log directory summary from file
	_log_dir_summary = new LogDirectorySummary(_user->logDirectory());
	_log_dir_summary->readFromFile();
	_date_selector_widget = new DateSelectorWidget(_log_dir_summary->firstLog().date(),_log_dir_summary->lastLog().date());

	QPushButton* find_intervals_button = new QPushButton("Find Intervals");
	connect(find_intervals_button, SIGNAL(clicked()),this,SLOT(findIntervals()));

	// Create the tree view for displaying found intervals
	QStandardItemModel* model = new QStandardItemModel;
	QStandardItem* header0 = new QStandardItem(QString("Date"));
	QStandardItem* header1 = new QStandardItem(QString("Time (min)"));
	QStandardItem* header2 = new QStandardItem(QString("Dist (km)"));
	QStandardItem* header3 = new QStandardItem(QString("Mean Speed (km/h)"));
	QStandardItem* header4 = new QStandardItem(QString("Mean HR(bpm)"));
	model->setHorizontalHeaderItem(0,header0);
	model->setHorizontalHeaderItem(1,header1);
	model->setHorizontalHeaderItem(2,header2);
	model->setHorizontalHeaderItem(3,header3);
	model->setHorizontalHeaderItem(4,header4);

	_tree = new QTreeView(this);
	_tree->setModel(model);
	formatTreeView();
	_tree->show();

	// Layout the GUI
	QVBoxLayout* vlayout = new QVBoxLayout(this);
	vlayout->addWidget(_date_selector_widget);
	vlayout->addWidget(find_intervals_button);
	vlayout->addWidget(_tree);

	resize(500,500);
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
	_tree->sortByColumn(1,Qt::DescendingOrder);
	_tree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
	
/******************************************************/
void RideIntervalFinderWindow::setRide(DataLog* data_log, User* user)
{
	_user = user;
	_current_data_log = data_log;

	createWindow();
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

#include <fstream>
#include <string>
#define PROXIMITY_THD 15.0 // meters

/******************************************************/
void RideIntervalFinderWindow::findIntervals()
{
	//std::ofstream file("debug.txt", std::ios_base::binary);
    //std::string file_string;

	// Create a new modle view
	_model = new QStandardItemModel;
	QStandardItem *parent_item = _model->invisibleRootItem();

	QStandardItemModel* model = new QStandardItemModel;
	QStandardItem* header0 = new QStandardItem(QString("Date"));
	QStandardItem* header1 = new QStandardItem(QString("Time (min)"));
	QStandardItem* header2 = new QStandardItem(QString("Dist (km)"));
	QStandardItem* header3 = new QStandardItem(QString("Mean Speed (km/h)"));
	QStandardItem* header4 = new QStandardItem(QString("Mean HR(bpm)"));
	_model->setHorizontalHeaderItem(0,header0);
	_model->setHorizontalHeaderItem(1,header1);
	_model->setHorizontalHeaderItem(2,header2);
	_model->setHorizontalHeaderItem(3,header3);
	_model->setHorizontalHeaderItem(4,header4);

	_tree->setModel(_model);
	formatTreeView();

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
	for (int i=0; i < _log_dir_summary->numLogs(); ++i)
	{
		const QString filename = _log_dir_summary->log(i)._filename;

		//file << "Searching " << filename.toStdString() << std::endl;

		load_progress.setValue(i);
		load_progress.setLabelText("Loading log: " + filename);
		if (load_progress.wasCanceled())
			break;

		const QDate date = _log_dir_summary->log(i).date();

		
		// Check that the date is within the user selected range
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

					const int increment = 1; // jump 1 points at a time (speed-up)
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
							if (verifyRoute())
							{
								const LatLng lat_lng_st(data_log->ltd(found_start_index), data_log->lgd(found_start_index), PROXIMITY_THD);
								const LatLng lat_lng_st_nxt(data_log->ltd(found_start_index+1), data_log->lgd(found_start_index+1), PROXIMITY_THD);
					
								const LatLng lat_lng_ed(data_log->ltd(found_end_index), data_log->lgd(found_end_index), PROXIMITY_THD);
								const LatLng lat_lng_ed_nxt(data_log->ltd(found_end_index+1), data_log->lgd(found_end_index+1), PROXIMITY_THD);
					
							/*	QString dbg_string_start =  data_log->dateString() + 
								" time " + QString::number(data_log->time(found_start_index)) +
								" distance " + QString::number(start_lat_lng.dist(lat_lng_st)) + 
								" angle1 " + QString::number(lat_lng_st.angle(lat_lng_st_nxt)) +
								" angle2 " + QString::number(start_lat_lng.angle(start_lat_lng_nxt));

								QString dbg_string_end =  tr("                    ") + 
								" distance " + QString::number(end_lat_lng.dist(lat_lng_ed)) + 
								" angle1 " + QString::number(lat_lng_ed.angle(lat_lng_ed_nxt)) +
								" angle2 " + QString::number(end_lat_lng.angle(end_lat_lng_nxt));

								QString dbg_string_selected = tr("Selected: ") + data_log->dateString() +
									" distance " + QString::number(_current_data_log->dist(end_index) - _current_data_log->dist(start_index)) +
									" time " + QString::number(_current_data_log->time(end_index) - _current_data_log->time(start_index));
								
								QString dbg_string_found = tr("Found: ") + data_log->dateString() +
									" distance " + QString::number(data_log->dist(found_end_index) - data_log->dist(found_start_index)) +
									" time " + QString::number(data_log->time(found_end_index) - data_log->time(found_start_index));

								file << dbg_string_selected.toStdString() << "  " << dbg_string_found.toStdString() << std::endl;
*/
								// Populate the model view

								// Data and time
								QString date = data_log->dateString();
								date.chop(3); // remove seconds
								QStandardItem *ride_name = new QStandardItem(date);
								ride_name->setFlags(ride_name->flags() & ~Qt::ItemIsEditable);

								// Ride time length
								QStandardItem *ride_time = new QStandardItem(DataProcessing::minsFromSecs(data_log->time(found_end_index) - data_log->time(found_start_index)));
								ride_time->setFlags(ride_time->flags() & ~Qt::ItemIsEditable);

								QList<QStandardItem*> ride_list;
								ride_list << ride_name << ride_time;
								parent_item->appendRow(ride_list);

								_tree->setModel(_model);
							}
							start_found = false;
							end_found = false;
						}
					}
				}
			}
			delete data_log;
		}
	}

	QMessageBox::information(this, tr("RideIntervalFinder"), tr("finished searching!"));
	//file.close();
}

/******************************************************/
bool RideIntervalFinderWindow::arePointsEqual(
	const LatLng& pt_a, const LatLng& pt_a_nxt,
	const LatLng& pt_b, const LatLng& pt_b_nxt)
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
bool RideIntervalFinderWindow::verifyRoute()
{
	return true;
}