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
	void displayRide(DataLog& data_log, GoogleMap* google_map);

 signals:
	void panSelection(double delta_x);
	void panAndHoldSelection(double delta_x);

 private slots:
	void panSelection(int x, int y);
	void panAndHoldSelection(int x, int y);

 private:
	QwtPlot* _plot;
	QwtPlotCurve* _curve_hr;
	QwtPlotCurve* _curve_speed;
	QwtPlotCurve* _curve_grad;
	QwtPlotCurve* _curve_alt;
};

#endif // PLOTWINDOW_H