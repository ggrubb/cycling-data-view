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
	void setTimeVLtdLgd(DataLog& data_log);
	void createPage(std::ostringstream& page);
	std::string defineCoords(	
		QMap<double, std::pair<double, double> >::iterator first,
		QMap<double, std::pair<double, double> >::iterator last);
	void setSelection(const double& start_time, const double& end_time);

	QWebView *_view;
	QMap<double, std::pair<double, double> > _time_v_ltd_lgd;
	double _selection_begin_time;
	double _selection_end_time;
};

#endif // GOOGLEMAP_H