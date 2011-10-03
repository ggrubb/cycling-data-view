#ifndef DATASTATISTICSVIEW_H
#define DATASTATISTICSVIEW_H

#include <qtgui/qwidget>

class QTableWidget;

class DataStatisticsView : public QWidget
 {
 public:
	DataStatisticsView();
	~DataStatisticsView();

	void setTotals();
	void clearTable();
	void clearTotals();
	void clearSelection();

 private:

	QTableWidget* _table;
 };

#endif // DATASTATISTICSVIEW_H