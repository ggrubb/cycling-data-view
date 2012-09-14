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
class LatLng;
class GoogleMapWindow;
class LogDirectorySummary;
class QStandardItemModel;
class QTreeView;

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

	// Determine if 2 points on a path are equal
	// pt_a and pt_b are points of interest
	// pt_a_nxt and pt_b_nxt are the following points (for angle estimation)
	bool arePointsEqual(
		const LatLng& pt_a, const LatLng& pt_a_nxt,
		const LatLng& pt_b, const LatLng& pt_b_nxt);

	bool verifyRoute();
	void formatTreeView();

	TcxParser* _tcx_parser;
	FitParser* _fit_parser;

	DateSelectorWidget* _date_selector_widget;
	QTreeView* _tree;
	QStandardItemModel* _model;

	User* _user;
	DataLog* _current_data_log;
	LogDirectorySummary* _log_dir_summary;

	// Maintain a handle to the Google map window to query it for current user route selection
	GoogleMapWindow* _google_map_window;
};

#endif // RIDEINTERVALFINDERWINDOW_H