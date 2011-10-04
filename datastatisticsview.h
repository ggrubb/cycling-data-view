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
	void clearTable();
	void clearTotalsColumn();
	void clearSelectionColumn();
public slots:
	void setSelection(int idx_start, int idx_end);

 private:

	QTableWidget* _table;
	DataLog* _data_log;
 };

#endif // DATASTATISTICSVIEW_H