#ifndef RIDESELECTIONWINDOW_H
#define RIDESELECTIONWINDOW_H

#include <qtgui/qwidget>

class QTreeView;
class QStandardItemModel;
class QDir;
class TcxParser;
class DataLog;
class QModelIndex;

class RideSelectionWindow : public QWidget
 {
	 Q_OBJECT
 public:
	RideSelectionWindow();
	~RideSelectionWindow();

	void setLogDirectory(const QString& path);

 signals:
	void displayRide(DataLog* data_log);

private slots:
	void rideSelected(const QModelIndex& index);

 private:
	void populateWithRides(const std::vector<DataLog*>& data_logs);

	QTreeView* _tree;
	QStandardItemModel* _model;
	QDir* _log_directory;
	std::vector<DataLog*> _data_logs;
	TcxParser* _parser;
 };

#endif // RIDESELECTIONWINDOW_H