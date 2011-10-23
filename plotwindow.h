#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <qtgui/qwidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class DataLog;
class GoogleMap;
class DataStatisticsView;
class QwtPlotPicker;
class QwtCustomPlotZoomer;
class QwtCustomPlotPicker;
class QwtPlotPanner;
class QButtonGroup;
class QCheckBox;
class QRadioButton;
class QSlider;

class PlotWindow : public QWidget
{
	Q_OBJECT

 public:
	PlotWindow();
	~PlotWindow();
	
	// Display the ride log in plots, and connect curser to google map
	void displayRide(DataLog* data_log, GoogleMap* google_map, DataStatisticsView* stats_view);

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

 private slots:
	void setMarkerPosition(const QPointF& point);
	void beginSelection(const QPointF& point);
	void endSelection(const QPointF& point);
	void zoomSelection(const QRectF& rect);
	void panSelection(int x, int y);
	void panAndHoldSelection(int x, int y);
	void xAxisUnitsChanged(int idx);
	void curveSelectionChanged();
	void signalSmoothingChanged();

 private:
	void drawGraphs();
	void setCurveData();

	QwtPlot* _plot;
	QwtPlotCurve* _curve_hr;
	QwtPlotCurve* _curve_speed;
	QwtPlotCurve* _curve_cadence;
	QwtPlotCurve* _curve_alt;
	
	QButtonGroup* _x_axis_measurement;
	QRadioButton* _time_axis;
	QRadioButton* _dist_axis;
	QCheckBox* _hr_cb;
	QCheckBox* _speed_cb;
	QCheckBox* _alt_cb;
	QCheckBox* _cadence_cb;
	QSlider* _smoothing_selection;
	
	QwtCustomPlotPicker* _plot_picker1;
	QwtPlotPicker* _plot_picker2;
	QwtCustomPlotZoomer* _plot_zoomer;
	QwtPlotPanner* _plot_panner;

	DataLog* _data_log;
	std::vector<double>* _data_hr_filtered;
	std::vector<double>* _data_speed_filtered;
	std::vector<double>* _data_cadence_filtered;
	std::vector<double>* _data_alt_filtered;
};

#endif // PLOTWINDOW_H