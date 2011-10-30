#include "mainwindow.h"
#include "datalog.h"
#include "googlemap.h"
#include "plotwindow.h"
#include "datastatisticsview.h"
#include "rideselectionwindow.h"
#include "user.h"
#include "aboutwindow.h"

#include <stdio.h>
#include <iostream>
#include <qtgui/qlabel>
#include <qtgui/qscrollarea>
#include <qtgui/qscrollbar>
#include <qtgui/qaction>
#include <qtgui/qmenu>
#include <qtgui/qmenubar>
#include <qtgui/qfiledialog>
#include <qtgui/qmessagebox>
#include <qtgui/qpainter>
#include <qtgui/qapplication>
#include <qtgui/qgridlayout>
#include <qtgui/qdesktopwidget>
#include <qtgui/qbitmap>
#include <qtgui/qlistwidget>
#include <qtgui/qinputdialog>

#define VERSION_INFO "Version 1.0 (Nov 2011)\n Copyright 2011\n grant.grubb@gmail.com"
#define USER_DIRECTORY "/users/"

/******************************************************/
MainWindow::MainWindow():
QMainWindow()
 {
	createActions();
	createMenus();

	_current_user = new User();

	_google_map = new GoogleMap();
	_stats_view = new DataStatisticsView();
	_plot_window = new PlotWindow(_google_map, _stats_view);

	_ride_selector = new RideSelectionWindow();
	connect(_ride_selector , SIGNAL(displayRide(DataLog*)),this,SLOT(setRide(DataLog*)));
	connect(_ride_selector , SIGNAL(displayLap(int)),this,SLOT(setLap(int)));

	QWidget* central_widget = new QWidget;
	QGridLayout* glayout1 = new QGridLayout(central_widget);
	glayout1->addWidget(_ride_selector,0,0);
	glayout1->addWidget(_plot_window,0,1);
	glayout1->addWidget(_stats_view,1,0);
	glayout1->addWidget(_google_map,1,1);

	setCentralWidget(central_widget);
	setWindowTitle(tr("RideViewer"));
	setWindowIcon(QIcon("./resources/rideviewer_head128x128.ico")); 
	
	// Set to full screen size
	QSize size = qApp->desktop()->size();
	resize(size.boundedTo(QSize(1280,700)));
	move(0,0);
 }

/******************************************************/
MainWindow::~MainWindow()
{
	delete _google_map;
	delete _plot_window;
	delete _stats_view;
	delete _ride_selector;
	delete _current_user;
}

/******************************************************/
void MainWindow::setUser()
{
	QDir directory;
	QStringList filter;
	filter << "*.usr";
	directory.setNameFilters(filter);
	directory.setPath(QDir::currentPath() + USER_DIRECTORY);
	QStringList user_filenames = directory.entryList();

	if (user_filenames.length() > 0)
	{
		// Load the users from file
		std::vector<User*> users;
		QStringList user_names;
		for (int i = 0; i < user_filenames.length(); ++i)
		{
			User* user = new User;
			user->readFromFile(QDir::currentPath() + USER_DIRECTORY + user_filenames[i]);
			users.push_back(user);

			user_names.append(user->name());
		}

		// Prompt to select a user
		QString	user_name = QInputDialog::getItem(this, tr("User Selection"), tr("Select User:"), user_names, 0, false, 0, 0);
		
		// Set the selected user
		_current_user = users[user_names.indexOf(user_name)];
		_ride_selector->setLogDirectory(_current_user->logDirectory());
	}
	else
	{
		// Say the user needs to add a user
	}
}

/******************************************************/
void MainWindow::addUser()
{
	User user("Grant","D:/Grant/projects/cycling-data-view/Debug/test logs2",64.0,8.5, 140, 155,170,180, 190);
	user.writeToFile("grant.usr");
}

/******************************************************/
void MainWindow::setRide(DataLog* data_log)
{
	// Plot 2d curves (important to be called first since it is responsible for signal filtering)
	_plot_window->displayRide(data_log);

	// Overlay route in Google maps
	_google_map->displayRide(data_log);

	// Statistical viewer
	_stats_view->displayRide(data_log);
}

/******************************************************/
void MainWindow::setLap(int lap_index)
{
	// Zoom plots to lap (this automatically propogates to map and stats)
	_plot_window->displayLap(lap_index);
}

/******************************************************/
 void MainWindow::about()
 {
	 // Create and display about window
	QPixmap about_image("resources/about_screen.png"); 
	AboutWindow* about = new AboutWindow(about_image);
	about->setMessageRect(QRect::QRect(200, 80, 150, 50), Qt::AlignRight); // setting the message position
	 
	QFont about_font;
	about_font.setFamily("Arial");
	about_font.setBold(true);
	about_font.setPixelSize(9);
	about_font.setStretch(125);
	 
	about->setFont(about_font);
	about->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::SplashScreen);
	about->show();
	about->showVersionInfo(tr(VERSION_INFO));
 }

/******************************************************/
 void MainWindow::createActions()
 {
     _set_act = new QAction(tr("Select..."), this);
     connect(_set_act, SIGNAL(triggered()), this, SLOT(setUser()));

	 _add_act = new QAction(tr("Add..."), this);
     connect(_add_act, SIGNAL(triggered()), this, SLOT(addUser()));

     _exit_act = new QAction(tr("Exit"), this);
     connect(_exit_act, SIGNAL(triggered()), this, SLOT(close()));

     _about_act = new QAction(tr("About"), this);
     connect(_about_act, SIGNAL(triggered()), this, SLOT(about()));
 }

/******************************************************/
 void MainWindow::createMenus()
 {
     _file_menu = new QMenu(tr("&User"), this);
     _file_menu->addAction(_set_act);
     _file_menu->addAction(_add_act);
     _file_menu->addSeparator();
     _file_menu->addAction(_exit_act);

     _help_menu = new QMenu(tr("&Help"), this);
     _help_menu->addAction(_about_act);

     menuBar()->addMenu(_file_menu);
     menuBar()->addMenu(_help_menu);
 }
