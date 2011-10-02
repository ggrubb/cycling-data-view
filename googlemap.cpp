#include "googlemap.h"
#include "datalog.h"

#include <QWebView.h>
#include <QWebPage.h>
#include <QWebFrame.h>
#include <QDir.h>
#include <sstream>
#include <iostream>

using namespace std;

#define UNDEFINED_TIME -1

/******************************************************/
/* Helper class to fool google maps to give desktop view*/
class ChromePage : public QWebPage
{
	virtual QString userAgentForUrl(const QUrl& url) const {
	 return "Chrome/1.0";
	}
};

/******************************************************/
GoogleMap::GoogleMap()
{
	_view = new QWebView();
	_view->setPage(new ChromePage()); // hack required to get google maps to display for a desktop, not touchscreen
	_selection_begin_time = UNDEFINED_TIME;
	_selection_end_time = UNDEFINED_TIME;
}

/******************************************************/
GoogleMap::~GoogleMap()
{

}

/******************************************************/
void GoogleMap::setMarkerPosition(const QPointF& point)
{
	double time = point.x();
	QMap<double, std::pair<double, double> >::iterator ltd_lgd_iterator = _time_v_ltd_lgd.lowerBound(time);
	if (ltd_lgd_iterator != _time_v_ltd_lgd.end())
	{
		double ltd = ltd_lgd_iterator.value().first;
		double lgd = ltd_lgd_iterator.value().second;

		ostringstream stream;
		stream << "setMarker(" << ltd << "," << lgd << ");";
		_view->page()->mainFrame()->evaluateJavaScript(QString::fromStdString(stream.str()));
	}
}

/******************************************************/
void GoogleMap::beginSelection(const QPointF& point)
{
	if (point.x() > 0)
		_selection_begin_time = point.x();
}

/******************************************************/
void GoogleMap::endSelection(const QPointF& point)
{
	_selection_end_time = point.x();

	QMap<double, std::pair<double, double> >::iterator begin_it = _time_v_ltd_lgd.lowerBound(_selection_begin_time);
	QMap<double, std::pair<double, double> >::iterator end_it = _time_v_ltd_lgd.lowerBound(_selection_end_time);
	
	ostringstream stream;
	stream << "var coords = [" << endl
		<< defineCoords(begin_it, end_it) << endl // create a path from GPS coords
		<< "];" << endl
		<< "setSelectionPath(coords);";
	_view->page()->mainFrame()->evaluateJavaScript(QString::fromStdString(stream.str()));
}

/******************************************************/
void GoogleMap::zoomSelection(const QRectF& rect)
{
	if (rect.x() == 0 && rect.y() == 0) // check if zoomed to full view
	{
		_selection_begin_time = UNDEFINED_TIME;
		_selection_end_time = UNDEFINED_TIME;

		ostringstream stream;
		stream << "deleteSelectionPath();";
		_view->page()->mainFrame()->evaluateJavaScript(QString::fromStdString(stream.str()));
	}
	else // handle the case where zooming out, but not to full view
	{
		beginSelection(rect.topLeft());
		endSelection(rect.bottomRight());
	}
}

/******************************************************/
void GoogleMap::moveSelection(int x, int y)
{
	
	beginSelection(QPointF(_selection_begin_time + x,0));
	endSelection(QPointF(_selection_end_time + x, 0));
	cout << "x: " << x << endl;
	cout << "delta: " << _selection_end_time - _selection_begin_time << endl;
}

/******************************************************/
void GoogleMap::setTimeVLtdLgd(DataLog& data_log)
{
	// Create a map from log time to log latitude and longitude
	for (int i=0; i < data_log.numPoints(); ++i)
	{
		_time_v_ltd_lgd.insert(data_log.time(i), std::pair<double, double>(data_log.ltd(i), data_log.lgd(i)));
	}
}

/******************************************************/
void GoogleMap::displayRide(DataLog& data_log)
{
	std::ostringstream page;
	setTimeVLtdLgd(data_log);
	createPage(page);
	_view->setHtml(QString::fromStdString(page.str()));
	_view->show();
}

/******************************************************/
std::string GoogleMap::defineCoords(
	QMap<double, std::pair<double, double> >::iterator first,
	QMap<double, std::pair<double, double> >::iterator last)
{
	std::ostringstream stream;
	for (QMap<double, std::pair<double, double> >::iterator it = first; it != last; it++)
	{
		stream << "new google.maps.LatLng(" << it.value().first << "," << it.value().second << ")," << std::endl;
	}

	return stream.str();
}

/******************************************************/
void GoogleMap::createPage(std::ostringstream& page)
{
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
		
		// Global variables
		<< "var map;" << endl
		<< "var marker;" << endl
		<< "marker = new google.maps.Marker();" << endl
		<< "var selected_path;" << endl
		<< "selected_path = new google.maps.Polyline({strokeColor: \"#0000FF\",strokeOpacity: 0.7,strokeWeight: 3});" << endl

		// Function initialise
		<< "function initialize() {" << endl
		<< "var myOptions = {" << endl
		<< "mapTypeId: google.maps.MapTypeId.ROADMAP" << endl
		<< "};" << endl
		<< "map = new google.maps.Map(document.getElementById(\"map_canvas\"), myOptions);" << endl
		<< "var ride_coords = [" << defineCoords(_time_v_ltd_lgd.begin(), _time_v_ltd_lgd.end()) << "];" << endl // create a path from GPS coords
		<< "var ride_path = new google.maps.Polyline({" << endl
		<< "path: ride_coords," << endl
		<< "strokeColor: \"#FF0000\"," << endl
		<< "strokeOpacity: 1.0," << endl
		<< "strokeWeight: 2," << endl
		<< "map: map" << endl
		<< "});" << endl
		<< "var bounds = new google.maps.LatLngBounds();" << endl
		<< "for (var i = 0, len = ride_coords.length; i < len; i++) {" << endl
		<< "bounds.extend (ride_coords[i]);" << endl
		<< "}" << endl
		<< "map.fitBounds(bounds);" << endl
		<< "}" << endl

		// Function setMarker
		<< "function setMarker(ltd,lgd) {" << endl
		<< "var lat_lng = new google.maps.LatLng(ltd ,lgd);" << endl
		<< "marker.setPosition(lat_lng);" << endl
		<< "marker.setMap(map);" << endl
		<< "}" << endl

		// Function deleteMarker
		<< "function deleteMarker() { " << endl
		<< "marker.setMap(null);" << endl
		<< "}" << endl

		// Function setSelectionPath
		<< "function setSelectionPath(coords) { " << endl
		<< "selected_path.setPath(coords);" << endl
		<< "selected_path.setMap(map);" << endl
		<< "}" << endl

		// Function deleteSelectionPath
		<< "function deleteSelectionPath() { " << endl
		<< "selected_path.setMap(null);" << endl
		<< "}" << endl

		<< "</script>" << endl
		<< "</head>" << endl
		<< "<body onload=\"initialize()\">" << endl
		<< "<div id=\"map_canvas\" style=\"width:100%; height:100%\"></div>" << endl
		<< "</body>" << endl
		<< "</html>" << endl;

	page << oss.str();
}


