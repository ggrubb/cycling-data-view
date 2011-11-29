#include "googlemapcollagewindow.h"
#include "datalog.h"
#include "dataprocessing.h"
#include "tcxparser.h"
#include "fitparser.h"

#include <QWebView.h>
#include <QWebPage.h>
#include <QWebFrame.h>
#include <QDir.h>
#include <QComboBox.h>
#include <QLabel.h>
#include <QBoxLayout.h>
#include <QProgressDialog.h>

#include <qwt_scale_widget.h>
#include <qwt_color_map.h>
#include <qwt_interval.h>

#include <sstream>
#include <iostream>

using namespace std;

/******************************************************/
// Helper class to fool google maps to give desktop view
class ChromePage : public QWebPage
{
	virtual QString userAgentForUrl(const QUrl& url) const {
	 return "Chrome/1.0";
	}
};

/******************************************************/
// A horizontal bar which shows the colour scale of path colour
class ColourBar : public QWidget
{
public:
	ColourBar(): QWidget(),
	_start_colour(Qt::red),
	_end_colour(Qt::red),
	_middle_colour(Qt::red),
	_min_value(0.0),
	_max_value(0.0)
	{}

	void setColourRange(const QColor start_colour, const QColor middle_colour ,const QColor end_colour, double min, double max)
	{
		_start_colour = start_colour;
		_end_colour = end_colour;
		_middle_colour = middle_colour;
		_min_value = min;
		_max_value = max;
	}

protected:
	void paintEvent(QPaintEvent* e)
	{
		QwtLinearColorMap color_map(_start_colour, _end_colour);
		color_map.addColorStop(0.5,_middle_colour);
		QPainter* painter = new QPainter(this);

		QwtScaleWidget *colour_bar = new QwtScaleWidget(QwtScaleDraw::BottomScale);
		colour_bar->setColorMap(QwtInterval(0.0, 1.0), &color_map);
		colour_bar->setColorBarEnabled(true);
		colour_bar->drawColorBar(painter, QRect(45,0, 340, 15));
		painter->setPen(Qt::black);
		painter->setFont(QFont("Helvetica", 8));
		painter->drawText(0,13,"Min " + QString::number(_min_value,'f',0));
		painter->drawText(390,13,"Max " + QString::number(_max_value,'f',0));
		painter->end();
	}

private:
	QColor _start_colour;
	QColor _end_colour;
	QColor _middle_colour;
	double _min_value;
	double _max_value;
};

/******************************************************/
GoogleMapCollageWindow::GoogleMapCollageWindow()
{
	_view = new QWebView();
	_view->setPage(new ChromePage()); // hack required to get google maps to display for a desktop, not touchscreen

	_tcx_parser = new TcxParser();
	_fit_parser = new FitParser();

	// Selection for path colour scheme
	_path_colour_scheme = new QComboBox();
	_path_colour_scheme->setMaximumWidth(120);
	_path_colour_scheme->insertItem(0,"None");
	_path_colour_scheme->insertItem(1,"Heart Rate (bpm)");
	_path_colour_scheme->insertItem(2,"Speed (km/h)");
	_path_colour_scheme->insertItem(3,"Gradient (%)");
	_path_colour_scheme->insertItem(4,"Cadence (rpm)");
	_path_colour_scheme->insertItem(5,"Power (W)");
	connect(_path_colour_scheme,SIGNAL(currentIndexChanged(int)), this, SLOT(definePathColour()));

	QLabel* label = new QLabel("Path coloured to: ");
	label->setMaximumWidth(90);
	
	_colour_bar = new ColourBar();
	QWidget* widget1 = new QWidget;
	QHBoxLayout* hlayout = new QHBoxLayout(widget1);
	hlayout->addSpacing(20);
	hlayout->addWidget(label);
	hlayout->addWidget(_path_colour_scheme);
	hlayout->addSpacing(20);
	hlayout->addWidget(_colour_bar);

	QVBoxLayout* vlayout = new QVBoxLayout(this);
	vlayout->addWidget(_view);
	vlayout->addWidget(widget1);
	vlayout->setSpacing(0);

	// Disable user interface until a ride is loaded
	setEnabled(false);
}

/******************************************************/
GoogleMapCollageWindow::~GoogleMapCollageWindow()
{}

/******************************************************/
void GoogleMapCollageWindow::setEnabled(bool enabled)
{
	_path_colour_scheme->setEnabled(enabled);
}

/******************************************************/
void GoogleMapCollageWindow::displayRides(const std::vector<QString>& filenames)
{
	_accumulated_points.clear();

	// Create a small progress bar
	QProgressDialog load_progress("Loading log:", "Cancel load", 0, filenames.size()-1, this);
	load_progress.setWindowModality(Qt::WindowModal);
	load_progress.setMinimumDuration(0); //msec
	load_progress.setWindowTitle("RideViewer");

	// Load new log files in the directory
	for (unsigned int i=0; i < filenames.size(); ++i)
	{
		load_progress.setValue(i);
		load_progress.setLabelText("Loading log: " + filenames[i]);
		if (load_progress.wasCanceled())
			break;

		DataLog* data_log = new DataLog;	
		if (parse(filenames[i], data_log))
		{	
			if (data_log->lgdValid() && data_log->ltdValid())
			{
				for (int pt=0; pt < data_log->numPoints(); ++pt)
				{
					LatLng lat_lng;
					lat_lng.lat = data_log->ltd(pt);
					lat_lng.lng = data_log->lgd(pt);
					_accumulated_points.insert(lat_lng, 1);
				}
			}
		}
	}

	if (_accumulated_points.size() > 0) // we have a valid path to show
	{
		// Create the google map web page
		ostringstream page;
		createPage(page);
		_view->setHtml(QString::fromStdString(page.str()));

		show();

		// Enabled user interface
		setEnabled(true);
		_path_colour_scheme->setCurrentIndex(0);
	}
}

/******************************************************/
bool GoogleMapCollageWindow::parse(const QString filename, DataLog* data_log)
{
	if (filename.contains(".fit"))
	{
		return _fit_parser->parse(filename, *data_log);
	}
	else if (filename.contains(".tcx"))
	{
		return _tcx_parser->parse(filename, *data_log);
	}
	else
	{
		return false; // unknown log type
	}
}

/******************************************************/
void GoogleMapCollageWindow::definePathColour()
{
/*	// Colour the path and set the colour bar correspondingly
	ostringstream stream;
	stream.precision(2); // only need low precision
	stream.setf(ios::fixed,ios::floatfield);
	
	// First determine min and max of signal to colour code
	double max = 0.0;
	double min = 0.0;
	switch (_path_colour_scheme->currentIndex())
	{
	case 0: // none
		break;
	case 1: // heart rate
		max = _data_log->maxHeartRate();
		min = DataProcessing::computeMin(_data_log->heartRateFltd().begin(), _data_log->heartRateFltd().end());
		break;
	case 2: // speed
		max = _data_log->maxSpeed();
		min = DataProcessing::computeMin(_data_log->speedFltd().begin(), _data_log->speedFltd().end());
		break;
	case 3: // gradient
		max = _data_log->maxGradient();
		min = DataProcessing::computeMin(_data_log->gradientFltd().begin(), _data_log->gradientFltd().end());
		break;
	case 4: // cadence
		max = _data_log->maxCadence();//DataProcessing::computeNthPercentile(_data_log->cadence().begin(), _data_log->cadence().end(), 0.95);  // large spikes in cadence can exist, so be harsher when choosing max
		min = DataProcessing::computeMin(_data_log->cadenceFltd().begin(), _data_log->cadenceFltd().end());
		break;
	case 5: // power
		max = _data_log->maxPower();
		min = DataProcessing::computeMin(_data_log->powerFltd().begin(), _data_log->powerFltd().end());
		break;
	}

	double factor;
	double min_key = 1.0;
	stream << "var key = [" << endl;
	for (int i=0; i < _data_log->numPoints()-1; ++i) // one less key since there is one more polyline segment in the path
	{
		double key = 1.0;
		switch (_path_colour_scheme->currentIndex())
		{
		case 0: // none
			key = 1.0;
			break;
		case 1: // heart rate
			factor = 0.9;
			if (max > 0.0)
				key = ((_data_log->heartRateFltd(i)/_data_log->maxHeartRate())*(1.0+factor) ) - factor;
			break;
		case 2: // speed
			factor = 0.1;
			if (max > 0.0)
				key = ((_data_log->speedFltd(i)/_data_log->maxSpeed())*(1.0+factor) ) - factor;
			break;
		case 3: // gradient
			if (max > 0.0)
				key = (_data_log->gradientFltd(i)/(_data_log->maxGradient()*0.5 + 0.00001) ) + 0.5;
			break;
		case 4: // cadence
			{
			factor = 0.0;
			if (max > 0.0)
				key = std::min( _data_log->cadenceFltd(i)/max, 1.0);
			}
			break;
		case 5: // power
			factor = 0.7;
			if (max > 0.0)
				key = ((_data_log->powerFltd(i)/_data_log->maxPower())*(1.0+factor) ) - factor;
			break;
		}
		key = std::max(key,0.0);
		key = std::min(key,1.0);
		stream << key << ", ";

		if (key < min_key) // keep track of the min key value
			min_key = key;
	}

	stream << "];" << endl; 
	stream << "strokeRidePath(key);";
	_view->page()->mainFrame()->evaluateJavaScript(QString::fromStdString(stream.str()));

	// Draw the colour bar appropriately, depending on the max key value
	if (min_key < 1.0)
	{
		_colour_bar->setColourRange(Qt::green, Qt::yellow, Qt::red, min, max);
	}
	else
	{
		_colour_bar->setColourRange(Qt::red, Qt::red, Qt::red, 0.0, 0.0);
	}
	_colour_bar->update();
	*/
}

/******************************************************/
std::string GoogleMapCollageWindow::defineCoords()
{
	ostringstream stream;
	stream.precision(6); // set precision so we plot lat/long correctly
	stream.setf(ios::fixed,ios::floatfield);

	QMap<LatLng, int>::const_iterator it = _accumulated_points.begin();
	while (it != _accumulated_points.end())
	{
		stream << "new google.maps.LatLng(" << it.key().lat << "," << it.key().lng<< ")," << endl;
		++it;
	}

	return stream.str();
}

/******************************************************/
void GoogleMapCollageWindow::createPage(std::ostringstream& page)
{
	ostringstream oss;
    oss.precision(6); // set precision so we plot lat/long correctly
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
		
		// Global variables
		<< "var map;" << endl
		<< "var selected_path;" << endl
		<< "var colours = [\"00FF00\", \"19FF00\", \"32FF00\", \"4CFF00\", \"66FF00\", \"7FFF00\", \"99FF00\", \"B2FF00\", \"CCFF00\", \"E5FF00\", \"FFFF00\", \"FFE500\", \"FFCC00\", \"FFB200\", \"FF9900\", \"FF7F00\", \"FF6600\", \"FF4C00\", \"FF3300\", \"FF1900\", \"FF0000\"];" << endl // colour table, from green to red in 20 steps
		<< "var ride_path = new Array();" << endl
		<< "var ride_bounds = new google.maps.LatLngBounds();" << endl
		<< "var ride_coords;" << endl

		// Global variables - define marker images
		<< "var marker_image = new google.maps.MarkerImage(" << endl
		<< "'" << QDir::currentPath().toStdString() << "/resources/marker_image.png'," << endl
		<< "new google.maps.Size(50,50)," << endl
		<< "new google.maps.Point(0,0)," << endl
		<< "new google.maps.Point(25,50) );" << endl

		<< "var marker_shadow = new google.maps.MarkerImage(" << endl
		<< "'" << QDir::currentPath().toStdString() << "/resources/marker_shadow.png'," << endl
		<< "new google.maps.Size(78,50)," << endl
		<< "new google.maps.Point(0,0)," << endl
		<< "new google.maps.Point(25,50) );" << endl

		<< "var start_image = new google.maps.MarkerImage(" << endl
		<< "'" << QDir::currentPath().toStdString() << "/resources/start_image.png'," << endl
		<< "new google.maps.Size(25,25)," << endl
		<< "new google.maps.Point(0,0)," << endl
		<< "new google.maps.Point(25,25) );" << endl

		<< "var start_shadow = new google.maps.MarkerImage(" << endl
		<< "'" << QDir::currentPath().toStdString() << "/resources/start_shadow.png'," << endl
		<< "new google.maps.Size(41,25)," << endl
		<< "new google.maps.Point(0,0)," << endl
		<< "new google.maps.Point(25,25) );" << endl

		<< "var finish_image = new google.maps.MarkerImage(" << endl
		<< "'" << QDir::currentPath().toStdString() << "/resources/finish_image.png'," << endl
		<< "new google.maps.Size(25,25)," << endl
		<< "new google.maps.Point(0,0)," << endl
		<< "new google.maps.Point(0,25) );" << endl

		<< "var finish_shadow = new google.maps.MarkerImage(" << endl
		<< "'" << QDir::currentPath().toStdString() << "/resources/finish_shadow.png'," << endl
		<< "new google.maps.Size(41,25)," << endl
		<< "new google.maps.Point(0,0)," << endl
		<< "new google.maps.Point(0,25) );" << endl

		<< "var marker = new google.maps.Marker({icon: marker_image, shadow: marker_shadow});" << endl
		<< "var start_marker = new google.maps.Marker({icon: start_image, shadow: start_shadow});" << endl
		<< "var finish_marker = new google.maps.Marker({icon: finish_image, shadow: finish_shadow});" << endl

		// Function initialise
		<< "function initialize() {" << endl
		<< "selected_path = new google.maps.Polyline({strokeColor: \"#000000\",strokeOpacity: 1.0, strokeWeight: 8, zIndex: 1});" << endl
		<< "map = new google.maps.Map(document.getElementById(\"map_canvas\"), {mapTypeId: google.maps.MapTypeId.ROADMAP});" << endl

		<< "ride_coords = [" << defineCoords() << "];" << endl // create a path from GPS coords
		
		<< "for (i=0;i<200;i++) {" << endl
		<< "new google.maps.Circle({center: ride_coords[i], radius: 10.0, map: map})" << endl
		<< "}" << endl
		
		<< "for (var i = 0, len = ride_coords.length; i < len; i++) {" << endl
		<< "ride_bounds.extend(ride_coords[i]);" << endl
		<< "}" << endl
		<< "map.fitBounds(ride_bounds);" << endl
		<< "}" << endl

		// Function setMarker
		<< "function setMarker(ltd,lgd) {" << endl
		<< "var lat_lng = new google.maps.LatLng(ltd ,lgd);" << endl
		<< "marker.setPosition(lat_lng);" << endl
		<< "marker.setMap(map);" << endl
		<< "}" << endl

		// Function setSelectionPath
		<< "function setSelectionPath(coords, zoom_map) { " << endl
		<< "selected_path.setPath(coords);" << endl
		<< "selected_path.setMap(map);" << endl
		<< "if (zoom_map) {" << endl
		<< "var path_bounds = new google.maps.LatLngBounds();" << endl
		<< "for (var i = 0, len = coords.length; i < len; i++) {" << endl
		<< "path_bounds.extend(coords[i]);" << endl
		<< "}" << endl
		<< "map.fitBounds(path_bounds);" << endl
		<< "}" << endl
		<< "start_marker.setPosition(coords[0]);" << endl
		<< "finish_marker.setPosition(coords[coords.length-1]);" << endl
		<< "}" << endl

		// Function deleteSelectionPath
		<< "function deleteSelectionPath() {" << endl
		<< "selected_path.setMap(null);" << endl
		<< "map.fitBounds(ride_bounds);" << endl
		<< "start_marker.setPosition(ride_coords[0]);" << endl
		<< "finish_marker.setPosition(ride_coords[ride_coords.length-1]);" << endl
		<< "}" << endl

		// Function to stroke ride path (ie colour it) according to key vector (0 <= key[i] <= 1)
		<< "function strokeRidePath(key) {" << endl
		<< "if (key.length == ride_path.length) {" << endl
		<< "for (i=0; i<ride_path.length-1; i++) {" << endl
		<< "ride_path[i].setOptions({strokeColor: colourFromFraction(key[i])});" << endl
		<< "}" << endl
		<< "}" << endl
		<< "}" << endl

		// Function to convert num to hex (0 <= frac <= 1.0)
		<< "function colourFromFraction(frac) {" << endl
		<< "index = frac*colours.length" << endl
		<< "if (Math.round(index) == colours.length)" << endl
		<< "return colours[colours.length-1];" << endl
		<< "else" << endl
		<< "return colours[Math.round(index)];" << endl
		<< "}" << endl

		// Function to convert num to hex
		<< "function decimalToHex(d, padding) {" << endl
		<< "var hex = Number(d).toString(16);" << endl
		<< "padding = typeof (padding) === \"undefined\" || padding === null ? padding = 2 : padding;" << endl
		<< "while (hex.length < padding) {" << endl
		<< "hex = \"0\" + hex;" << endl
		<< "}" << endl
		<< "return hex;" << endl
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
void GoogleMapCollageWindow::createEmptyPage(std::ostringstream& page)
{
	ostringstream oss;
    oss.precision(6); // set precision so we plot lat/long correctly
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
		
		<< "</head>" << endl
		<< "<body bgcolor=\"#E2E2E2\">" << endl
		<< "<br><br><br><br><br><small><center>No GPS data to display in this log</center></small>" << endl
		<< "</body>" << endl
		<< "</html>" << endl;

	page << oss.str();
}

