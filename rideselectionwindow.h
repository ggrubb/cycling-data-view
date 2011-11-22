#ifndef RIDESELECTIONWINDOW_H
#define RIDESELECTIONWINDOW_H

#include <qtgui/qwidget>

class QTreeView;
class QStandardItemModel;
class QModelIndex;
class QLabel;
class TcxParser;
class DataLog;
class User;
class LogDirectorySummary;

class RideSelectionWindow : public QWidget
 {
	 Q_OBJECT
 public:
	RideSelectionWindow();
	~RideSelectionWindow();

	void setUser(User* user);

 signals:
	void displayRide(DataLog* data_log);
	void displayLap(int lap_index);

private slots:
	void rideSelected(const QModelIndex& index);

 private:
	void populateTableWithRides();
	void formatTreeView();

	QTreeView* _tree;
	QStandardItemModel* _model;
	QLabel* _head_label;
	
	TcxParser* _parser;

	DataLog* _current_data_log;
	LogDirectorySummary* _log_dir_summary;
 };

#endif // RIDESELECTIONWINDOW_H