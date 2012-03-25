#include "hrzoneitem.h"

#include <qwt_scale_map.h>
#include <qwt_plot.h>

#include <QPainter.h>
#include <QPen.h>
#include <QBrush.h>

/******************************************************/
HRZoneItem::HRZoneItem(const QwtText& title) : 
QwtPlotItem(title),
_colour(Qt::red),
_width(0)
{}

/******************************************************/
void HRZoneItem::setColour(const QColor& colour)
{
	_colour = colour;
}

/******************************************************/
void HRZoneItem::setWidth(int width)
{
	_width = width;
}

/******************************************************/
void HRZoneItem::setMinMax(const std::pair<int, int>& min_max)
{
	_min_max = min_max;
}

/******************************************************/
void HRZoneItem::draw(QPainter* painter, const QwtScaleMap& xmap, const QwtScaleMap& ymap, const QRectF& canvasRect) const
{
	// Paint the bar
	painter->fillRect(xmap.transform(0), //x
	                  ymap.transform(_min_max.second), //y
	                  xmap.transform(_width) - xmap.transform(0), //w
	                  ymap.transform(_min_max.first) - ymap.transform(_min_max.second),  //h
	                  QBrush(_colour));
}