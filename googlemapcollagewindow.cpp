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

	QLabel* label = new QLabel("Path coloured to ride frequency");
	label->setMaximumWidth(160);
	
	_colour_bar = new ColourBar();
	QWidget* widget1 = new QWidget;
	QHBoxLayout* hlayout = new QHBoxLayout(widget1);
	hlayout->addSpacing(20);
	hlayout->addWidget(label);
	hlayout->addSpacing(20);
	hlayout->addWidget(_colour_bar);

	QVBoxLayout* vlayout = new QVBoxLayout(this);
	vlayout->addWidget(_view);
	vlayout->addWidget(widget1);
	vlayout->setSpacing(0);

	setWindowTitle("RideCollage");
	setWindowIcon(QIcon("./resources/rideviewer_head128x128.ico")); 
}

/******************************************************/
GoogleMapCollageWindow::~GoogleMapCollageWindow()
{}

/******************************************************/
void GoogleMapCollageWindow::displayRides(const std::vector<QString>& filenames)
{
	_accumulated_points.clear();
	_accumulated_point_extra_info.clear();
	_max_count=0;

	// Create a small progress bar
	QProgressDialog load_progress("Loading log:", "Cancel load", 0, filenames.size()-1, this);
	load_progress.setWindowModality(Qt::WindowModal);
	load_progress.setMinimumDuration(0); //msec
	load_progress.setWindowTitle("RideCollage");

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
				for (int pt=0; pt < data_log->numPoints(); pt+=4)
				{
					LatLng lat_lng(data_log->ltd(pt), data_log->lgd(pt));
					bool found = false;
					
					int a_pt=_accumulated_points.size()-1; // going backwards is slightly faster because the latest points are at the end (so break quicker)
					while (a_pt >= 0)
					{
						// Check if we have visited this point previously
						if (lat_lng == _accumulated_points[a_pt].first)
						{
							// Only increment frequency if we have not been here in this ride, or it is this ride but more than 60 secs ago
							if (i != _accumulated_point_extra_info[a_pt].first || 
								0)//abs(data_log->time(pt) - _accumulated_point_extra_info[a_pt].second) > 60)
							{
								_accumulated_points[a_pt].second++;
								_accumulated_point_extra_info[a_pt].first = i;
								_accumulated_point_extra_info[a_pt].second = data_log->time(pt);
								if (_accumulated_points[a_pt].second > _max_count)
									_max_count = _accumulated_points[a_pt].second;
							}
							found = true;
							break;
						}
						--a_pt;
					}

					if (!found)
					{
						_accumulated_points.push_back(std::make_pair(lat_lng,1));
						_accumulated_point_extra_info.push_back(std::make_pair(i,data_log->time(pt)));
					}
				}
			}
		}
		delete data_log;
	}
	
	if (_accumulated_points.size() > 0) // we have a valid path to show
	{
		// Create the google map web page
		ostringstream page;
		createPage(page);
		_view->setHtml(QString::fromStdString(page.str()));
		show();
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
std::string GoogleMapCollageWindow::defineColours()
{
	// Colour the path and set the colour bar correspondingly
	ostringstream stream;
	stream.precision(2); // only need low precision
	stream.setf(ios::fixed,ios::floatfield);

	for (unsigned int i=0; i < _accumulated_points.size(); ++i) // one less key since there is one more polyline segment in the path
	{
		double key = std::min((double)_accumulated_points[i].second/_max_count, 1.0);
		stream << key << ", ";
	}

	_colour_bar->setColourRange(Qt::green, Qt::yellow, Qt::red, 1, _max_count);

	return stream.str();
}

/******************************************************/
std::string GoogleMapCollageWindow::defineCoords()
{
	ostringstream stream;
	stream.precision(6); // set precision so we plot lat/long correctly
	stream.setf(ios::fixed,ios::floatfield);

	for (unsigned int i=0; i < _accumulated_points.size(); ++i)
	{
		stream << "new google.maps.LatLng(" << _accumulated_points[i].first.lat << "," << _accumulated_points[i].first.lng<< ")," << endl;
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
		<< "var colours = [\"00FF00\", \"19FF00\", \"32FF00\", \"4CFF00\", \"66FF00\", \"7FFF00\", \"99FF00\", \"B2FF00\", \"CCFF00\", \"E5FF00\", \"FFFF00\", \"FFE500\", \"FFCC00\", \"FFB200\", \"FF9900\", \"FF7F00\", \"FF6600\", \"FF4C00\", \"FF3300\", \"FF1900\", \"FF0000\"];" << endl // colour table, from green to red in 20 steps
		<< "var bounds = new google.maps.LatLngBounds();" << endl
		<< "var grey_style = [ { featureType: \"all\",  elementType: \"all\", stylers: [ { saturation: -100 }]}];" << endl
		<< "var colour_style = [ { featureType: \"all\",  elementType: \"all\", stylers: [ { saturation: 0 }]}];" << endl

		// Function initialise
		<< "function initialize() {" << endl
		<< "var map = new google.maps.Map(document.getElementById(\"map_canvas\"), {mapTypeControlOptions: {mapTypeIds: ['colour', 'grey', google.maps.MapTypeId.TERRAIN]}} );" << endl
		<< "var map_type_grey = new google.maps.StyledMapType(grey_style, { name:\"Grey\" });" << endl
		<< "var map_type_colour = new google.maps.StyledMapType(colour_style, { name:\"Colour\" });" << endl
		<< "map.mapTypes.set('grey', map_type_grey);" << endl
		<< "map.mapTypes.set('colour', map_type_colour);" << endl
		<< "map.setMapTypeId('grey');" << endl

		<< "var ride_coords = [" << defineCoords() << "];" << endl // create a path from GPS coords
		<< "var colour_key = [" << defineColours() << "];" << endl // create a key of colours
		<< "var max_index = 0;" << endl
		<< "var max_key = 0;" << endl
		
		<< "for (var i = 0, len = ride_coords.length; i < len; i++) {" << endl
		<< "var colour = colourFromFraction(colour_key[i]);" << endl
		<< "new google.maps.Circle({fillColor: colour, strokeColor: colour, strokeWeight: 2, fillOpacity: 1.0, center: ride_coords[i], radius: 30.0, clickable: false, map: map})" << endl
		<< "if (colour_key[i] > max_key)" << endl
		<< "{max_key = colour_key[i]; max_index = i;}" << endl
		<< "}" << endl
		
		<< "var circle = new google.maps.Circle({center: ride_coords[max_index], radius: 70000});" << endl
		<< "map.fitBounds(circle.getBounds());" << endl
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
