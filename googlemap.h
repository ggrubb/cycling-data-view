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

	void displayRide(DataLog& data_log);

private slots:
	void setMarkerPosition(const QPointF& point);
	void beginSelection(const QPointF& point);
	void endSelection(const QPointF& point);
	void zoomSelection(const QRectF& rect);
	void moveSelection(int x, int y);
 private:
	void setTimeVLtdLgd(DataLog& data_log);
	void createPage(std::ostringstream& page);
	std::string defineCoords(	
		QMap<double, std::pair<double, double> >::iterator first,
		QMap<double, std::pair<double, double> >::iterator last);

	QWebView *_view;
	QMap<double, std::pair<double, double> > _time_v_ltd_lgd;
	double _selection_begin_time;
	double _selection_end_time;
};

#endif // GOOGLEMAP_H