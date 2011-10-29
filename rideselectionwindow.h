#ifndef RIDESELECTIONWINDOW_H
#define RIDESELECTIONWINDOW_H

#include <qtgui/qwidget>

class QTreeView;
class QStandardItemModel;
class TcxParser;
class DataLog;
class QModelIndex;
class LogDirectorySummary;

class RideSelectionWindow : public QWidget
 {
	 Q_OBJECT
 public:
	RideSelectionWindow();
	~RideSelectionWindow();

	void setLogDirectory(const QString& path);

 signals:
	void displayRide(DataLog* data_log);
	void displayLap(int lap_index);

private slots:
	void rideSelected(const QModelIndex& index);

 private:
	void populateTableWithRides();

	QTreeView* _tree;
	QStandardItemModel* _model;
	
	TcxParser* _parser;

	std::vector<DataLog*> _data_logs; // cached logs currently loaded
	DataLog* _current_data_log;
	LogDirectorySummary* _log_dir_summary;
 };

#endif // RIDESELECTIONWINDOW_H