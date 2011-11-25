#ifndef DATASTATISTICSVIEW_H
#define DATASTATISTICSVIEW_H

#include <Qwidget.h>

class QTableWidget;
class QLabel;
class DataLog;
class User;

class DataStatisticsWindow : public QWidget
 {
	 Q_OBJECT
 public:
	DataStatisticsWindow();
	~DataStatisticsWindow();

	void displayRide(DataLog* data_log, User* user);

 public slots:
	void displayCompleteRideStats();
	void displaySelectedRideStats(int idx_start, int idx_end);
	void deleteSelection();
	
 private:
	void clearTable();
	void clearTotalsColumn();
	void clearSelectionColumn();

	QTableWidget* _table;
	QLabel* _head_label;
	DataLog* _data_log;
	User* _user;
 };

#endif // DATASTATISTICSVIEW_H