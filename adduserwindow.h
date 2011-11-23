#ifndef ADDUSERWINDOW_H
#define ADDUSERWINDOW_H
 
#include <Qwidget.h>

class QLineEdit;
class QLabel;
class QDoubleSpinBox;
class QDoubleSpinBox;
class QSpinBox;
class User;

class AddUserWindow : public QWidget
{
	Q_OBJECT
public:
	AddUserWindow();
	~AddUserWindow();

	void setUser(User* user);

signals:
	void riderSelected(User* user);

private slots:
	void selectDirectory();
	void createRider();
	void cancel();

private:
	QLineEdit* _name_input;
	QLabel* _log_directory_input;
	QDoubleSpinBox* _weight_input;
	QDoubleSpinBox* _bike_weight_input;
	QSpinBox* _hr_zone1_input;
	QSpinBox* _hr_zone2_input;
	QSpinBox* _hr_zone3_input;
	QSpinBox* _hr_zone4_input;
	QSpinBox* _hr_zone5_input;
};
 
#endif // ADDUSERWINDOW_H