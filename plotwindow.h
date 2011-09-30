#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <qtcore/qobject>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class DataLog;
class GoogleMap;

class PlotWindow : public QObject
{
	Q_OBJECT

 public:
	PlotWindow();
	~PlotWindow();

	void displayRide(DataLog& data_log, GoogleMap* google_map);

 private slots:
	void plotSelection(const QPointF& point);
	void plotMoved(int x, int y);

 private:
	QwtPlot* _plot;
	QwtPlotCurve* _curve_hr;
	QwtPlotCurve* _curve_alt;
};

#endif // GOOGLEMAP_H