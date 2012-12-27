#ifndef DATASTATISTICSVIEW_H
#define DATASTATISTICSVIEW_H

#include <Qwidget.h>

#include <boost/shared_ptr.hpp>

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

	void displayRide(DataLog* data_log, boost::shared_ptr<User> user);

 public slots:
	void displayCompleteRideStats();
	void displaySelectedRideStats(int idx_start, int idx_end);
	void moveSelection(int delta_idx);
	void deleteSelection();
	
 private:
	void clearTable();
	void clearTotalsColumn();
	void clearSelectionColumn();

	QTableWidget* _table;
	QLabel* _head_label;
	DataLog* _data_log;
	boost::shared_ptr<User> _user;

	// The start index of selection to highlight
	int _selection_begin_idx;
	// The end index of selection to highlight
	int _selection_end_idx;
 };

#endif // DATASTATISTICSVIEW_H