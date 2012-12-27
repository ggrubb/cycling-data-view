#ifndef LOGEDITORWINDOW_H
#define LOGEDITORWINDOW_H

#include <Qwidget.h>

#include <boost/shared_ptr.hpp>

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
	LogEditorWindow(boost::shared_ptr<User> user, boost::shared_ptr<DataLog> data_log);
	~LogEditorWindow();

 signals:
	void dataLogUpdated(boost::shared_ptr<DataLog>);

 public slots:
	void displayRide();
	void find();
	void clear();
	void next();

	void save();
	void split();
	
 private:
	bool searchComparison(double left, double right);
	
	// GUI members
	QTableWidget* _table;
	QLabel* _head_label;
	QComboBox* _field_selection;
	QComboBox* _equality_selection;
	QDoubleSpinBox* _search_value;

	// Indecies for scrolling through search results
	std::vector<int> _search_result_indecies;
	int	_search_result_index;

	// The data and user
	boost::shared_ptr<DataLog> _data_log;
	boost::shared_ptr<User> _user;
 };

#endif // LOGEDITORWINDOW_H