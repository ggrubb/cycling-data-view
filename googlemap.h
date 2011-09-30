#ifndef GOOGLEMAP_H
#define GOOGLEMAP_H

#include "datalog.h"

#include <qtxml/qdomdocument>
#include <qtcore/qobject>
#include <qtcore/qpoint>
#include <QWebView.h>

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

 private:
	void createPage(std::ostringstream& page, DataLog& data_log);
	std::string createPolyline(DataLog& data_log);

	QWebView *_view;
	DataLog _data_log;
};

#endif // GOOGLEMAP_H