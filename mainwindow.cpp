#include "mainwindow.h"
#include "datalog.h"
#include "googlemap.h"
#include "plotwindow.h"
#include "datastatisticsview.h"
#include "rideselectionwindow.h"
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

#define VERSION_INFO "Version 1.0 (Nov 2011)\n Copyright 2011\n grant.grubb@gmail.com"

/******************************************************/
MainWindow::MainWindow():
QMainWindow()
 {
	createActions();
	createMenus();

	_google_map = new GoogleMap();
	_plot_window = new PlotWindow();
	_stats_view = new DataStatisticsView();
	_data_log = new DataLog();
	_ride_selector = new RideSelectionWindow();
	connect(_ride_selector , SIGNAL(displayRide(DataLog*)),this,SLOT(setRide(DataLog*)));


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
	delete _data_log;
	delete _ride_selector;
}

/******************************************************/
 void MainWindow::open()
 {
	_ride_selector->setLogDirectory("D:/Grant/training logs");
 }

/******************************************************/
void MainWindow::setRide(DataLog* data_log)
{
	// Overlay route in Google maps
	_google_map->displayRide(data_log);

	// Statistical viewer
	_stats_view->displayRide(data_log);

	// Plot 2d curves
	_plot_window->displayRide(data_log, _google_map, _stats_view);
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

	// Define the file to read
	//QString filename("05_04_2011 17_42_07_history.tcx");
	
	//// Do some OpenCV tests
	//cv::Mat img_orig = cv::imread("img.png");
	//std::vector<uchar> buf;
	//std::vector<int> params(2); 
	//params[0] = CV_IMWRITE_JPEG_QUALITY;
	//params[1] = 50;
	//imencode(".jpg", img_orig, buf, params);
	//
	//// Display the data in some way
	////QMessageBox::about(this, tr("Debug print"), tr("Document read ") + QString::number(read_success) + " " + QString::number(overview_data._total_time));
	//buf[100] = 0;
	//buf[150] = 0;

	//cv::Mat buf_m(buf);
	//cv::Mat img_decd = cv::imdecode(buf_m, 1);
	//params[0]= CV_IMWRITE_PNG_COMPRESSION;
	//params[1] = 0;
	//cv::imwrite("img_out.png",img_decd, params);
 }

/******************************************************/
 void MainWindow::createActions()
 {
     _open_act = new QAction(tr("&Open..."), this);
     _open_act->setShortcut(tr("Ctrl+O"));
     connect(_open_act, SIGNAL(triggered()), this, SLOT(open()));

     _exit_act = new QAction(tr("E&xit"), this);
     _exit_act->setShortcut(tr("Ctrl+Q"));
     connect(_exit_act, SIGNAL(triggered()), this, SLOT(close()));

     _about_act = new QAction(tr("&About"), this);
     connect(_about_act, SIGNAL(triggered()), this, SLOT(about()));
 }

/******************************************************/
 void MainWindow::createMenus()
 {
     _file_menu = new QMenu(tr("&File"), this);
     _file_menu->addAction(_open_act);
     _file_menu->addSeparator();
     _file_menu->addAction(_exit_act);

     _help_menu = new QMenu(tr("&Help"), this);
     _help_menu->addAction(_about_act);

     menuBar()->addMenu(_file_menu);
     menuBar()->addMenu(_help_menu);
 }
