#include "plotwindow.h"
#include "datalog.h"
#include "googlemap.h"
#include "datastatisticsview.h"

#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_picker_machine.h>
#include <qwt_painter.h>
#include <qtgui/qcombobox>
#include <qtgui/qvboxlayout>
#include <iostream>

// A custom plot zoomer, which defines a better rubber band, and zooms on x-axis only
class QwtPlotCustomZoomer : public QwtPlotZoomer
{
public:
	QwtPlotCustomZoomer(int xAxis, int yAxis, QwtPlotCanvas* canvas, bool doReplot=true):
	QwtPlotZoomer(xAxis,yAxis,canvas,doReplot)
	{}

	// Override user selected points to only take their x coord selection
	bool accept(QPolygon& p) const
	{
		if ( p.count() < 2 )
			return true;

		// Set the zoom rect to be top to bottm, irrespective of what the user selects in y axis
		p[0].setY(0);
		p[1].setY(200);
		return true;
	}

	// Draw a nice rubber band
	void drawRubberBand(QPainter* painter) const
	{
		if ( rubberBand() < UserRubberBand )
			QwtPlotPicker::drawRubberBand( painter );
		else
		{
			if ( !isActive() || rubberBandPen().style() == Qt::NoPen )
			{
				return;
			}

			QPolygon p = selection();

			if ( p.count() < 2 )
				return;

			const QPoint& pt1 = p[0];
			const QPoint& pt2 = p[1];

			const int end = 250;
			painter->drawLine(pt1.x(), 0, pt1.x(), end);
			painter->drawLine(pt2.x(), 0, pt2.x(), end);
			painter->fillRect(QRect(pt1.x(), 0, pt2.x() - pt1.x(), end), QBrush(QColor("black"), Qt::Dense7Pattern));
		}
	}

};

/******************************************************/
PlotWindow::PlotWindow()
{
	_data_log = new DataLog();

	_plot = new QwtPlot();
	_plot->setAxisScale(QwtPlot::yLeft, 0, 250);
	_plot->setAxisScale(QwtPlot::xBottom, 0, 10000);

	QColor c;

	_curve_hr = new QwtPlotCurve("Heart Rate");
	c = Qt::darkRed;
	_curve_hr->setPen(c);

	_curve_grad = new QwtPlotCurve("Gradient");
	c = Qt::darkBlue;
	_curve_grad->setPen(c);

	_curve_speed = new QwtPlotCurve("Speed");
	c = Qt::yellow;
	_curve_speed->setPen(c);

	_curve_alt = new QwtPlotCurve("Altitude");
	_curve_alt->setRenderHint(QwtPlotItem::RenderAntialiased);
	c = Qt::darkGreen;
	c.setAlpha(150);
	_curve_alt->setPen(c);
    _curve_alt->setBrush(c);

	_curve_alt->attach(_plot);
	_curve_speed->attach(_plot);
	_curve_grad->attach(_plot);
	_curve_hr->attach(_plot);

	// Plot picker for cursor display
	_plot_picker1 = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, _plot->canvas());
	_plot_picker1->setRubberBandPen(QColor(Qt::white));
    _plot_picker1->setTrackerPen(QColor(Qt::black));
	_plot_picker1->setStateMachine(new QwtPickerTrackerMachine());
	connect(_plot_picker1, SIGNAL(moved(const QPointF&)), this, SLOT(setMarkerPosition(const QPointF&)));
	
	// Plot picker for user selection
	_plot_picker2 = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOff, _plot->canvas());
	_plot_picker2->setStateMachine(new QwtPickerDragPointMachine());
	connect(_plot_picker2, SIGNAL(appended(const QPointF&)), this, SLOT(beginSelection(const QPointF&)));
	connect(_plot_picker2, SIGNAL(moved(const QPointF&)), this, SLOT(endSelection(const QPointF&)));

	// Plot zoomer
	QwtPlotCustomZoomer* _plot_zoomer = new QwtPlotCustomZoomer(QwtPlot::xBottom, QwtPlot::yLeft, _plot->canvas());
	_plot_zoomer->setRubberBand(QwtPicker::UserRubberBand);
    _plot_zoomer->setRubberBandPen(QColor(Qt::white));
    _plot_zoomer->setTrackerMode(QwtPicker::AlwaysOff);
    _plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier);
    _plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);
	connect(_plot_zoomer, SIGNAL(zoomed(const QRectF&)), this, SLOT(zoomSelection(const QRectF&)));

	// Plot panner
	_plot_panner = new QwtPlotPanner(_plot->canvas());
	_plot_panner->setMouseButton(Qt::MidButton);
	connect(_plot_panner, SIGNAL(moved(int, int)), this, SLOT(panSelection(int, int)));
	connect(_plot_panner, SIGNAL(panned(int, int)), this, SLOT(panAndHoldSelection(int, int)));

	// Selection for x-axis
	_x_axis_measurement = new QComboBox();
	_x_axis_measurement->insertItem(0,"Time");
	_x_axis_measurement->insertItem(1,"Distance");
	connect(_x_axis_measurement,SIGNAL(currentIndexChanged(int)), this, SLOT(xAxisUnitsChanged(int)));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(_plot);
	layout->addWidget(_x_axis_measurement);
	resize(700,270);
}

/******************************************************/
PlotWindow::~PlotWindow()
{

}

/******************************************************/
void PlotWindow::displayRide(DataLog* data_log, GoogleMap* google_map, DataStatisticsView* stats_view)
{
	_data_log = data_log;

	drawGraphs();
	show();
	
	// Connect this window to the google map
	connect(this, SIGNAL(setMarkerPosition(int)), google_map, SLOT(setMarkerPosition(int)));
	connect(this, SIGNAL(beginSelection(int)), google_map, SLOT(beginSelection(int)));
	connect(this, SIGNAL(endSelection(int)), google_map, SLOT(endSelection(int)));
	connect(this, SIGNAL(zoomSelection(int,int)), google_map, SLOT(zoomSelection(int,int)));
	connect(this, SIGNAL(panSelection(int)), google_map, SLOT(moveSelection(int)));
	connect(this, SIGNAL(panAndHoldSelection(int)), google_map, SLOT(moveAndHoldSelection(int)));

	// Connect this window to the statistical viewer
	connect(this, SIGNAL(zoomSelection(int,int)), stats_view, SLOT(setSelection(int,int)));
}

/******************************************************/
void PlotWindow::drawGraphs()
{
	if (_x_axis_measurement->currentIndex() == 0) // time
	{
		_curve_hr->setRawSamples(&_data_log->time(0), &_data_log->heartRate(0), _data_log->numPoints());
		_curve_speed->setRawSamples(&_data_log->time(0), &_data_log->speed(0), _data_log->numPoints());
		_curve_grad->setRawSamples(&_data_log->time(0), &_data_log->gradient(0), _data_log->numPoints());
		_curve_alt->setRawSamples(&_data_log->time(0), &_data_log->alt(0), _data_log->numPoints());
		
		_plot->setAxisScale(QwtPlot::xBottom, 0, _data_log->totalTime());
	}
	else // distance
	{
		_curve_hr->setRawSamples(&_data_log->dist(0), &_data_log->heartRate(0), _data_log->numPoints());
		_curve_speed->setRawSamples(&_data_log->dist(0), &_data_log->speed(0), _data_log->numPoints());
		_curve_grad->setRawSamples(&_data_log->dist(0), &_data_log->gradient(0), _data_log->numPoints());
		_curve_alt->setRawSamples(&_data_log->dist(0), &_data_log->alt(0), _data_log->numPoints());
		
		_plot->setAxisScale(QwtPlot::xBottom, 0, _data_log->totalDist());
	}
	_plot->replot();
}

/******************************************************/
void PlotWindow::setMarkerPosition(const QPointF& point)
{
	if (_x_axis_measurement->currentIndex() == 0) // time
		emit setMarkerPosition(_data_log->indexFromTime(point.x()));
	else // distance
		emit setMarkerPosition(_data_log->indexFromDist(point.x()));
}		

/******************************************************/
void PlotWindow::beginSelection(const QPointF& point)
{
	_plot_picker1->setRubberBand(QwtPlotPicker::NoRubberBand);
	_plot_picker1->setEnabled(false);
	if (_x_axis_measurement->currentIndex() == 0) // time
		emit beginSelection(_data_log->indexFromTime(point.x()));
	else // distance
		emit beginSelection(_data_log->indexFromDist(point.x()));
}

/******************************************************/
void PlotWindow::endSelection(const QPointF& point)
{
	if (_x_axis_measurement->currentIndex() == 0) // time
		emit endSelection(_data_log->indexFromTime(point.x()));
	else // distance
		emit endSelection(_data_log->indexFromDist(point.x()));
}

/******************************************************/
void PlotWindow::zoomSelection(const QRectF& rect)
{
	_plot_picker1->setRubberBand(QwtPlotPicker::CrossRubberBand);
	_plot_picker1->setEnabled(true);
	if (_x_axis_measurement->currentIndex() == 0) // time
		emit zoomSelection(_data_log->indexFromTime(rect.left()), _data_log->indexFromTime(rect.right()));
	else // distance
		emit zoomSelection(_data_log->indexFromDist(rect.left()), _data_log->indexFromDist(rect.right()));
}

/******************************************************/
void PlotWindow::panSelection(int x, int y)
{
	emit panSelection(_plot->invTransform(QwtPlot::xBottom,x)-_plot->invTransform(QwtPlot::xBottom,0));
}

/******************************************************/
void PlotWindow::panAndHoldSelection(int x, int y)
{
	emit panAndHoldSelection(_plot->invTransform(QwtPlot::xBottom,x)-_plot->invTransform(QwtPlot::xBottom,0));
}

/******************************************************/
void PlotWindow::xAxisUnitsChanged(int idx)
{
	drawGraphs();
}
