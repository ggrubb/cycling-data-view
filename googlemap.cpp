#include "googlemap.h"
#include "datalog.h"

#include <QWebView.h>
#include <QWebPage.h>
#include <QWebFrame.h>
#include <QDir.h>
#include <sstream>
#include <iostream>

using namespace std;

#define UNDEFINED_IDX -1

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
	_selection_begin_idx = UNDEFINED_IDX;
	_selection_end_idx = UNDEFINED_IDX;
}

/******************************************************/
GoogleMap::~GoogleMap()
{

}

/******************************************************/
void GoogleMap::displayRide(DataLog* data_log)
{
	_data_log = data_log;

	ostringstream page;
	createPage(page);
	_view->setHtml(QString::fromStdString(page.str()));
	_view->show();
}

/******************************************************/
void GoogleMap::setMarkerPosition(int idx)
{
	if (idx > 0 && idx < _data_log->numPoints())
	{
		double ltd = _data_log->ltd(idx);
		double lgd = _data_log->lgd(idx);

		ostringstream stream;
		stream << "setMarker(" << ltd << "," << lgd << ");";
		_view->page()->mainFrame()->evaluateJavaScript(QString::fromStdString(stream.str()));
	}
}

/******************************************************/
void GoogleMap::setSelection(int idx_start, int idx_end)
{
	ostringstream stream;
	stream << "var coords = [" << endl
		<< defineCoords(idx_start, idx_end) << endl // create a path from GPS coords
		<< "];" << endl
		<< "setSelectionPath(coords);";
	_view->page()->mainFrame()->evaluateJavaScript(QString::fromStdString(stream.str()));
}

/******************************************************/
void GoogleMap::beginSelection(int idx_begin)
{
	_selection_begin_idx = idx_begin;
}

/******************************************************/
void GoogleMap::endSelection(int idx_end)
{
	_selection_end_idx = idx_end;
	if (_selection_end_idx > _selection_begin_idx)
	{
		setSelection(_selection_begin_idx,_selection_end_idx);
	}
	else
	{
		setSelection(_selection_end_idx,_selection_begin_idx);
	}
}

/******************************************************/
void GoogleMap::zoomSelection(int idx_start, int idx_end)
{
	if (idx_start == 0 && idx_end == _data_log->numPoints()-1) // check if zoomed to full view
	{
		_selection_begin_idx = UNDEFINED_IDX;
		_selection_end_idx = UNDEFINED_IDX;

		ostringstream stream;
		stream << "deleteSelectionPath();";
		_view->page()->mainFrame()->evaluateJavaScript(QString::fromStdString(stream.str()));
	}
	else // handle the case where zooming out, but not to full view
	{
		_selection_begin_idx = idx_start;
		_selection_end_idx = idx_end;
		setSelection(_selection_begin_idx,_selection_end_idx);
	}
}

/******************************************************/
void GoogleMap::moveSelection(int delta_idx)
{
	int i = std::max(_selection_begin_idx - delta_idx, 0);
	int j = std::min(_selection_end_idx - delta_idx, _data_log->numPoints());
	setSelection(i, j);
}

/******************************************************/
void GoogleMap::moveAndHoldSelection(int delta_idx)
{
	_selection_begin_idx = std::max(_selection_begin_idx - delta_idx, 0);
	_selection_end_idx = std::min(_selection_end_idx - delta_idx, _data_log->numPoints());
	setSelection(_selection_begin_idx,_selection_end_idx);
}

/******************************************************/
std::string GoogleMap::defineCoords(int idx_start, int idx_end)
{
	ostringstream stream;
	stream.precision(6); // set precision so we plot lat/long correctly
	stream.setf(ios::fixed,ios::floatfield);

	for (int i = idx_start; i < idx_end; ++i)
	{
		stream << "new google.maps.LatLng(" << _data_log->ltd(i) << "," << _data_log->lgd(i) << ")," << endl;
	}

	return stream.str();
}

/******************************************************/
void GoogleMap::createPage(std::ostringstream& page)
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
		<< "var ride_coords = [" << defineCoords(0, _data_log->numPoints()) << "];" << endl // create a path from GPS coords
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


