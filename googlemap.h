#ifndef GOOGLEMAP_H
#define GOOGLEMAP_H

#include <qtxml/qdomdocument>
#include <qtcore/qobject>
#include <qtcore/qpoint>
#include <QWebView.h>

class DataLog;

class GoogleMap : public QObject
{
	Q_OBJECT

 public:
	GoogleMap();
	~GoogleMap();

	void displayRide(DataLog& data_log);

 private:
	void createPage(std::ostringstream& page, DataLog& data_log);
	std::string createPolyline(DataLog& data_log);

	QWebView *_view;
};

#endif // GOOGLEMAP_H