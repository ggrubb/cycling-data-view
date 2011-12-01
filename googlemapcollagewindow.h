#ifndef GOOGLEMAPCOLLAGE_H
#define GOOGLEMAPCOLLAGE_H

#include <qtxml/qdomdocument>
#include <QWidget.h>
#include <QPoint.h>
#include <QWebView.h>
#include <QMap.h>

#include <math.h>
#include <iostream>

class DataLog;
class TcxParser;
class FitParser;
class QComboBox;
class ColourBar;

#define M_PI 3.14159265358979323846

class LatLng
{
public:
	LatLng(double lati, double lngi):
	  lat(lati),
	  lng(lngi)
	{}
	LatLng(){}

	double lat;
	double lng;

	bool operator==(const LatLng &other) const
	{
		//lat2 = other
		//lat1 = this
		const double R = 6371000.0; // m, radius of the earth
		const double d_lat = (other.lat-lat)*M_PI/180.0;
		const double d_lon = (other.lng-lng)*M_PI/180.0;
		const double lat1 = lat*M_PI/180.0;
		const double lat2 = other.lat*M_PI/180.0;

		const double a = sin(d_lat/2.0) * sin(d_lat/2.0) +
				sin(d_lon/2.0) * sin(d_lon/2.0) * cos(lat1) * cos(lat2); 
		const double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a)); 
		const double dist = R * c;
		
		//std::cout << "this: " << lat << ", " << lng << " other: " << other.lat << ", " << other.lng << " dist: " << dist << std::endl;
		const double dist_threshold = 300; //m
		if (abs(dist) < dist_threshold)
			return true;
		else
			return false;
	}
};

class GoogleMapCollageWindow : public QWidget
{
	Q_OBJECT

 public:
	GoogleMapCollageWindow();
	~GoogleMapCollageWindow();

	// Display all the rides in filenames on a google map
	void displayRides(const std::vector<QString>& filenames);
 
 protected:
	virtual void closeEvent(QCloseEvent* event);

 private:
	// Create the webpage to display google maps
	void createPage(std::ostringstream& page);

	// Stroke the ride path according to frequency
	void definePathColour();

	std::string defineCoords();
	bool parse(const QString filename, DataLog* data_log);

	TcxParser* _tcx_parser;
	FitParser* _fit_parser;

	// The window to display google maps
	QWebView *_view;

	std::vector<std::pair<LatLng, int> > _accumulated_points; // first=lat,long, second=count
	std::vector<std::pair<int, double> > _accumulated_point_extra_info; // first=ride id, second=time
	ColourBar* _colour_bar;
	int _max_count;
};

#endif // GOOGLEMAPCOLLAGE_H