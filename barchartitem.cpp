#include "barchartitem.h"

#include <qwt_scale_map.h>
#include <qwt_plot.h>

#include <QPainter.h>
#include <QPen.h>
#include <QBrush.h>

/******************************************************/
BarChartItem::BarChartItem(const QwtText& title) : 
QwtPlotItem(title),
_bar_colour(Qt::red)
{}

/******************************************************/
void BarChartItem::setColour(const QColor& colour)
{
	_bar_colour = colour;
}

/******************************************************/
void BarChartItem::setData(const QList< QPair<int, QString> >& data)
{
	_data = data;

	// Set X axis
	plot()->setAxisScale(QwtPlot::xBottom, 0, (_data.size()*2)-1);

	// Set Y axis
	int max;
	QList< QPair<int, QString> >::const_iterator itr;
	for(max = 0,itr = _data.begin(); itr != _data.end(); ++itr)
	{
		if((*itr).first > max)
			max = (*itr).first;
	}
	plot()->setAxisScale(QwtPlot::yLeft, 0, max);
}

/******************************************************/
const QList< QPair<int, QString> >& BarChartItem::data(void) const
{
	return _data;
}

/******************************************************/
void BarChartItem::draw(QPainter* painter, const QwtScaleMap& xmap, const QwtScaleMap& ymap, const QRectF& canvasRect) const
{
	for(int i = 0;i <_data.size();++i)
	{
		// Paint the bar
		painter->fillRect(xmap.transform(i*2),
		                  ymap.transform(0),
		                  xmap.transform((i*2)+1) - xmap.transform(i*2),
		                  ymap.transform(_data[i].first) - ymap.transform(0),
		                  QBrush(_bar_colour));
	}
}