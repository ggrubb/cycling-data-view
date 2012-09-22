#ifndef LOGEDITORWINDOW_H
#define LOGEDITORWINDOW_H

#include <Qwidget.h>

class QTableWidget;
class QLabel;
class DataLog;
class User;

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
	DataLog* _data_log;
	User* _user;
 };

#endif // LOGEDITORWINDOW_H