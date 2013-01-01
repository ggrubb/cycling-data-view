#ifndef SPECIFYUSERWINDOW_H
#define SPECIFYUSERWINDOW_H
 
#include <Qwidget.h>

#include <boost/shared_ptr.hpp>

class QLineEdit;
class QLabel;
class QDoubleSpinBox;
class QDoubleSpinBox;
class QSpinBox;
class User;

class SpecifyUserWindow : public QWidget
{
	Q_OBJECT
public:
	SpecifyUserWindow();
	~SpecifyUserWindow();

	void setUser(boost::shared_ptr<User> user);

signals:
	void userSelected(boost::shared_ptr<User> user);

private slots:
	void selectDirectory();
	void createRider();
	void cancel();

private:
	QLineEdit* _name_input;
	QLabel* _log_directory_input;
	QDoubleSpinBox* _weight_input;
	QSpinBox* _hr_zone1_input;
	QSpinBox* _hr_zone2_input;
	QSpinBox* _hr_zone3_input;
	QSpinBox* _hr_zone4_input;
	QSpinBox* _hr_zone5_input;
};
 
#endif // SPECIFYUSERWINDOW_H