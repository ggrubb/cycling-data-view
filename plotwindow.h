#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <qtcore/qobject>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class DataLog;
class GoogleMap;
class QwtPlotPicker;

class PlotWindow : public QObject
{
	Q_OBJECT

 public:
	PlotWindow();
	~PlotWindow();
	// Display the ride log in plots, and connect curser to google map
	void displayRide(DataLog* data_log, GoogleMap* google_map);

 signals:
	void setMarkerPosition(int idx);
	void beginSelection(int idx_begin);
	void endSelection(int idx_end);
	void zoomSelection(int idx_begin, int idx_end);
	void panSelection(int idx_delta);
	void panAndHoldSelection(int idx_delta);

 private slots:
	void setMarkerPosition(const QPointF& point);
	void beginSelection(const QPointF& point);
	void endSelection(const QPointF& point);
	void zoomSelection(const QRectF& rect);
	void panSelection(int x, int y);
	void panAndHoldSelection(int x, int y);

 private:
	QwtPlot* _plot;
	QwtPlotCurve* _curve_hr;
	QwtPlotCurve* _curve_speed;
	QwtPlotCurve* _curve_grad;
	QwtPlotCurve* _curve_alt;
	DataLog* _data_log;
};

#endif // PLOTWINDOW_H