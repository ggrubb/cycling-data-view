#include "googlemap.h"
#include "datalog.h"

#include <QWebView.h>
#include <QWebPage.h>
#include <QWebFrame.h>
#include <QDir.h>
#include <sstream>
#include <iostream>




/******************************************************/
GoogleMap::GoogleMap()
{
	_view = new QWebView();
}

/******************************************************/
GoogleMap::~GoogleMap()
{

}

/******************************************************/
/* Helper class to fool google maps to give desktop view*/
class ChromePage : public QWebPage
{
	virtual QString userAgentForUrl(const QUrl& url) const {
	 return "Chrome/1.0";
	}
};

/******************************************************/
void GoogleMap::somethingHappened(const QPointF& point)
{
	using namespace std;
	ostringstream stream;
	//stream << "var image = '" << QDir::currentPath().toStdString() << "/bike.png';" << endl
	//stream 	<< "var myLatLng = new google.maps.LatLng(57.6957,11.9234);" << endl
	//	<< "var beachMarker = new google.maps.Marker({ position: myLatLng, map: window.map });" << endl;

	int idx = point.x();
	double ltd = _data_log.ltd(idx);
	double lgd = _data_log.lgd(idx);
	std::cout << ltd << " " << lgd << std::endl;

	stream << "//deleteMarker();" << endl 
		<< "setMarker(" << ltd << "," << lgd << ");";
	_view->page()->mainFrame()->evaluateJavaScript(QString::fromStdString(stream.str()));
	//_view->page()->mainFrame()->evaluateJavaScript(QString("setMarker(57.6957,11.9234)"));
}

/******************************************************/
void GoogleMap::displayRide(DataLog& data_log)
{
	std::ostringstream page;
	createPage(page, data_log);
 
	_view->setPage(new ChromePage()); // hack required to get google maps to display for a desktop, not touchscreen
	_view->setHtml(QString::fromStdString(page.str()));
	_view->show();

	connect(_view, SIGNAL(loadFinished(bool)), this, SLOT(somethingHappened()));
	_data_log = data_log;
}

/******************************************************/
std::string GoogleMap::createPolyline(DataLog& data_log)
{
	std::ostringstream stream;
	int i=0;
	while (i < data_log.numPoints()-1)
	{
		stream << "new google.maps.LatLng(" << data_log.ltd(i) << "," << data_log.lgd(i) << ")," << std::endl;
		++i;
	}
	stream << "new google.maps.LatLng(" << data_log.ltd(i) << "," << data_log.lgd(i) << ")";

	return stream.str();
}

/******************************************************/
void GoogleMap::createPage(std::ostringstream& page, DataLog& data_log)
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
		
		// Gloabal variables
		<< "var map;" << endl
		<< "var marker;" << endl
		//<< "new google.maps.Marker()" << endl
		
		// Function initialise
		<< "function initialize() {" << endl
		<< "var latlng = new google.maps.LatLng(" << data_log.ltd(0) << "," << data_log.lgd(0) << ");" << endl
		<< "var myOptions = {" << endl
		<< "zoom: 10," << endl
		<< "center: latlng," << endl
		<< "mapTypeId: google.maps.MapTypeId.ROADMAP" << endl
		<< "};" << endl
		<< "map = new google.maps.Map(document.getElementById(\"map_canvas\"), myOptions);" << endl
		// Create a path from GPS coords
		<< "var ride_coords = [" << endl
		<< createPolyline(data_log) << endl
		<< "];" << endl
		// Plot the path
		<< "var ride_path = new google.maps.Polyline({" << endl
		<< "path: ride_coords," << endl
		<< "strokeColor: \"#FF0000\"," << endl
		<< "strokeOpacity: 1.0," << endl
		<< "strokeWeight: 2" << endl
		<< "});" << endl
		<< "ride_path.setMap(map);" << endl
		<< "}" << endl
		
		// Function setMarker
		<< "function setMarker(ltd,lgd) {" << endl
		//<< "var image = '" << QDir::currentPath().toStdString() << "/bike.png';" << endl
		<< "var lat_lng = new google.maps.LatLng(ltd ,lgd);" << endl
		<< "marker = new google.maps.Marker({ position: lat_lng, map: map});" << endl
		<< "}" << endl

		// Function deleteMarker
		<< "function deleteMarker() { " << endl
		<< "marker.setMap(null);" << endl
		<< "}" << endl

		<< "</script>" << endl
		<< "</head>" << endl
		<< "<body onload=\"initialize()\">" << endl
		<< "<div id=\"map_canvas\" style=\"width:100%; height:100%\"></div>" << endl
		<< "</body>" << endl
		<< "</html>" << endl;

	page << oss.str();
}


