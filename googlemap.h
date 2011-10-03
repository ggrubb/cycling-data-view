#ifndef GOOGLEMAP_H
#define GOOGLEMAP_H

#include "datalog.h"

#include <qtxml/qdomdocument>
#include <qtcore/qobject>
#include <qtcore/qpoint>
#include <QWebView.h>
#include <qmap.h>

//class DataLog;

class GoogleMap : public QObject
{
	Q_OBJECT

 public:
	GoogleMap();
	~GoogleMap();

	// Display the ride route on a google map
	void displayRide(DataLog& data_log);

private slots:
	// Set marker in google map
	void setMarkerPosition(const QPointF& point);
	// Call when user begins to highlight a seletection (to highlight path on the map)
	void beginSelection(const QPointF& point);
	// Call when a user has complted highlighting a selection (to highlight path on the map)
	void endSelection(const QPointF& point);
	// Call when a user defines a selection to zoom (to highlight path on the map)
	void zoomSelection(const QRectF& rect);
	// Call when a user moves the selected region (to highlight path on the map)
	void moveSelection(double delta_x);
	// Call when a user completes moving the selected region (to highlight path on the map)
	void holdSelection(double delta_x);

 private:
	// Convert raw data log into internal map structure for easy lookup
	void setTimeVLtdLgd(DataLog& data_log);
	void createPage(std::ostringstream& page);
	// Create sting decription of lat/long between first and last iterators
	std::string defineCoords(	
		QMap<double, std::pair<double, double> >::iterator first,
		QMap<double, std::pair<double, double> >::iterator last);
	// Draw the path between the start and end time on the map
	void setSelection(const double& start_time, const double& end_time);

	// The window to display google maps
	QWebView *_view;
	// Map to define association between time and lat/long
	QMap<double, std::pair<double, double> > _time_v_ltd_lgd;
	// The start time of selection to highlight
	double _selection_begin_time;
	// The end time of selection to highlight
	double _selection_end_time;
};

#endif // GOOGLEMAP_H