#ifndef RIDEINTERVALFINDERWINDOW_H
#define RIDEINTERVALFINDERWINDOW_H

#include <qtxml/qdomdocument>
#include <QWidget.h>
#include <QMap.h>

class DataLog;
class TcxParser;
class FitParser;
class DateSelectorWidget;
class User;
class GoogleMapWindow;
class LogDirectorySummary;

class RideIntervalFinderWindow : public QWidget
{
	Q_OBJECT

 public:
	RideIntervalFinderWindow(GoogleMapWindow* google_map_window);
	~RideIntervalFinderWindow();

	void setRide(DataLog* data_log, User* user);

 private slots:
	 void findIntervals();

 private:
	void createWindow();
	bool parse(const QString filename, DataLog* data_log);

	TcxParser* _tcx_parser;
	FitParser* _fit_parser;

	DateSelectorWidget* _date_selector_widget;

	User* _user;
	DataLog* _current_data_log;
	LogDirectorySummary* _log_dir_summary;

	// Maintain a handle to the Google map window to query it for current user route selection
	GoogleMapWindow* _google_map_window;
};

#endif // RIDEINTERVALFINDERWINDOW_H