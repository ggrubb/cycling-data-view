#ifndef BARCHARTITEM_H
#define BARCHARTITEM_H

#include <qwt_plot_item.h>
#include <qwt_text.h>

#include <QList.h>
#include <QPair.h>
#include <QColor.h>

class BarChartItem
	: public QwtPlotItem
{

	public:
		BarChartItem(const QwtText& title = QwtText());

		// Colour of the bar
		void setColour(const QColor& colour);
		// Width of the bar
		void setTimeWidth(int width_sec);
		// Offset of the bar (ie shift to the right on the plot)
		void setTimeOffset(int offset_sec);
		// Set the plot data (pairs: hegiht, time interval)
		void setData(const QList< QPair<int, int> >& heights);
		const QList< QPair<int, int> >& data(void) const;
		// Return the number of elements
		int numElements() const;
		// Scale the plot axis according the data
		void scalePlotAxis();

		void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;
 
	private:
		QList< QPair<int, int> > _data;
		QColor _bar_colour;
		int _width_sec; // width of bars, in seconds
		int _offset_sec; // offset of bars, in seconds
};

#endif
