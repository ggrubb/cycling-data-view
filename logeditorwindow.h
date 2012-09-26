#ifndef LOGEDITORWINDOW_H
#define LOGEDITORWINDOW_H

#include <Qwidget.h>

class QTableWidget;
class QLabel;
class DataLog;
class User;
class QComboBox;
class QDoubleSpinBox;

class LogEditorWindow : public QWidget
 {
	 Q_OBJECT
 public:
	LogEditorWindow(User* user, DataLog* data_log);
	~LogEditorWindow();

 public slots:
	void displayRide();
	
 private:
	void clearTable();
	void clearTotalsColumn();

	QTableWidget* _table;
	QLabel* _head_label;
	QComboBox* _field_selection;
	QComboBox* _equality_selection;
	QDoubleSpinBox* _search_value;

	std::vector<int> _search_result_indecies;

	DataLog* _data_log;
	User* _user;
 };

#endif // LOGEDITORWINDOW_H