#ifndef HRZONEITEM_H
#define HRZONEITEM_H

#include <qwt_plot_item.h>
#include <qwt_text.h>

#include <QList.h>
#include <QPair.h>
#include <QColor.h>

class HRZoneItem
	: public QwtPlotItem
{

	public:
		HRZoneItem(const QwtText& title = QwtText());

		// Colour of the bar
		void setColour(const QColor& colour);
		// Width of the bar
		void setWidth(int width);
		// Set the min and max hr of the zone
		void setMinMax(const std::pair<int, int>& min_max);
		
		void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;
 
	private:
		std::pair<int, int> _min_max;
		QColor _colour;
		int _width; // width of bar
};

#endif
