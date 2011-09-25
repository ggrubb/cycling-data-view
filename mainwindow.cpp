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

ImageViewer::ImageViewer():
_dom_document("mydocument")
 {
     _image_label = new QLabel;
     _image_label->setBackgroundRole(QPalette::Base);
     _image_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
     _image_label->setScaledContents(true);

     _scroll_area = new QScrollArea;
     _scroll_area->setBackgroundRole(QPalette::Dark);
     _scroll_area->setWidget(_image_label);
     setCentralWidget(_scroll_area);

     createActions();
     createMenus();

     setWindowTitle(tr("Image Viewer"));
     resize(500, 400);
 }

 void ImageViewer::open()
 {
     QString file_name = QFileDialog::getOpenFileName(this,
                                     tr("Open File"), QDir::currentPath());
     if (!file_name.isEmpty()) {
         QImage image(file_name);
         if (image.isNull()) {
             QMessageBox::information(this, tr("Image Viewer"),
                                      tr("Cannot load %1.").arg(file_name));
             return;
         }
         _image_label->setPixmap(QPixmap::fromImage(image));
         _scale_factor = 1.0;

         _print_act->setEnabled(true);
         _fit_to_window_act->setEnabled(true);
         updateActions();

         if (!_fit_to_window_act->isChecked())
             _image_label->adjustSize();
     }
 }

 void ImageViewer::print()
 {
     Q_ASSERT(_image_label->pixmap());
 #ifndef QT_NO_PRINTER
     QPrintDialog dialog(&_printer, this);
     if (dialog.exec()) {
         QPainter painter(&_printer);
         QRect rect = painter.viewport();
         QSize size = _image_label->pixmap()->size();
         size.scale(rect.size(), Qt::KeepAspectRatio);
         painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
         painter.setWindow(_image_label->pixmap()->rect());
         painter.drawPixmap(0, 0, *_image_label->pixmap());
     }
 #endif
 }

 void ImageViewer::zoomIn()
 {
     scaleImage(1.25);
 }

 void ImageViewer::zoomOut()
 {
     scaleImage(0.8);
 }

 void ImageViewer::normalSize()
 {
     _image_label->adjustSize();
     _scale_factor = 1.0;
 }

 void ImageViewer::fitToWindow()
 {
     bool fit_to_window = _fit_to_window_act->isChecked();
     _scroll_area->setWidgetResizable(fit_to_window);
     if (!fit_to_window) {
         normalSize();
     }
     updateActions();
 }

/******************************************************/
 void ImageViewer::about()
 {
	// Define the file to read
	QString filename("05_04_2011 17_42_07_history.tcx");
	DataLog data_log;
	TcxParser parser;
	parser.parse(filename, data_log);

	// Do a QWebView test
	GoogleMap* google_map = new GoogleMap();
	google_map->displayRide(data_log);

	// Do a Qwt test
	PlotWindow* plot_window = new PlotWindow();
	plot_window->displayRide(data_log);

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
	//QMessageBox::about(this, tr("Debug print"), tr("Num bytes ") + QString::number(buf.size()));

	//buf[100] = 0;
	//buf[150] = 0;

	//cv::Mat buf_m(buf);
	//cv::Mat img_decd = cv::imdecode(buf_m, 1);
	//params[0]= CV_IMWRITE_PNG_COMPRESSION;
	//params[1] = 0;
	//cv::imwrite("img_out.png",img_decd, params);
 }

 void ImageViewer::createActions()
 {
     _open_act = new QAction(tr("&Open..."), this);
     _open_act->setShortcut(tr("Ctrl+O"));
     connect(_open_act, SIGNAL(triggered()), this, SLOT(open()));

     _print_act = new QAction(tr("&Print..."), this);
     _print_act->setShortcut(tr("Ctrl+P"));
     _print_act->setEnabled(false);
     connect(_print_act, SIGNAL(triggered()), this, SLOT(print()));

     _exit_act = new QAction(tr("E&xit"), this);
     _exit_act->setShortcut(tr("Ctrl+Q"));
     connect(_exit_act, SIGNAL(triggered()), this, SLOT(close()));

     _zoom_in_act = new QAction(tr("Zoom &In (25%)"), this);
     _zoom_in_act->setShortcut(tr("Ctrl++"));
     _zoom_in_act->setEnabled(false);
     connect(_zoom_in_act, SIGNAL(triggered()), this, SLOT(zoomIn()));

     _zoom_out_act = new QAction(tr("Zoom &Out (25%)"), this);
     _zoom_out_act->setShortcut(tr("Ctrl+-"));
     _zoom_out_act->setEnabled(false);
     connect(_zoom_out_act, SIGNAL(triggered()), this, SLOT(zoomOut()));

     _normal_size_act = new QAction(tr("&Normal Size"), this);
     _normal_size_act->setShortcut(tr("Ctrl+S"));
     _normal_size_act->setEnabled(false);
     connect(_normal_size_act, SIGNAL(triggered()), this, SLOT(normalSize()));

     _fit_to_window_act = new QAction(tr("&Fit to Window"), this);
     _fit_to_window_act->setEnabled(false);
     _fit_to_window_act->setCheckable(true);
     _fit_to_window_act->setShortcut(tr("Ctrl+F"));
     connect(_fit_to_window_act, SIGNAL(triggered()), this, SLOT(fitToWindow()));

     _about_act = new QAction(tr("&About"), this);
     connect(_about_act, SIGNAL(triggered()), this, SLOT(about()));
 }

 void ImageViewer::createMenus()
 {
     _file_menu = new QMenu(tr("&File"), this);
     _file_menu->addAction(_open_act);
     _file_menu->addAction(_print_act);
     _file_menu->addSeparator();
     _file_menu->addAction(_exit_act);

     _view_menu = new QMenu(tr("&View"), this);
     _view_menu->addAction(_zoom_in_act);
     _view_menu->addAction(_zoom_out_act);
     _view_menu->addAction(_normal_size_act);
     _view_menu->addSeparator();
     _view_menu->addAction(_fit_to_window_act);

     _help_menu = new QMenu(tr("&Help"), this);
     _help_menu->addAction(_about_act);

     menuBar()->addMenu(_file_menu);
     menuBar()->addMenu(_view_menu);
     menuBar()->addMenu(_help_menu);
 }

 void ImageViewer::updateActions()
 {
     _zoom_in_act->setEnabled(!_fit_to_window_act->isChecked());
     _zoom_out_act->setEnabled(!_fit_to_window_act->isChecked());
     _normal_size_act->setEnabled(!_fit_to_window_act->isChecked());
 }

 void ImageViewer::scaleImage(double factor)
 {
     Q_ASSERT(_image_label->pixmap());

     _scale_factor *= factor;
     _image_label->resize(_scale_factor * _image_label->pixmap()->size());

     adjustScrollBar(_scroll_area->horizontalScrollBar(), factor);
     adjustScrollBar(_scroll_area->verticalScrollBar(), factor);

     _zoom_in_act->setEnabled(_scale_factor < 3.0);
     _zoom_out_act->setEnabled(_scale_factor > 0.333);
 }

 void ImageViewer::adjustScrollBar(QScrollBar *scroll_bar, double factor)
 {
     scroll_bar->setValue(int(factor * scroll_bar->value()
                             + ((factor - 1) * scroll_bar->pageStep()/2)));
 }