#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class DataLog;
class User;
class GoogleMapWindow;
class DataStatisticsWindow;
class QwtPlotPicker;
class QwtCustomPlotZoomer;
class QwtCustomPlotPicker;
class QwtPlotPanner;
class QwtPlotMarker;
class QComboBox;
class QCheckBox;
class QSlider;
class QLabel;

class PlotWindow : public QWidget
{
	Q_OBJECT

 public:
	// Create plot and connect to google map and stats viewer
	PlotWindow(GoogleMapWindow* google_map, DataStatisticsWindow* stats_view);
	~PlotWindow();
	
	// Display the ride log in plots
	void displayRide(DataLog* data_log, User* user);
	
	// Zoom to highlight a lap
	void displayLap(int lap_index);
	
	// Enable/disable all the user controls
	void setEnabled(bool enabled);

 signals:
	void setMarkerPosition(int idx);
	void beginSelection(int idx_begin);
	void endSelection(int idx_end);
	void zoomSelection(int idx_begin, int idx_end);
	void deleteSelection();
	void panSelection(int idx_delta);
	void panAndHoldSelection(int idx_delta);
	void updateDataView();

 private slots:
	void setMarkerPosition(const QPointF& point);
	void beginSelection(const QPointF& point);
	void endSelection(const QPointF& point);
	void zoomSelection(const QRectF& rect);
	void panSelection(int x, int y);
	void panAndHoldSelection(int x, int y);
	void xAxisUnitsChanged(int idx);
	void curveSelectionChanged();
	void lapSelectionChanged();
	void hrZoneSelectionChanged();
	void signalSmoothingChanged();

 private:
	void drawGraphs();
	void filterCurveData();
	void setCurveData();

	void drawLapMarkers();
	void clearLapMarkers();

	void drawHRZoneMarkers();
	void clearHRZoneMarkers();

	QwtPlot* _plot;
	QwtPlotCurve* _curve_hr;
	QwtPlotCurve* _curve_speed;
	QwtPlotCurve* _curve_cadence;
	QwtPlotCurve* _curve_alt;

	std::vector<QwtPlotMarker* > _lap_markers;
	std::vector<QwtPlotMarker* > _hr_zone_markers;
	
	QComboBox* _x_axis_measurement;
	QCheckBox* _hr_cb;
	QCheckBox* _speed_cb;
	QCheckBox* _alt_cb;
	QCheckBox* _cadence_cb;
	QCheckBox* _power_cb;
	QCheckBox* _temp_cb;
	QSlider* _smoothing_selection;
	QLabel* _smoothing_label;
	QCheckBox* _hr_zones_cb;
	QCheckBox* _laps_cb;
	
	QwtCustomPlotPicker* _plot_picker1;
	QwtPlotPicker* _plot_picker2;
	QwtCustomPlotZoomer* _plot_zoomer;
	QwtPlotPanner* _plot_panner;

	DataLog* _data_log;
	User* _user;
};

#endif // PLOTWINDOW_H