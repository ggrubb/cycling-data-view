#include "plotwindow.h"
#include "datalog.h"
#include "googlemap.h"

#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_picker_machine.h>
#include <iostream>

class QwtPlotPickerHighlightSelection: public QwtPlotPicker
{
public:
	QwtPlotPickerHighlightSelection(int xAxis,int yAxis,QwtPicker::RubberBand rubberBand,QwtPicker::DisplayMode trackerMode,QwtPlotCanvas* canvas):
	QwtPlotPicker(xAxis,yAxis,rubberBand,trackerMode,canvas)
	{
	
	}
	
protected: 
	virtual void widgetMousePressedEvent( QMouseEvent *me )
	{
		if ( mouseMatch( QwtEventPattern::MouseSelect1, me ) )
		{
			//std::cout << "here!\n";
		}
	}

};

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
void PlotWindow::displayRide(DataLog& data_log, GoogleMap* google_map)
{
	_curve_hr->setSamples(&data_log.time(0), &data_log.heartRate(0), data_log.numPoints());
	_curve_alt->setSamples(&data_log.time(0), &data_log.alt(0), data_log.numPoints());
	_curve_hr->attach(_plot);
	_curve_alt->attach(_plot);

	_plot->replot();
	_plot->show();

	// Plot picker for cursor display
	QwtPlotPicker* plot_picker1 = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, _plot->canvas());
	plot_picker1->setRubberBandPen(QColor(Qt::white));
    plot_picker1->setTrackerPen(QColor(Qt::black));
	plot_picker1->setStateMachine(new QwtPickerTrackerMachine());
	connect(plot_picker1, SIGNAL(moved(const QPointF&)), this, SLOT(plotSelection(const QPointF&)));
	connect(plot_picker1, SIGNAL(moved(const QPointF&)), google_map, SLOT(setMarkerPosition(const QPointF&)));
	
	// Plot picker for user selection
	QwtPlotPicker* plot_picker2 = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOff, _plot->canvas());
	plot_picker2->setStateMachine(new QwtPickerDragPointMachine());
	connect(plot_picker2, SIGNAL(appended(const QPointF&)), google_map, SLOT(beginSelection(const QPointF&)));
	connect(plot_picker2, SIGNAL(moved(const QPointF&)), google_map, SLOT(endSelection(const QPointF&)));

	// Plot zoomer
	QwtPlotZoomer* plot_zoomer = new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, _plot->canvas());
    plot_zoomer->setRubberBand(QwtPicker::RectRubberBand);
    plot_zoomer->setRubberBandPen(QColor(Qt::black));
    plot_zoomer->setTrackerMode(QwtPicker::AlwaysOff);
    plot_zoomer->setTrackerPen(QColor(Qt::white));
    plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect2,Qt::RightButton, Qt::ControlModifier);
    plot_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);
	connect(plot_zoomer, SIGNAL(zoomed(const QRectF&)), google_map, SLOT(clearSelection(const QRectF&)));

	// Plot panner
	QwtPlotPanner* plot_panner = new QwtPlotPanner(_plot->canvas());
	connect(plot_panner, SIGNAL(moved(int, int)), this, SLOT(plotMoved(int , int)));
	plot_panner->setMouseButton(Qt::MidButton);
}

/******************************************************/
void PlotWindow::plotSelection(const QPointF& point)
{
	//std::cout << point.x() << " " << point.y() << std::endl;
}

/******************************************************/
void PlotWindow::plotMoved(int x, int y)
{
	_plot->updateAxes();
	_plot->replot();
}