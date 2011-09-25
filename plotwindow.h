#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <qtcore/qobject>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class DataLog;

class PlotWindow : public QObject
{
	Q_OBJECT

 public:
	PlotWindow();
	~PlotWindow();

	void displayRide(DataLog& data_log);

 private slots:
	void plotSelection(const QPointF& point);

 private:
	QwtPlot* _plot;
	QwtPlotCurve* _curve_hr;
	QwtPlotCurve* _curve_alt;
};

#endif // GOOGLEMAP_H