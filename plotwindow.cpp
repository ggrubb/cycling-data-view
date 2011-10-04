#include "plotwindow.h"
#include "datalog.h"
#include "googlemap.h"

#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_picker_machine.h>
#include <iostream>

/******************************************************/
PlotWindow::PlotWindow()
{
	_plot = new QwtPlot();
	_plot->setAxisAutoScale(QwtPlot::xBottom ,true);
	_plot->resize(700,250);

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
        
}

/******************************************************/
PlotWindow::~PlotWindow()
{

}

/******************************************************/
void PlotWindow::displayRide(DataLog* data_log, GoogleMap* google_map)
{
	_data_log = data_log;

	_curve_hr->setSamples(&data_log->time(0), &data_log->heartRate(0), data_log->numPoints());
	_curve_speed->setSamples(&data_log->time(0), &data_log->speed(0), data_log->numPoints());
	_curve_grad->setSamples(&data_log->time(0), &data_log->gradient(0), data_log->numPoints());
	_curve_alt->setSamples(&data_log->time(0), &data_log->alt(0), data_log->numPoints());
	
	_curve_alt->attach(_plot);
	_curve_speed->attach(_plot);
	_curve_grad->attach(_plot);
	_curve_hr->attach(_plot);

	_plot->replot();
	_plot->show();

	// Plot picker for cursor display
	QwtPlotPicker* plot_picker1 = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, _plot->canvas());
	plot_picker1->setRubberBandPen(QColor(Qt::white));
    plot_picker1->setTrackerPen(QColor(Qt::black));
	plot_picker1->setStateMachine(new QwtPickerTrackerMachine());
	connect(plot_picker1, SIGNAL(moved(const QPointF&)), this, SLOT(setMarkerPosition(const QPointF&)));
	
	// Plot picker for user selection
	QwtPlotPicker* plot_picker2 = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOff, _plot->canvas());
	plot_picker2->setStateMachine(new QwtPickerDragPointMachine());
	connect(plot_picker2, SIGNAL(appended(const QPointF&)), this, SLOT(beginSelection(const QPointF&)));
	connect(plot_picker2, SIGNAL(moved(const QPointF&)), this, SLOT(endSelection(const QPointF&)));

	// Plot zoomer
	QwtPlotZoomer* plot_zoomer = new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, _plot->canvas());
    plot_zoomer->setRubberBand(QwtPicker::RectRubberBand);
    plot_zoomer->setRubberBandPen(QColor(Qt::black));
    plot_zoomer->setTrackerMode(QwtPicker::AlwaysOff);
    plot_zoomer->setTrackerPen(QColor(Qt::white));
    plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier);
    plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);
	connect(plot_zoomer, SIGNAL(zoomed(const QRectF&)), this, SLOT(zoomSelection(const QRectF&)));

	// Plot panner
	QwtPlotPanner* plot_panner = new QwtPlotPanner(_plot->canvas());
	plot_panner->setMouseButton(Qt::MidButton);
	connect(plot_panner, SIGNAL(moved(int, int)), this, SLOT(panSelection(int, int)));
	connect(plot_panner, SIGNAL(panned(int, int)), this, SLOT(panAndHoldSelection(int, int)));
	
	// Connect this window to the google map
	connect(this, SIGNAL(setMarkerPosition(int)), google_map, SLOT(setMarkerPosition(int)));
	connect(this, SIGNAL(beginSelection(int)), google_map, SLOT(beginSelection(int)));
	connect(this, SIGNAL(endSelection(int)), google_map, SLOT(endSelection(int)));
	connect(this, SIGNAL(zoomSelection(int,int)), google_map, SLOT(zoomSelection(int,int)));
	connect(this, SIGNAL(panSelection(int)), google_map, SLOT(moveSelection(int)));
	connect(this, SIGNAL(panAndHoldSelection(int)), google_map, SLOT(moveAndHoldSelection(int)));
}

/******************************************************/
void PlotWindow::setMarkerPosition(const QPointF& point)
{
	emit setMarkerPosition(_data_log->indexFromTime(point.x()));
}

/******************************************************/
void PlotWindow::beginSelection(const QPointF& point)
{
	emit beginSelection(_data_log->indexFromTime(point.x()));
}

/******************************************************/
void PlotWindow::endSelection(const QPointF& point)
{
	emit endSelection(_data_log->indexFromTime(point.x()));
}

/******************************************************/
void PlotWindow::zoomSelection(const QRectF& rect)
{
	emit zoomSelection(_data_log->indexFromTime(rect.left()), _data_log->indexFromTime(rect.right()));
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
