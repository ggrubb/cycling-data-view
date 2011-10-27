#ifndef DATASTATISTICSVIEW_H
#define DATASTATISTICSVIEW_H

#include <qtgui/qwidget>

class QTableWidget;
class DataLog;

class DataStatisticsView : public QWidget
 {
	 Q_OBJECT
 public:
	DataStatisticsView();
	~DataStatisticsView();

	void displayRide(DataLog* data_log);
	void displayLap(int lap_index);

 public slots:
	void displayCompleteRideStats();
	void displaySelectedRideStats(int idx_start, int idx_end);
	void deleteSelection();
	
 private:
	void clearTable();
	void clearTotalsColumn();
	void clearSelectionColumn();

	QTableWidget* _table;
	DataLog* _data_log;
 };

#endif // DATASTATISTICSVIEW_H