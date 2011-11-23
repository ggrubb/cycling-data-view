#ifndef GOOGLEMAP_H
#define GOOGLEMAP_H

#include <qtxml/qdomdocument>
#include <QWidget.h>
#include <QPoint.h>
#include <QWebView.h>
#include <QMap.h>

class DataLog;
class QComboBox;
class ColourBar;

class GoogleMap : public QWidget
{
	Q_OBJECT

 public:
	GoogleMap();
	~GoogleMap();

	// Display the ride route on a google map
	void displayRide(DataLog* data_log);

	// Enable/disable all the user controls
	void setEnabled(bool enabled);

public slots:
	// Stroke the ride path according to user selected parameter
	void definePathColour();

private slots:
	// Set marker in google map
	void setMarkerPosition(int idx);

	// Call when user begins to highlight a seletection (to highlight path on the map)
	void beginSelection(int idx_begin);

	// Call when a user has complted highlighting a selection (to highlight path on the map)
	void endSelection(int idx_end);

	// Call when a user defines a selection to zoom (to highlight path on the map)
	void zoomSelection(int idx_start, int idx_end);

	// Call when user returns to full zoom, so no selection to highligh
	void deleteSelection();

	// Call when a user moves the selected region (to highlight path on the map)
	void moveSelection(int delta_idx);

	// Call when a user completes moving the selected region (to highlight path on the map)
	void moveAndHoldSelection(int delta_idx);

 private:
	 // Create the webpage and put in the given stream
	void createPage(std::ostringstream& page);

	// Create sting decription of lat/long between first and last iterators
	std::string defineCoords(int idx_start, int idx_end);

	// Draw the path between the start and end time on the map. Bool param to define whether to zoom the map
	void setSelection(int idx_start, int idx_end, bool zoom_map);

	// The window to display google maps
	QWebView *_view;
	// Map to define association between time and lat/long
	QMap<double, std::pair<double, double> > _time_v_ltd_lgd;
	// The start index of selection to highlight
	int _selection_begin_idx;
	// The end index of selection to highlight
	int _selection_end_idx;
	// Pointer to the data log
	DataLog* _data_log;
	QComboBox* _path_colour_scheme;
	ColourBar* _colour_bar;
};

#endif // GOOGLEMAP_H