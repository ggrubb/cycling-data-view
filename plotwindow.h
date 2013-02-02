#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget.h>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <boost/shared_ptr.hpp>

class DataLog;
class User;
class GoogleMapWindow;
class DataStatisticsWindow;
class HRZoneItem;
class QwtPlotPicker;
class QwtCustomPlotZoomer;
class QwtCustomPlotPicker;
class QwtPlotPanner;
class QwtPlotMarker;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QLabel;

class PlotWindow : public QWidget
{
	Q_OBJECT

 public:
	// Create plot and connect to google map and stats viewer
	PlotWindow(
		boost::shared_ptr<GoogleMapWindow> google_map, 
		boost::shared_ptr<DataStatisticsWindow> stats_view);
	~PlotWindow();
	
	// Display the ride log in plots
	void displayRide(
		boost::shared_ptr<DataLog> data_log, 
		boost::shared_ptr<User> user);
	
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
	QwtPlotCurve* _curve_power;
	QwtPlotCurve* _curve_temp;

	std::vector<QwtPlotMarker* > _lap_markers;
	std::vector<HRZoneItem* > _hr_zone_markers;
	
	boost::shared_ptr<QCheckBox> _hr_cb;
	boost::shared_ptr<QCheckBox> _speed_cb;
	boost::shared_ptr<QCheckBox> _alt_cb;
	boost::shared_ptr<QCheckBox> _cadence_cb;
	boost::shared_ptr<QCheckBox> _power_cb;
	boost::shared_ptr<QCheckBox> _temp_cb;
	QComboBox* _x_axis_measurement;
	QSpinBox *_smoothing_selection;
	QCheckBox* _hr_zones_cb;
	QCheckBox* _laps_cb;
	
	QwtCustomPlotPicker* _plot_picker1;
	QwtPlotPicker* _plot_picker2;
	QwtCustomPlotZoomer* _plot_zoomer;
	QwtPlotPanner* _plot_panner;

	boost::shared_ptr<DataLog> _data_log;
	boost::shared_ptr<User> _user;
};

#endif // PLOTWINDOW_H