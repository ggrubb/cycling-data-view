#include "plotwindow.h"
#include "datalog.h"
#include "googlemap.h"
#include "datastatisticsview.h"

#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_picker_machine.h>
#include <qwt_painter.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qtgui/qcombobox>
#include <qtgui/qvboxlayout>
#include <qtgui/qhboxlayout>
#include <qtgui/qcheckbox>
#include <qtgui/qlabel>
#include <iostream>

// Define colour of plot curves
#define HR_COLOUR Qt::darkRed
#define ALT_COLOUR Qt::darkGreen
#define GRAD_COLOUR Qt::darkBlue
#define SPEED_COLOUR Qt::yellow

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
	_plot->enableAxis(QwtPlot::yRight,true);
	//_plot->setAxisScale(QwtPlot::yLeft, 0, 250);
	//_plot->setAxisScale(QwtPlot::xBottom, 0, 10000);
	_plot->setAxisAutoScale(QwtPlot::xBottom,true);

	QColor c;

	_curve_hr = new QwtPlotCurve("Heart Rate");
	c = HR_COLOUR;
	_curve_hr->setPen(c);
	_curve_hr->setYAxis(QwtPlot::yLeft);

	_curve_grad = new QwtPlotCurve("Gradient");
	c = GRAD_COLOUR;
	_curve_grad->setPen(c);
	_curve_grad->setYAxis(QwtPlot::yLeft);

	_curve_speed = new QwtPlotCurve("Speed");
	c = SPEED_COLOUR;
	_curve_speed->setPen(c);
	_curve_speed->setYAxis(QwtPlot::yLeft);

	_curve_alt = new QwtPlotCurve("Elevation");
	_curve_alt->setRenderHint(QwtPlotItem::RenderAntialiased);
	c = ALT_COLOUR;
	c.setAlpha(150);
	_curve_alt->setPen(c);
    _curve_alt->setBrush(c);
	_curve_alt->setYAxis(QwtPlot::yRight);

	_curve_alt->attach(_plot);
	_curve_speed->attach(_plot);
	_curve_grad->attach(_plot);
	_curve_hr->attach(_plot);

	QwtScaleWidget* speed_axis = new QwtScaleWidget(QwtScaleDraw::LeftScale);
	speed_axis->setTitle("m/s");
	QwtScaleDiv scale_div = _plot->axisScaleEngine(QwtPlot::yLeft)->divideScale(0.0,100.0,5,4);
	speed_axis->setScaleDiv(_plot->axisScaleEngine(QwtPlot::yLeft)->transformation(), scale_div);

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
	_plot_zoomer = new QwtPlotCustomZoomer(QwtPlot::xBottom, QwtPlot::yLeft, _plot->canvas());
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

	// Selection for x-axis measurement
	_x_axis_measurement = new QComboBox();
	_x_axis_measurement->insertItem(0,"Time");
	_x_axis_measurement->insertItem(1,"Distance");
	connect(_x_axis_measurement,SIGNAL(currentIndexChanged(int)), this, SLOT(xAxisUnitsChanged(int)));

	// Selection for graph plots
	QCheckBox* hr_cb = new QCheckBox("Heart Rate");
	QCheckBox* speed_cb = new QCheckBox("Speed");
	QCheckBox* alt_cb = new QCheckBox("Elevation");
	QCheckBox* grad_cb = new QCheckBox("Gradient");
	
	// Layout the GUI
	QPalette plt;
	plt.setColor(QPalette::WindowText, HR_COLOUR);
	hr_cb->setPalette(plt);
	plt.setColor(QPalette::WindowText, SPEED_COLOUR);
	speed_cb->setPalette(plt);
	plt.setColor(QPalette::WindowText, ALT_COLOUR);
	alt_cb->setPalette(plt);
	plt.setColor(QPalette::WindowText, GRAD_COLOUR);
	grad_cb->setPalette(plt);

	QLabel* x_axis_measurement_label = new QLabel("X Axis:");
	x_axis_measurement_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	x_axis_measurement_label->setIndent(0);
	
	QWidget* plot_options_widget = new QWidget;
	QHBoxLayout* hlayout1 = new QHBoxLayout(plot_options_widget);
	hlayout1->addWidget(x_axis_measurement_label);
	hlayout1->addWidget(_x_axis_measurement);
	hlayout1->addSpacing(20);
	hlayout1->addWidget(hr_cb);
	hlayout1->addWidget(speed_cb);
	hlayout1->addWidget(alt_cb);
	hlayout1->addWidget(grad_cb);

	QWidget* plot_widget = new QWidget;
	QHBoxLayout* hlayout2 = new QHBoxLayout(plot_widget);
	hlayout2->addWidget(speed_axis);
	hlayout2->addWidget(_plot);
	
	QVBoxLayout* vlayout = new QVBoxLayout(this);
	vlayout->addWidget(plot_widget);
	vlayout->addWidget(plot_options_widget);
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
	_plot_zoomer->setZoomBase();
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
