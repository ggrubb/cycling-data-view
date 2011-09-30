#include "mainwindow.h"
#include "tcxparser.h"
#include "datalog.h"
#include "googlemap.h"
#include "plotwindow.h"

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
#include <highgui.h>

/******************************************************/
MainWindow::MainWindow()
 {
	createActions();
	createMenus();

	_parser = new TcxParser();
	_google_map = new GoogleMap();
	_plot_window = new PlotWindow();

	setWindowTitle(tr("Cycling Data View"));
	resize(500, 400);
 }

/******************************************************/
MainWindow::~MainWindow()
{
	delete _parser;
	delete _google_map;
	delete _plot_window;
}

/******************************************************/
 void MainWindow::open()
 {
     QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(),tr("TCX Files (*.tcx)"));
     if (!filename.isEmpty()) {

		DataLog data_log;
		if (!_parser->parse(filename, data_log)) 
		{
			QMessageBox::information(this, tr("Cycling Data View"),tr("Cannot load %1.").arg(filename));
			return;
		}
		
		// Overlay route in Google maps
		_google_map->displayRide(data_log);

		// Plot 2d curves
		_plot_window->displayRide(data_log, _google_map);
     }
 }


/******************************************************/
 void MainWindow::about()
 {
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
	QMessageBox::about(this, tr("Cycling Data View"), tr("Version 0.1\nSept 2011\nGrubtronic Software"));

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
