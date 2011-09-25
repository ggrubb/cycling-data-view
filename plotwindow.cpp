#include "plotwindow.h"
#include "datalog.h"

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

	_curve_hr = new QwtPlotCurve("Heart Rate");
	_curve_alt = new QwtPlotCurve("Altitude");
}

/******************************************************/
PlotWindow::~PlotWindow()
{

}

/******************************************************/
void PlotWindow::displayRide(DataLog& data_log)
{
	_curve_hr->setSamples(&data_log.time(0), &data_log.heartRate(0), data_log.numPoints());
	_curve_alt->setSamples(&data_log.time(0), &data_log.alt(0), data_log.numPoints());
	_curve_hr->attach(_plot);
	_curve_alt->attach(_plot);

	_plot->replot();
	_plot->show();

	// Plot picker
	QwtPlotPicker* plot_picker = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, _plot->canvas());
	connect(plot_picker, SIGNAL(selected(const QPointF&)), this, SLOT(plotSelection(const QPointF&)));
	plot_picker->setStateMachine(new QwtPickerDragPointMachine());
	plot_picker->setRubberBandPen(QColor(Qt::green));
    plot_picker->setRubberBand(QwtPicker::CrossRubberBand);
    plot_picker->setTrackerPen(QColor(Qt::white));

	// Plot zoomer
	QwtPlotZoomer* plot_zoomer = new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, _plot->canvas());
    plot_zoomer->setRubberBand(QwtPicker::RectRubberBand);
    plot_zoomer->setRubberBandPen(QColor(Qt::green));
    plot_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    plot_zoomer->setTrackerPen(QColor(Qt::white));
	//plot_zoomer->setTrackerMode(QwtPicker::AlwaysOff);
    plot_zoomer->setRubberBand(QwtPicker::NoRubberBand);
    plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier);
    plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);

	// Plot panner
	QwtPlotPanner* plot_panner = new QwtPlotPanner(_plot->canvas());
	plot_panner->setMouseButton(Qt::MidButton);
}

/******************************************************/
void PlotWindow::plotSelection(const QPointF& point)
{
	std::cout << point.x() << " " << point.y() << std::endl;
}