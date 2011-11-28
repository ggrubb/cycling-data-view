#ifndef GOOGLEMAPCOLLAGE_H
#define GOOGLEMAPCOLLAGE_H

#include <qtxml/qdomdocument>
#include <QWidget.h>
#include <QPoint.h>
#include <QWebView.h>
#include <QMap.h>

class DataLog;
class TcxParser;
class FitParser;
class QComboBox;
class ColourBar;

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

	// Pointer to the data logs
	std::vector<DataLog*> _data_logs;
	QComboBox* _path_colour_scheme;
	ColourBar* _colour_bar;
};

#endif // GOOGLEMAPCOLLAGE_H