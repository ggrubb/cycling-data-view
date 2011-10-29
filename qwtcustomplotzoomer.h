#ifndef QWTCUSTOMPLOTZOOMER_H
#define QWTCUSTOMPLOTZOOMER_H

#include <qwt_plot_zoomer.h>

// A custom plot zoomer, which defines a better rubber band, and zooms on x-axis only
class QwtCustomPlotZoomer : public QwtPlotZoomer
{
public:
	QwtCustomPlotZoomer(int x_axis, int y_axis, QwtPlotCanvas* canvas, bool do_replot=true);

protected:
	// Override user selected points to only take their x coord selection
	bool accept(QPolygon& p) const;

	// Draw a nice rubber band
	void drawRubberBand(QPainter* painter) const;
};

#endif // QWTCUSTOMPLOTZOOMER_H