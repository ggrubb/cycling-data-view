#include "googlemap.h"
#include "datalog.h"

#include <QWebView.h>
#include <QWebPage.h>
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
void GoogleMap::displayRide(DataLog& data_log)
{
	QString html_file_name(QDir::tempPath());
    html_file_name.append("/maps.html");
    QFile html_file(html_file_name);
    html_file.remove();
    html_file.open(QIODevice::ReadWrite);
	std::ostringstream page;
	createPage(page, data_log);
    html_file.write(page.str().c_str(),page.str().length());
    html_file.flush();
    html_file.close();
    QString url_name("file:///");
    url_name.append(html_file_name);

	_view->setPage(new ChromePage()); // hack required to get google maps to display for a desktop, not touchscreen
    _view->load(QUrl(url_name));
	_view->show();
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
		<< "function initialize() {" << endl
		<< "var latlng = new google.maps.LatLng(" << data_log.ltd(0) << "," << data_log.lgd(0) << ");" << endl
		<< "var myOptions = {" << endl
		<< "zoom: 10," << endl
		<< "center: latlng," << endl
		<< "mapTypeId: google.maps.MapTypeId.ROADMAP" << endl
		<< "};" << endl
		<< "var map = new google.maps.Map(document.getElementById(\"map_canvas\"), myOptions);" << endl

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

		//Create an icon marker
		<< "var image = '" << QDir::currentPath().toStdString() << "/bike.png';" << endl
		<< "var myLatLng = new google.maps.LatLng(" << data_log.ltd(0) << "," << data_log.lgd(0) << ");" << endl
		<< "var beachMarker = new google.maps.Marker({ position: myLatLng, map: map, icon: image });" << endl

		<< "}" << endl
		<< "</script>" << endl
		<< "</head>" << endl
		<< "<body onload=\"initialize()\">" << endl
		<< "<div id=\"map_canvas\" style=\"width:100%; height:100%\"></div>" << endl
		<< "</body>" << endl
		<< "</html>" << endl;

	page << oss.str();
}


