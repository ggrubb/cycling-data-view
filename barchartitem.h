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

		void setColour(const QColor& colour);
		void setData(const QList< QPair<int, QString> >& heights);
		const QList< QPair<int, QString> >& data(void) const;

		void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;
 
	private:
		QList< QPair<int, QString> > _data;
		QColor _bar_colour;

};

#endif
