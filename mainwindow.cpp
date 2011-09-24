#include "mainwindow.h"

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
#include <qtwebkit/qwebview>
#include <qtwebkit/qwebpage>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_picker_machine.h>
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
struct RideOverviewData
{
	QString _name;
	QString _date;
	float _total_time;
	float _total_dist;
	float _max_speed;
	float _max_heart_rate;
	float _max_cadence;
	float _avg_speed;
	float _avg_heart_rate;
	float _avg_cadence;
};

/******************************************************/
struct RideDetailData
{
	int _num_points;
	std::vector<double> _time;
	std::vector<double> _lat;
	std::vector<double> _long;
	std::vector<double> _alt;
	std::vector<double> _dist;
	std::vector<int> _heart_rate;
	std::vector<int> _cadence;
	std::vector<double> _speed;

	void resize(int size)
	{
		_time.resize(size);
		_lat.resize(size);
		_long.resize(size);
		_alt.resize(size);
		_dist.resize(size);
		_heart_rate.resize(size);
		_cadence.resize(size);
		_speed.resize(size);
	};
};

/******************************************************/
void parseRideOverview(QDomElement& doc, RideOverviewData& overview_data)
{
	QDomElement lap = doc.firstChild().firstChild().firstChildElement("Lap");
	QDomElement	total_time_seconds = lap.firstChildElement("TotalTimeSeconds");
	QDomElement	distance_meters = lap.firstChildElement("DistanceMeters");
	QDomElement	max_speed = lap.firstChildElement("MaximumSpeed");
	QDomNode	max_heart_rate = lap.firstChildElement("MaximumHeartRateBpm").firstChild();
	QDomNode	max_cadence = lap.firstChildElement("Extensions").firstChild().firstChild().nextSibling();
	QDomNode	avg_heart_rate = lap.firstChildElement("AverageHeartRateBpm").firstChild();
	QDomNode	avg_speed = lap.firstChildElement("Extensions").firstChild().firstChild();
	QDomElement	avg_cadence = lap.firstChildElement("Cadence");

	overview_data._name = QString("Ride X");
	overview_data._date = lap.attributes().item(0).nodeValue();
	overview_data._total_time = total_time_seconds.firstChild().nodeValue().toFloat();
	overview_data._total_dist = distance_meters.firstChild().nodeValue().toFloat();
	overview_data._avg_cadence = avg_cadence.firstChild().nodeValue().toFloat();
	overview_data._avg_heart_rate = avg_heart_rate.firstChild().nodeValue().toFloat();
	overview_data._avg_speed = avg_speed.firstChild().nodeValue().toFloat();
	overview_data._max_speed = max_speed.firstChild().nodeValue().toFloat();
	overview_data._max_heart_rate = max_heart_rate.firstChild().nodeValue().toFloat();
	overview_data._max_cadence = -1;

}

/******************************************************/
void parseRideDetails(QDomElement& doc, RideDetailData& detail_data)
{
	QDomNode track = doc.firstChild().firstChild().firstChildElement("Lap").firstChildElement("Track");
	
	int track_point_idx = 0;
	int total_track_points = 0;
	int num_empty_track_points = 0;
	while (!track.isNull())
	{
		QDomNode track_point = track.firstChild();

		// Count the number of track points to allocate sufficient space
		int num_track_pts = 0;
		while (!track_point.isNull())
		{
			num_track_pts++;
			track_point = track_point.nextSibling();
		}
		total_track_points += num_track_pts;

		// Allocate space
		detail_data._num_points = total_track_points;
		detail_data.resize(total_track_points);

		// Now extract all the data
		track_point = track.firstChild();
		for (int i=0; i < num_track_pts; ++i)
		{
			QStringList tmp_sl = track_point.firstChildElement("Time").firstChild().nodeValue().split('T');
			if (tmp_sl.size() > 1) // check to ensure the time format is as expected
			{
				QString tmp_s = tmp_sl.at(1);
				tmp_s.chop(1);
				QStringList time_strings = tmp_s.split(':');
				detail_data._time[track_point_idx] = time_strings.at(0).toInt()*3600 + time_strings.at(1).toInt()*60 + time_strings.at(2).toInt();
				detail_data._speed[track_point_idx] = track_point.firstChildElement("Extensions").firstChild().firstChild().nodeValue().toFloat();
				detail_data._long[track_point_idx] = track_point.firstChildElement("Position").firstChildElement("LongitudeDegrees").firstChild().nodeValue().toFloat();
				detail_data._lat[track_point_idx] = track_point.firstChildElement("Position").firstChildElement("LatitudeDegrees").firstChild().nodeValue().toFloat();
				detail_data._heart_rate[track_point_idx] = track_point.firstChildElement("HeartRateBpm").firstChild().firstChild().nodeValue().toFloat();
				detail_data._dist[track_point_idx] = track_point.firstChildElement("DistanceMeters").firstChild().nodeValue().toFloat();
				detail_data._cadence[track_point_idx] = track_point.firstChildElement("Cadence").firstChild().nodeValue().toFloat();
				detail_data._alt[track_point_idx] = track_point.firstChildElement("AltitudeMeters").firstChild().nodeValue().toFloat();
			}
			track_point = track_point.nextSibling();

			// Sometimes the xml contains empty trackpoint nodes, with just a time, but no data.
			// Here we check this, and don't increment counter if the trackpoint was empty
			bool valid_track_point = true;
			if (detail_data._long[track_point_idx] == 0 && detail_data._lat[track_point_idx] == 0)
			{
				valid_track_point = false;
				num_empty_track_points++;
			}

			if (valid_track_point)
				track_point_idx++;
		}

		track = track.nextSibling();
	}

	// Resize to account for empty trackpoints
	total_track_points -= num_empty_track_points;
	detail_data.resize(total_track_points);
	detail_data._num_points = total_track_points;

	// Clean up the ride time
	for (int i=detail_data._num_points-1; i >= 0; --i)
	{
		detail_data._time[i] = detail_data._time[i] - detail_data._time[0];
	}

}

/******************************************************/
std::string createPolyline(const RideDetailData& ride_data)
{
	std::ostringstream stream;
	int i=0;
	while (i < ride_data._num_points-1)
	{
		stream << "new google.maps.LatLng(" << ride_data._lat[i] << "," << ride_data._long[i] << ")," << std::endl;
		++i;
	}
	stream << "new google.maps.LatLng(" << ride_data._lat[i] << "," << ride_data._long[i] << ")";

	return stream.str();
}

/******************************************************/
void createPage(std::ostringstream& page, const RideDetailData& ride_data)
{
	using namespace std;
	ostringstream oss;
    oss.precision(6);
    oss.setf(ios::fixed,ios::floatfield);

	oss << "<!DOCTYPE html>" << endl
		<< "<html>" << endl
		<< "<head>" << endl
		<< "<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />" << endl
		<< "<style type=\"text/css\">" << endl
		<< "html { height: 100% }" << endl
		<< "body { height: 100%; margin: 0; padding: 0 }" << endl
		<< "#map_canvas { height: 100% }" << endl
		<< "</style>" << endl
		<< "<script type=\"text/javascript\"" << endl
	    << "src=\"http://maps.googleapis.com/maps/api/js?v=3.1&sensor=true\">" << endl
		<< "</script>" << endl
		<< "<script type=\"text/javascript\">" << endl
		<< "function initialize() {" << endl
		<< "var latlng = new google.maps.LatLng(" << ride_data._lat[0] << "," << ride_data._long[0] << ");" << endl
		<< "var myOptions = {" << endl
		<< "zoom: 10," << endl
		<< "center: latlng," << endl
		<< "mapTypeId: google.maps.MapTypeId.ROADMAP" << endl
		<< "};" << endl
		<< "var map = new google.maps.Map(document.getElementById(\"map_canvas\"), myOptions);" << endl

		<< "var ride_coords = [" << endl
		<< createPolyline(ride_data) << endl
		<< "];" << endl

		<< "var ride_path = new google.maps.Polyline({" << endl
		<< "path: ride_coords," << endl
		<< "strokeColor: \"#FF0000\"," << endl
		<< "strokeOpacity: 1.0," << endl
		<< "strokeWeight: 2" << endl
		<< "});" << endl

		<< "ride_path.setMap(map);" << endl

		<< "}" << endl
		<< "</script>" << endl
		<< "</head>" << endl
		<< "<body onload=\"initialize()\">" << endl
		<< "<div id=\"map_canvas\" style=\"width:100%; height:100%\"></div>" << endl
		<< "</body>" << endl
		<< "</html>" << endl;

	page << oss.str();
}

/******************************************************/
void ImageViewer::plotSelection(const QPointF& point)
{
	std::cout << point.x() << " " << point.y() << std::endl;
}

/******************************************************/
class ChromePage : public QWebPage
{
	virtual QString userAgentForUrl(const QUrl& url) const {
	 return "Chrome/1.0";
	}
};

/******************************************************/
 void ImageViewer::about()
 {
	// Define the file to read
	QString error_msg;
	int error_line, error_column;
	QFile file("05_04_2011 17_42_07_history.tcx");
	bool read_success = _dom_document.setContent(&file, &error_msg, &error_line, &error_column);
	QDomElement doc = _dom_document.documentElement();

	// Extract the data
	RideOverviewData overview_data;
	RideDetailData detail_data;
	parseRideOverview(doc, overview_data);
	parseRideDetails(doc, detail_data);

	// Do a QWebView test
	QString html_file_name(QDir::tempPath());
    html_file_name.append("/maps.html");
    QFile html_file(html_file_name);
    html_file.remove();
    html_file.open(QIODevice::ReadWrite);
	std::ostringstream page;
	createPage(page, detail_data);
    html_file.write(page.str().c_str(),page.str().length());
    html_file.flush();
    html_file.close();
    QString url_name("file:///");
    url_name.append(html_file_name);

	
	QWebView *view = new QWebView();
	view->setPage(new ChromePage()); // hack required to get google maps to display for a desktop, not touchscreen
    view->load(QUrl(url_name));
	view->show();

	// Do a Qwt test
	QwtPlot* plot = new QwtPlot();

	QwtPlotCurve *curve_hr = new QwtPlotCurve("Heart Rate");
	QwtPlotCurve *curve_alt = new QwtPlotCurve("Altitude");

	std::vector<double> hr = std::vector<double>(detail_data._heart_rate.begin(), detail_data._heart_rate.end());
	curve_hr->setSamples(&detail_data._time[0], &hr[0], detail_data._num_points);
	curve_alt->setSamples(&detail_data._time[0], &detail_data._alt[0], detail_data._num_points);
	
	curve_hr->attach(plot);
	curve_alt->attach(plot);

	plot->replot();
	plot->show();

	// Plot picker
	QwtPlotPicker* plot_picker = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, plot->canvas());
	connect(plot_picker, SIGNAL(selected(const QPointF&)), this, SLOT(plotSelection(const QPointF&)));
	plot_picker->setStateMachine(new QwtPickerDragPointMachine());
	plot_picker->setRubberBandPen(QColor(Qt::green));
    plot_picker->setRubberBand(QwtPicker::CrossRubberBand);
    plot_picker->setTrackerPen(QColor(Qt::white));

	// Plot zoomer
	QwtPlotZoomer* plot_zoomer = new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, plot->canvas());
    plot_zoomer->setRubberBand(QwtPicker::RectRubberBand);
    plot_zoomer->setRubberBandPen(QColor(Qt::green));
    plot_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    plot_zoomer->setTrackerPen(QColor(Qt::white));
	//plot_zoomer->setTrackerMode(QwtPicker::AlwaysOff);
    plot_zoomer->setRubberBand(QwtPicker::NoRubberBand);
    plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier);
    plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);

	// Plot panner
	QwtPlotPanner* plot_panner = new QwtPlotPanner(plot->canvas());
	plot_panner->setMouseButton(Qt::MidButton);

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