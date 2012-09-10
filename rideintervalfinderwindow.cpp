#include "rideintervalfinderwindow.h"
#include "datalog.h"
#include "dateselectorwidget.h"
#include "tcxparser.h"
#include "fitparser.h"
#include "user.h"
#include "logdirectorysummary.h"
#include "latlng.h"

#include <cassert>

#include <QDir.h>
#include <QComboBox.h>
#include <QLabel.h>
#include <QBoxLayout.h>
#include <QProgressDialog.h>
#include <QPushButton.h>
#include <QMessageBox.h>

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
	
	// Layout the GUI
	QVBoxLayout* vlayout = new QVBoxLayout(this);
	vlayout->addWidget(_date_selector_widget);
	vlayout->addWidget(find_intervals_button);

	resize(500,200);
}
	
/******************************************************/
void RideIntervalFinderWindow::setRide(DataLog* data_log, User* user)
{
	_user = user;
	_current_data_log = data_log;

	createWindow();
}

/******************************************************
void GoogleMapCollageWindow::displayRides(const std::vector<QString>& filenames)
{
	_accumulated_points.clear();
	_accumulated_point_extra_info.clear();
	_max_count=0;

	// Create a small progress bar
	QProgressDialog load_progress("Loading log:", "Cancel load", 0, filenames.size()-1, this);
	load_progress.setWindowModality(Qt::WindowModal);
	load_progress.setMinimumDuration(0); //msec
	load_progress.setWindowTitle("RideCollage");

	// Load new log files in the directory
	for (unsigned int i=0; i < filenames.size(); ++i)
	{
		load_progress.setValue(i);
		load_progress.setLabelText("Loading log: " + filenames[i]);
		if (load_progress.wasCanceled())
			break;

		DataLog* data_log = new DataLog;	
		if (parse(filenames[i], data_log))
		{	
			if (data_log->lgdValid() && data_log->ltdValid())
			{
				for (int pt=0; pt < data_log->numPoints(); pt+=4)
				{
					LatLng lat_lng(data_log->ltd(pt), data_log->lgd(pt));
					bool found = false;
					
					int a_pt=_accumulated_points.size()-1; // going backwards is slightly faster because the latest points are at the end (so break quicker)
					while (a_pt >= 0)
					{
						// Check if we have visited this point previously
						if (lat_lng == _accumulated_points[a_pt].first)
						{
							// Only increment frequency if we have not been here in this ride, or it is this ride but more than 60 secs ago
							if (i != _accumulated_point_extra_info[a_pt].first || 
								0)//abs(data_log->time(pt) - _accumulated_point_extra_info[a_pt].second) > 60)
							{
								_accumulated_points[a_pt].second++;
								_accumulated_point_extra_info[a_pt].first = i;
								_accumulated_point_extra_info[a_pt].second = data_log->time(pt);
								if (_accumulated_points[a_pt].second > _max_count)
									_max_count = _accumulated_points[a_pt].second;
							}
							found = true;
							break;
						}
						--a_pt;
					}

					if (!found)
					{
						_accumulated_points.push_back(std::make_pair(lat_lng,1));
						_accumulated_point_extra_info.push_back(std::make_pair(i,data_log->time(pt)));
					}
				}
			}
		}
		delete data_log;
	}
	
	if (_accumulated_points.size() > 0) // we have a valid path to show
	{
		// Create the google map web page
		ostringstream page;
		createPage(page);
		_view->setHtml(QString::fromStdString(page.str()));
		show();
	}
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
	// Create a small progress bar
	QProgressDialog load_progress("Loading log:", "Cancel load", 0, _log_dir_summary->numLogs()-1, this);
	load_progress.setWindowModality(Qt::WindowModal);
	load_progress.setMinimumDuration(0); //msec
	load_progress.setWindowTitle("RideIntervalFinder");

	// Load new log files in the directory
	for (unsigned int i=0; i < _log_dir_summary->numLogs(); ++i)
	{
		const QString filename = _log_dir_summary->log(i)._filename;

		load_progress.setValue(i);
		load_progress.setLabelText("Loading log: " + filename);
		if (load_progress.wasCanceled())
			break;

		const QDate date = _log_dir_summary->log(i).date();

		// Check that the date is within the user selected range
		if (date >= _date_selector_widget->minDate() && date <= _date_selector_widget->maxDate())
		{
			DataLog* data_log = new DataLog;	
			if (parse(filename, data_log))
			{	
				if (data_log->lgdValid() && data_log->ltdValid())
				{
					for (int pt=0; pt < data_log->numPoints(); pt+=4)
					{
						LatLng lat_lng(data_log->ltd(pt), data_log->lgd(pt));
						bool found = false;
						
					}
				}
			}
			delete data_log;
		}
	}

	QMessageBox::information(this, tr("RideIntervalFinder"), tr("here!"));
}