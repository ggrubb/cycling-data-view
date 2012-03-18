#include "barchartitem.h"

#include <qwt_scale_map.h>
#include <qwt_plot.h>

#include <QPainter.h>
#include <QPen.h>
#include <QBrush.h>

/******************************************************/
BarChartItem::BarChartItem(const QwtText& title) : 
QwtPlotItem(title),
_bar_colour(Qt::red),
_width_sec(10),
_offset_sec(0)
{}

/******************************************************/
void BarChartItem::setColour(const QColor& colour)
{
	_bar_colour = colour;
}

/******************************************************/
void BarChartItem::setTimeWidth(int width_sec)
{
	_width_sec = width_sec;
}

/******************************************************/
void BarChartItem::setTimeOffset(int offset_sec)
{
	_offset_sec = offset_sec;
}

/******************************************************/
void BarChartItem::setData(const QList< QPair<int, int> >& data)
{
	_data = data;
}

/******************************************************/
void BarChartItem::scalePlotAxis()
{
	if (_data.size() > 0)
	{
		// Set X axis
		plot()->setAxisScale(QwtPlot::xBottom, _data.begin()->second, (_data.end()-1)->second + _width_sec*2);

		// Set Y axis
		int max;
		QList< QPair<int, int> >::const_iterator itr;
		for(max = 0,itr = _data.begin(); itr != _data.end(); ++itr)
		{
			if((*itr).first > max)
				max = (*itr).first;
		}
		plot()->setAxisScale(yAxis(), 0, max);
	}
}

/******************************************************/
const QList< QPair<int, int> >& BarChartItem::data(void) const
{
	return _data;
}

/******************************************************/
int BarChartItem::numElements() const
{
	return _data.size();
}

/******************************************************/
void BarChartItem::draw(QPainter* painter, const QwtScaleMap& xmap, const QwtScaleMap& ymap, const QRectF& canvasRect) const
{
	for(int i = 0;i <_data.size();++i)
	{
		// Paint the bar
		painter->fillRect(xmap.transform(_data[i].second + _offset_sec), //x
		                  ymap.transform(0), //y
		                  xmap.transform(_width_sec) - xmap.transform(0), //w
		                  ymap.transform(_data[i].first) - ymap.transform(0),  //h
		                  QBrush(_bar_colour));
	}
}