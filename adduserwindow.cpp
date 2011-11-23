#include "adduserwindow.h"
#include "user.h"
 
#include <QLineEdit.h>
#include <QLabel.h>
#include <QSpinBox.h>
#include <QSpinBox.h>
#include <QPushButton.h>
#include <QFileDialog.h>
#include <QGridLayout.h>

/******************************************************/
AddUserWindow::AddUserWindow():
QWidget()
{
	setWindowTitle(tr("Specify Rider"));
	setWindowIcon(QIcon("./resources/rideviewer_head128x128.ico")); 

	_name_input = new QLineEdit();
	_log_directory_input = new QLabel(QDir::homePath());
	_weight_input = new QDoubleSpinBox();
	_bike_weight_input = new QDoubleSpinBox();
	_hr_zone1_input = new QSpinBox();
	_hr_zone2_input = new QSpinBox();
	_hr_zone3_input = new QSpinBox();
	_hr_zone4_input = new QSpinBox();
	_hr_zone5_input = new QSpinBox();

	_weight_input->setRange(10.0,200.0);
	_hr_zone1_input->setRange(50,250);
	_hr_zone2_input->setRange(50,250);
	_hr_zone3_input->setRange(50,250);
	_hr_zone4_input->setRange(50,250);
	_hr_zone5_input->setRange(50,250);

	_hr_zone1_input->setValue(120);
	_hr_zone2_input->setValue(140);
	_hr_zone3_input->setValue(160);
	_hr_zone4_input->setValue(170);
	_hr_zone5_input->setValue(180);

	QLabel* name_label = new QLabel("Name:");
	QLabel* log_directory_label = new QLabel("Logfile Directory:");
	QLabel* weight_label = new QLabel("Weight (kg):");
	QLabel* bike_weight_label = new QLabel("Bike Weight (kg):");
	QLabel* hr_zone1_label = new QLabel("HR Zone 1 - recovery (bpm):");
	QLabel* hr_zone2_label = new QLabel("HR Zone 2 - endurance (bpm):");
	QLabel* hr_zone3_label = new QLabel("HR Zone 3 - tempo (bpm):");
	QLabel* hr_zone4_label = new QLabel("HR Zone 4 - threshold (bpm):");
	QLabel* hr_zone5_label = new QLabel("HR Zone 5 - V02 max (bpm):");

	QPushButton* directory_button = new QPushButton("Select...");
	connect(directory_button, SIGNAL(clicked()), this, SLOT(selectDirectory()));
	
	QPushButton* done_button = new QPushButton("Done");
	connect(done_button, SIGNAL(clicked()), this, SLOT(createRider()));

	QPushButton* cancel_button = new QPushButton("Cancel");
	connect(cancel_button, SIGNAL(clicked()), this, SLOT(cancel()));

	QGridLayout* grid_layout = new QGridLayout(this);

	grid_layout->addWidget(name_label,0,0);
	grid_layout->addWidget(_name_input,0,1);

	grid_layout->addWidget(log_directory_label,1,0);
	grid_layout->addWidget(directory_button,1,1);
	grid_layout->addWidget(_log_directory_input,2,0,1,2);

	grid_layout->addWidget(weight_label,3,0);
	grid_layout->addWidget(_weight_input,3,1);

	grid_layout->addWidget(bike_weight_label,4,0);
	grid_layout->addWidget(_bike_weight_input,4,1);

	grid_layout->addWidget(hr_zone1_label,5,0);
	grid_layout->addWidget(_hr_zone1_input,5,1);

	grid_layout->addWidget(hr_zone2_label,6,0);
	grid_layout->addWidget(_hr_zone2_input,6,1);

	grid_layout->addWidget(hr_zone3_label,7,0);
	grid_layout->addWidget(_hr_zone3_input,7,1);

	grid_layout->addWidget(hr_zone4_label,8,0);
	grid_layout->addWidget(_hr_zone4_input,8,1);

	grid_layout->addWidget(hr_zone5_label,9,0);
	grid_layout->addWidget(_hr_zone5_input,9,1);

	grid_layout->addWidget(done_button,10,0);
	grid_layout->addWidget(cancel_button,10,1);

	show();
}
 
/******************************************************/
AddUserWindow::~AddUserWindow()
{
	delete _name_input;
	delete _log_directory_input;
	delete _weight_input;
	delete _bike_weight_input;
	delete _hr_zone1_input;
	delete _hr_zone2_input;
	delete _hr_zone3_input;
	delete _hr_zone4_input;
	delete _hr_zone5_input;
}

/******************************************************/
void AddUserWindow::setUser(User* user)
{
	_name_input->setText(user->name());
	_log_directory_input->setText(user->logDirectory());
	_weight_input->setValue(user->weight());
	_bike_weight_input->setValue(user->bikeWeight());
	_hr_zone1_input->setValue(user->zone1());
	_hr_zone2_input->setValue(user->zone2());
	_hr_zone3_input->setValue(user->zone3());
	_hr_zone4_input->setValue(user->zone4());
	_hr_zone5_input->setValue(user->zone5());
}

/******************************************************/
void AddUserWindow::selectDirectory()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Select Directory", QDir::homePath(), QFileDialog::ShowDirsOnly);
	_log_directory_input->setText(dir);
	_log_directory_input->update();
}

/******************************************************/
void AddUserWindow::createRider()
{
	User* new_user = new User(
		_name_input->text(),
		_log_directory_input->text(),
		_weight_input->value(),
		_bike_weight_input->value(),
		_hr_zone1_input->value(),
		_hr_zone2_input->value(),
		_hr_zone3_input->value(),
		_hr_zone4_input->value(),
		_hr_zone5_input->value());

	emit riderSelected(new_user);

	this->~AddUserWindow();
}

/******************************************************/
void AddUserWindow::cancel()
{
	this->~AddUserWindow();
}
 