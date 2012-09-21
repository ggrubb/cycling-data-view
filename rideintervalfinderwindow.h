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
class QStandardItem;

class RideIntervalFinderWindow : public QWidget
{
	Q_OBJECT

 public:
	RideIntervalFinderWindow(GoogleMapWindow* google_map_window, User* user, DataLog* data_log);
	~RideIntervalFinderWindow();

 private slots:
	 // Find all the routes defined by the user
	 void findIntervals();

 private:
	// Create the window and layout the GUI
	void formatTreeView();
	void setModelColumnHeadings(QStandardItemModel& model) const;

	// Parse the given logfile, resulting data is in data_log
	bool parse(const QString filename, DataLog* data_log);

	// Populate table item with interval data
	void populateIntervalData(
		QList<QStandardItem*>& interval_list, 
		DataLog& data_log, 
		int found_start_index, int found_end_index) const;

	// Determine if 2 points on a path are equal
	// pt_a and pt_b are points of interest
	// pt_a_nxt and pt_b_nxt are the following points (for angle estimation)
	bool arePointsEqual(
		const LatLng& pt_a, const LatLng& pt_a_nxt,
		const LatLng& pt_b, const LatLng& pt_b_nxt) const;

	// Verify the route defined between 2 points. Return true if verifed, false otherwise
	bool verifyRoute(
		DataLog& log1, int start_index1, int end_index1,
		DataLog& log2, int start_index2, int end_index2) const;

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