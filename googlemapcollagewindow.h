#ifndef GOOGLEMAPCOLLAGE_H
#define GOOGLEMAPCOLLAGE_H

#include <qtxml/qdomdocument>
#include <QWidget.h>
#include <QPoint.h>
#include <QWebView.h>
#include <QMap.h>

#include <math.h>

class DataLog;
class TcxParser;
class FitParser;
class QComboBox;
class ColourBar;

class LatLng
{
public:
	double lat;
	double lng;

	// Turn this into an aprox equals operator
	bool operator==(const LatLng &other) const
	{
		const double d1 = other.lat - lat;
		const double d2 = other.lng - lng;
		const double dist = sqrt(d1*d1 + d2*d2);

		if (dist < 0.0001)
			return true;
		else
			return false;
    }

	bool operator<(const LatLng &other) const
	{
		const double d1 = sqrt(lat*lat + lng*lng);
		const double d2 = sqrt(other.lat*other.lat + other.lng*other.lng);
		const double dist = d1 - d2;
		
		if (abs(dist) < 0.0001)
			return false;
		else if (dist < 0)
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

	// Display the ride route on a google map
	void displayRides(const std::vector<QString>& filenames);

	// Enable/disable all the user controls
	void setEnabled(bool enabled);

public slots:
	// Stroke the ride path according to user selected parameter
	void definePathColour();

private slots:

 private:
	// Create the webpage to display google maps
	void createPage(std::ostringstream& page);

	// Create the webpage to show no GPS data
	void createEmptyPage(std::ostringstream& page);

	std::string defineCoords();
	bool parse(const QString filename, DataLog* data_log);

	TcxParser* _tcx_parser;
	FitParser* _fit_parser;

	// The window to display google maps
	QWebView *_view;

	QMap<LatLng, int> _accumulated_points; // key = lat,long, value = count
	QComboBox* _path_colour_scheme;
	ColourBar* _colour_bar;
};

#endif // GOOGLEMAPCOLLAGE_H