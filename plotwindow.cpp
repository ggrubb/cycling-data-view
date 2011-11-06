#include "plotwindow.h"
#include "datalog.h"
#include "googlemap.h"
#include "datastatisticsview.h"
#include "dataprocessing.h"
#include "qwtcustomplotpicker.h"
#include "qwtcustomplotzoomer.h"

#include <qwt_plot_picker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_picker_machine.h>
#include <qwt_painter.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_text.h>
#include <qwt_plot_marker.h>

#include <qtgui/qcombobox>
#include <qtgui/qvboxlayout>
#include <qtgui/qhboxlayout>
#include <qtgui/qcheckbox>
#include <qtgui/qlabel>
#include <qtgui/qradiobutton>
#include <qtgui/qbuttongroup>
#include <qtgui/qslider>

#include <iostream>
#include <sstream>

// Define colour of plot curves
#define HR_COLOUR Qt::darkGreen // old = Qt::darkRed
#define ALT_COLOUR QColor(170,150,95) // old = QColor(135,180,105)
#define CADENCE_COLOUR Qt::darkBlue
#define SPEED_COLOUR Qt::yellow

/******************************************************/
QwtCustomPlotZoomer::QwtCustomPlotZoomer(int x_axis, int y_axis, QwtPlotCanvas* canvas, bool do_replot):
	QwtPlotZoomer(x_axis,y_axis,canvas,do_replot)
{}

/******************************************************/
bool QwtCustomPlotZoomer::accept(QPolygon& p) const
{
	if ( p.count() < 2 )
		return false;

	// Reject zooms which are too small
	if (abs(p[0].x() - p[1].x()) < 10)
		return false;

	// Set the zoom rect to be top to bottm, irrespective of what the user selects in y axis
	p[0].setY(0);
	p[1].setY(200);
	return true;
}

/******************************************************/
void QwtCustomPlotZoomer::drawRubberBand(QPainter* painter) const
{
	if ( rubberBand() < UserRubberBand )
		QwtPlotPicker::drawRubberBand( painter );
	else
	{
		if ( !isActive() || rubberBandPen().style() == Qt::NoPen )
			return;

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

/******************************************************/
QwtCustomPlotPicker::QwtCustomPlotPicker(int x_axis, int y_axis, DataLog* data_log, QwtPlotCanvas* canvas):
	QwtPlotPicker(x_axis,y_axis,QwtPlotPicker::UserRubberBand, QwtPicker::AlwaysOn, canvas),
	_data_log(data_log),
	_x_axis_units(DistAxis)
{}

/******************************************************/
void QwtCustomPlotPicker::setDataLog(
	DataLog* data_log)
{
	_data_log = data_log;
}

/******************************************************/
void QwtCustomPlotPicker::drawTracker(QPainter* painter) const
{
	return;
}

/******************************************************/
void QwtCustomPlotPicker::drawRubberBand(QPainter* painter) const
{
	if ( rubberBand() < UserRubberBand )
		QwtPlotPicker::drawRubberBand( painter );
	else
	{
		if ( !isActive() || rubberBandPen().style() == Qt::NoPen )
			return;

		QPolygon p = selection();

		if ( p.count() != 1 )
			return;

		const QPoint& pt1 = p[0];
		const double x_val = plot()->invTransform(QwtPlot::xBottom,pt1.x()); // determine x value (time or dist)
		
		// Draw vertical line where curser is
		const int end = 250;
		painter->drawLine(pt1.x(), 0, pt1.x(), end);

		// Set pen and fond for all coming text
		painter->setPen(QColor(Qt::black));
		painter->setFont(QFont("Helvetica", 8, QFont::Bold));
		
		// Compute the index based on the x value
		int idx;
		if (_x_axis_units == TimeAxis)
		{
			painter->drawText(QPoint(pt1.x()+5, 10), "time: " + QString::number(x_val,'g',4));
			idx = _data_log->indexFromTime(x_val);
		}
		else if (_x_axis_units == DistAxis)
		{
			painter->drawText(QPoint(pt1.x()+5, 10), "dist: " + QString::number(x_val,'g',5));
			idx = _data_log->indexFromDist(x_val);
		}
		
		// Using the index, determine the curve values
		const double hr = _data_log->heartRateFltd(idx);
		const QPoint pt1_hr(pt1.x(),plot()->transform(QwtPlot::yLeft,hr));
		const double speed = _data_log->speedFltd(idx);
		const QPoint pt1_speed(pt1.x(),plot()->transform(QwtPlot::yLeft,speed));
		const double alt = _data_log->altFltd(idx);
		const QPoint pt1_alt(pt1.x(),plot()->transform(QwtPlot::yRight,alt));
		const double cadence = _data_log->cadenceFltd(idx);
		const QPoint pt1_cadence(pt1.x(),plot()->transform(QwtPlot::yLeft,cadence));

		// Draw highlights on all curves
		const QPoint offset(8,-5);
		const QwtPlotItemList item_list = plot()->itemList(QwtPlotCurve::Rtti_PlotCurve);
		for (int i = 0; i < item_list.size(); ++i) 
		{
			if (item_list.at(i)->title().text() == "Heart Rate" && item_list.at(i)->isVisible())
			{
				painter->drawLine(pt1_hr.x(), pt1_hr.y(), pt1_hr.x()+6, pt1_hr.y());
				painter->drawText(pt1_hr + offset, QString::number(hr,'g',3));
			}
			if (item_list.at(i)->title().text() == "Speed" && item_list.at(i)->isVisible())
			{
				painter->drawLine(pt1_speed.x(), pt1_speed.y(), pt1_speed.x()+6, pt1_speed.y());
				painter->drawText(pt1_speed + offset, QString::number(speed,'g',3));
			}
			if (item_list.at(i)->title().text() == "Elevation" && item_list.at(i)->isVisible())
			{
				painter->drawLine(pt1_alt.x(), pt1_alt.y(), pt1_alt.x()+6, pt1_alt.y());
				painter->drawText(pt1_alt + offset, QString::number(alt,'g',4));
			}
			if (item_list.at(i)->title().text() == "Cadence" && item_list.at(i)->isVisible())
			{
				painter->drawLine(pt1_cadence.x(), pt1_cadence.y(), pt1_cadence.x()+6, pt1_cadence.y());
				painter->drawText(pt1_cadence + offset, QString::number(cadence,'g',3));
			}
		}
	}
}

/******************************************************/
void QwtCustomPlotPicker::xAxisUnitsChanged(int units)
{
	_x_axis_units = (AxisUnits)units;
}

/******************************************************/
PlotWindow::PlotWindow(GoogleMap* google_map, DataStatisticsView* stats_view)
{
	// Create the plot
	_plot = new QwtPlot();
	
	// Connect this window to the google map
	connect(this, SIGNAL(setMarkerPosition(int)), google_map, SLOT(setMarkerPosition(int)));
	connect(this, SIGNAL(beginSelection(int)), google_map, SLOT(beginSelection(int)));
	connect(this, SIGNAL(endSelection(int)), google_map, SLOT(endSelection(int)));
	connect(this, SIGNAL(zoomSelection(int,int)), google_map, SLOT(zoomSelection(int,int)));
	connect(this, SIGNAL(deleteSelection()), google_map, SLOT(deleteSelection()));
	connect(this, SIGNAL(panSelection(int)), google_map, SLOT(moveSelection(int)));
	connect(this, SIGNAL(panAndHoldSelection(int)), google_map, SLOT(moveAndHoldSelection(int)));
	connect(this, SIGNAL(updateDataView()), google_map, SLOT(definePathColour()));

	// Connect this window to the statistical viewer
	connect(this, SIGNAL(zoomSelection(int,int)), stats_view, SLOT(displaySelectedRideStats(int,int)));
	connect(this, SIGNAL(deleteSelection()), stats_view, SLOT(deleteSelection()));
	connect(this, SIGNAL(updateDataView()), stats_view, SLOT(displayCompleteRideStats()));
	
	// Setup the axis
	_plot->enableAxis(QwtPlot::yRight,true);
	_plot->setAxisAutoScale(QwtPlot::xBottom,true);

	QwtText axis_text;
	QFont font =  _plot->axisFont(QwtPlot::xBottom);
	font.setPointSize(8);
	axis_text.setFont(font);

	axis_text.setText("HR (bpm) Speed (km/h) Cadence (rpm)");
	_plot->setAxisTitle(QwtPlot::yLeft,axis_text);

	axis_text.setText("Elevation (m)");
	_plot->setAxisTitle(QwtPlot::yRight,axis_text);

	axis_text.setText("Distance (m)");
	_plot->setAxisTitle(QwtPlot::xBottom,axis_text);

	// Define the curves to plot
	QColor c;

	_curve_hr = new QwtPlotCurve("Heart Rate");
	c = HR_COLOUR;
	_curve_hr->setPen(c);
	_curve_hr->setYAxis(QwtPlot::yLeft);

	_curve_cadence = new QwtPlotCurve("Cadence");
	c = CADENCE_COLOUR;
	_curve_cadence->setPen(c);
	_curve_cadence->setYAxis(QwtPlot::yLeft);

	_curve_speed = new QwtPlotCurve("Speed");
	c = SPEED_COLOUR;
	_curve_speed->setPen(c);
	_curve_speed->setYAxis(QwtPlot::yLeft);

	_curve_alt = new QwtPlotCurve("Elevation");
	_curve_alt->setRenderHint(QwtPlotItem::RenderAntialiased);
	c = ALT_COLOUR;
	_curve_alt->setPen(c);
    _curve_alt->setBrush(c);
	_curve_alt->setYAxis(QwtPlot::yRight);
	_curve_alt->setBaseline(-100.0); // ensure display is correct even when -ve altitude

	_curve_alt->attach(_plot);
	_curve_speed->attach(_plot);
	_curve_cadence->attach(_plot);
	_curve_hr->attach(_plot);

	// Plot picker for numerical display
	_plot_picker1 = 
		new QwtCustomPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, _data_log, _plot->canvas());
	_plot_picker1->setRubberBandPen(QColor(Qt::white));
    _plot_picker1->setTrackerPen(QColor(Qt::black));
	_plot_picker1->setStateMachine(new QwtPickerTrackerMachine());
	connect(_plot_picker1, SIGNAL(moved(const QPointF&)), this, SLOT(setMarkerPosition(const QPointF&)));
	
	// Plot picker for drawing user selection
	_plot_picker2 = 
		new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOff, _plot->canvas());
	_plot_picker2->setStateMachine(new QwtPickerDragPointMachine());
	connect(_plot_picker2, SIGNAL(appended(const QPointF&)), this, SLOT(beginSelection(const QPointF&)));
	connect(_plot_picker2, SIGNAL(moved(const QPointF&)), this, SLOT(endSelection(const QPointF&)));

	// Plot zoomer
	_plot_zoomer = new QwtCustomPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, _plot->canvas());
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
	QWidget* axis_selection_widget = new QWidget;
	QVBoxLayout* button_group_layout = new QVBoxLayout(axis_selection_widget);
	_time_axis = new QRadioButton("Time");
	_dist_axis = new QRadioButton("Distance");
	_dist_axis->setChecked(true);
	button_group_layout->addWidget(_time_axis);
	button_group_layout->addWidget(_dist_axis);
	
	_x_axis_measurement = new QButtonGroup(axis_selection_widget);
	_x_axis_measurement->addButton(_time_axis, 0);
	_x_axis_measurement->addButton(_dist_axis, 1);
	connect(_x_axis_measurement,SIGNAL(buttonClicked(int)), this, SLOT(xAxisUnitsChanged(int)));
	connect(_x_axis_measurement,SIGNAL(buttonClicked(int)), _plot_picker1, SLOT(xAxisUnitsChanged(int)));

	// Checkboxes for graph plots
	_hr_cb = new QCheckBox("Heart Rate");
	_speed_cb = new QCheckBox("Speed");
	_alt_cb = new QCheckBox("Elevation");
	_cadence_cb = new QCheckBox("Cadence");
	_laps_cb = new QCheckBox("Laps");
	_hr_zones_cb = new QCheckBox("HR Zones");
	_hr_cb->setChecked(true);
	_speed_cb->setChecked(true);
	_alt_cb->setChecked(true);
	_cadence_cb->setChecked(true);
	_laps_cb->setChecked(true);
	_hr_zones_cb->setChecked(false);
	connect(_hr_cb, SIGNAL(stateChanged(int)),this,SLOT(curveSelectionChanged()));
	connect(_speed_cb, SIGNAL(stateChanged(int)),this,SLOT(curveSelectionChanged()));
	connect(_alt_cb, SIGNAL(stateChanged(int)),this,SLOT(curveSelectionChanged()));
	connect(_cadence_cb, SIGNAL(stateChanged(int)),this,SLOT(curveSelectionChanged()));
	connect(_laps_cb, SIGNAL(stateChanged(int)),this,SLOT(lapSelectionChanged()));
	connect(_hr_zones_cb, SIGNAL(stateChanged(int)),this,SLOT(hrZoneSelectionChanged()));

	// Slider for signal smoothing
	_smoothing_selection = new QSlider(Qt::Horizontal);
	_smoothing_selection->setRange(0,50);
	_smoothing_selection->setFixedSize(80,15);
	const int default_smoothing = 5;
	_smoothing_selection->setValue(default_smoothing);
	_smoothing_selection->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));
	_smoothing_label = new QLabel("Smoothing: " + QString::number(default_smoothing));
	QWidget* smoothing_widget = new QWidget;
	QVBoxLayout* vlayout2 = new QVBoxLayout(smoothing_widget);
	vlayout2->addWidget(_smoothing_label);
	vlayout2->addWidget(_smoothing_selection);
	connect(_smoothing_selection, SIGNAL(valueChanged(int)),this,SLOT(signalSmoothingChanged()));

	// Layout the GUI
	QPalette plt;
	plt.setColor(QPalette::WindowText, HR_COLOUR);
	_hr_cb->setPalette(plt);
	plt.setColor(QPalette::WindowText, SPEED_COLOUR);
	_speed_cb->setPalette(plt);
	plt.setColor(QPalette::WindowText, ALT_COLOUR);
	_alt_cb->setPalette(plt);
	plt.setColor(QPalette::WindowText, CADENCE_COLOUR);
	_cadence_cb->setPalette(plt);

	QWidget* plot_options_widget = new QWidget;
	QVBoxLayout* vlayout1 = new QVBoxLayout(plot_options_widget);
	vlayout1->addWidget(_hr_cb);
	vlayout1->addWidget(_speed_cb);
	vlayout1->addWidget(_alt_cb);
	vlayout1->addWidget(_cadence_cb);
	vlayout1->addWidget(axis_selection_widget);
	vlayout1->addWidget(smoothing_widget);
	vlayout1->addWidget(_laps_cb);
	vlayout1->addWidget(_hr_zones_cb);
	vlayout1->addStretch();

	QHBoxLayout* hlayout2 = new QHBoxLayout(this);
	hlayout2->addWidget(_plot);
	hlayout2->addWidget(plot_options_widget);
	
	resize(700,270);

	// Disable all controls until ride data is loaded
	setEnabled(false);
}

/******************************************************/
PlotWindow::~PlotWindow()
{}

/******************************************************/
void PlotWindow::setEnabled(bool enabled)
{
	_plot_picker1->setEnabled(enabled);
	_plot_picker2->setEnabled(enabled);
	_plot_zoomer->setEnabled(enabled);
	_plot_panner->setEnabled(enabled);
	_time_axis->setEnabled(enabled);
	_dist_axis->setEnabled(enabled);
	_smoothing_selection->setEnabled(enabled);
	_smoothing_label->setEnabled(enabled);
	_hr_cb->setEnabled(enabled);
	_speed_cb->setEnabled(enabled);
	_alt_cb->setEnabled(enabled);
	_cadence_cb->setEnabled(enabled);
	_laps_cb->setEnabled(enabled);
	_hr_zones_cb->setEnabled(enabled);
}

/******************************************************/
void PlotWindow::displayRide(DataLog* data_log)
{
	if (data_log != _data_log)
	{
		// Set the data
		_data_log = data_log;
		_plot_picker1->setDataLog(_data_log);

		// Show the data
		drawGraphs();

		// Display markers
		clearLapMarkers();
		if (_data_log->numLaps() > 1)
		{
			_laps_cb->setChecked(true);
			drawLapMarkers();
		}
		else
		{
			_laps_cb->setChecked(false);
		}

		// Enabled user interface
		setEnabled(true);

		show();
	}
	else
	{
		// Return to base zoom
		_plot_zoomer->zoom(_plot_zoomer->zoomBase());
	}
}

/******************************************************/
void PlotWindow::drawGraphs()
{
	filterCurveData();
	setCurveData();
	if (_x_axis_measurement->checkedId() == 0) // time
	{
		_plot->setAxisScale(QwtPlot::xBottom, 0, _data_log->totalTime());
	}
	else // distance
	{
		_plot->setAxisScale(QwtPlot::xBottom, 0, _data_log->totalDist());
	}
	_plot->replot();
	_plot_zoomer->setZoomBase();
}

/******************************************************/
void PlotWindow::setCurveData()
{
	if (_x_axis_measurement->checkedId() == 0) // time
	{
		_curve_hr->setRawSamples(&_data_log->time(0), &_data_log->heartRateFltd(0), _data_log->numPoints());
		_curve_speed->setRawSamples(&_data_log->time(0), &_data_log->speedFltd(0), _data_log->numPoints());
		_curve_cadence->setRawSamples(&_data_log->time(0), &_data_log->cadenceFltd(0), _data_log->numPoints());
		_curve_alt->setRawSamples(&_data_log->time(0), &_data_log->altFltd(0), _data_log->numPoints());
	}
	else // distance
	{
		_curve_hr->setRawSamples(&_data_log->dist(0), &_data_log->heartRateFltd(0), _data_log->numPoints());
		_curve_speed->setRawSamples(&_data_log->dist(0), &_data_log->speedFltd(0), _data_log->numPoints());
		_curve_cadence->setRawSamples(&_data_log->dist(0), &_data_log->cadenceFltd(0), _data_log->numPoints());
		_curve_alt->setRawSamples(&_data_log->dist(0), &_data_log->altFltd(0), _data_log->numPoints());
	}
}

/******************************************************/
void PlotWindow::drawLapMarkers()
{
	if (_data_log->numLaps() > 1)
	{
		for (int i=0; i < _data_log->numLaps(); ++i)
		{
			QwtPlotMarker* marker = new QwtPlotMarker;
			marker->setLineStyle(QwtPlotMarker::VLine);
			marker->setLinePen(QPen(Qt::DotLine));
			if (_x_axis_measurement->checkedId() == 0) // time
			{
				const double time = _data_log->time(_data_log->lap(i).second);
				marker->setXValue(time);
			}
			else // distance
			{
				const double dist = _data_log->dist(_data_log->lap(i).second);
				marker->setXValue(dist);
			}
			marker->attach(_plot);
			marker->show();
			_lap_markers.push_back(marker);
		}
	}
}

/******************************************************/
void PlotWindow::clearLapMarkers()
{
	for (unsigned int i=0; i < _lap_markers.size(); ++i)
	{
		_lap_markers[i]->detach();
		delete _lap_markers[i];
	}
	_lap_markers.resize(0);
}

/******************************************************/
void PlotWindow::displayLap(int lap_index)
{
	// Get laps details
	const std::pair<int, int> lap = _data_log->lap(lap_index);

	// Define zoom rect
	QRect zoom_rect;
	zoom_rect.setBottom(200);
	zoom_rect.setTop(0);
	if (_x_axis_measurement->checkedId() == 0) // time
	{
		zoom_rect.setLeft(_data_log->time(lap.first));
		zoom_rect.setRight(_data_log->time(lap.second));
	}
	else // dist
	{
		zoom_rect.setLeft(_data_log->dist(lap.first));
		zoom_rect.setRight(_data_log->dist(lap.second));
	}

	// Tell the plot zoomer to zoom on this rect
	_plot_zoomer->zoom(zoom_rect);
}

/******************************************************/
void PlotWindow::setMarkerPosition(const QPointF& point)
{
	if (_x_axis_measurement->checkedId() == 0) // time
		emit setMarkerPosition(_data_log->indexFromTime(point.x()));
	else // distance
		emit setMarkerPosition(_data_log->indexFromDist(point.x()));
}		

/******************************************************/
void PlotWindow::beginSelection(const QPointF& point)
{
	_plot_picker1->setEnabled(false);
	if (_x_axis_measurement->checkedId() == 0) // time
		emit beginSelection(_data_log->indexFromTime(point.x()));
	else // distance
		emit beginSelection(_data_log->indexFromDist(point.x()));
}

/******************************************************/
void PlotWindow::endSelection(const QPointF& point)
{
	if (_x_axis_measurement->checkedId() == 0) // time
		emit endSelection(_data_log->indexFromTime(point.x()));
	else // distance
		emit endSelection(_data_log->indexFromDist(point.x()));
}

/******************************************************/
void PlotWindow::zoomSelection(const QRectF& rect)
{
	_plot_picker1->setEnabled(true);

	if (_plot_zoomer->zoomRectIndex() == 0) // if fully zoomed out
	{
		emit deleteSelection();
	}
	else // regular zoom
	{
		if (_x_axis_measurement->checkedId() == 0) // time
			emit zoomSelection(_data_log->indexFromTime(rect.left()), _data_log->indexFromTime(rect.right()));
		else // distance
			emit zoomSelection(_data_log->indexFromDist(rect.left()), _data_log->indexFromDist(rect.right()));
	}
}

/******************************************************/
void PlotWindow::panSelection(int x, int y)
{
	if (_x_axis_measurement->checkedId() == 0) // time
	{
		emit panSelection(
		_data_log->indexFromTime(_plot->invTransform(QwtPlot::xBottom,x))-
		_data_log->indexFromTime(_plot->invTransform(QwtPlot::xBottom,0)));
	}
	else // distance
	{
		emit panSelection(
		_data_log->indexFromDist(_plot->invTransform(QwtPlot::xBottom,x))-
		_data_log->indexFromDist(_plot->invTransform(QwtPlot::xBottom,0)));
	}
}

/******************************************************/
void PlotWindow::panAndHoldSelection(int x, int y)
{
	if (_x_axis_measurement->checkedId() == 0) // time
	{
		emit panAndHoldSelection(
		_data_log->indexFromTime(_plot->invTransform(QwtPlot::xBottom,x))-
		_data_log->indexFromTime(_plot->invTransform(QwtPlot::xBottom,0)));
	}
	else // distance
	{
		emit panAndHoldSelection(
		_data_log->indexFromDist(_plot->invTransform(QwtPlot::xBottom,x))-
		_data_log->indexFromDist(_plot->invTransform(QwtPlot::xBottom,0)));
	}
}

/******************************************************/
void PlotWindow::xAxisUnitsChanged(int idx)
{
	if (idx == 0) // time
		_plot->setAxisTitle(QwtPlot::xBottom,"Time (s)");
	else // dist
		_plot->setAxisTitle(QwtPlot::xBottom,"Distance (m)");
	
	drawGraphs();
	emit deleteSelection();
}

/******************************************************/
void PlotWindow::curveSelectionChanged()
{
	if (_hr_cb->isChecked()) _curve_hr->show(); else _curve_hr->hide();
	if (_alt_cb->isChecked()) _curve_alt->show(); else _curve_alt->hide();
	if (_cadence_cb->isChecked()) _curve_cadence->show(); else _curve_cadence->hide();
	if (_speed_cb->isChecked()) _curve_speed->show(); else _curve_speed->hide();

	_plot->replot();
}

/******************************************************/
void PlotWindow::lapSelectionChanged()
{
	if (_laps_cb->isChecked())
		drawLapMarkers();
	else
		clearLapMarkers();
	_plot->replot();
}

/******************************************************/
void PlotWindow::hrZoneSelectionChanged()
{

}

/******************************************************/
void PlotWindow::signalSmoothingChanged()
{
	// Filter the data
	filterCurveData();

	// Update the plots
	setCurveData();
	_plot->replot();
	_smoothing_label->setText("Smoothing: " + QString::number(_smoothing_selection->value()));

	// Update other windows viewing this data
	emit updateDataView();
}

/******************************************************/
void PlotWindow::filterCurveData()
{
	DataProcessing::lowPassFilterSignal(_data_log->heartRate(),_data_log->heartRateFltd(),_smoothing_selection->value());
	DataProcessing::lowPassFilterSignal(_data_log->speed(),_data_log->speedFltd(),_smoothing_selection->value());
	DataProcessing::lowPassFilterSignal(_data_log->cadence(),_data_log->cadenceFltd(),_smoothing_selection->value());
	DataProcessing::lowPassFilterSignal(_data_log->alt(),_data_log->altFltd(),_smoothing_selection->value());
	DataProcessing::lowPassFilterSignal(_data_log->gradient(),_data_log->gradientFltd(),_smoothing_selection->value());

	_data_log->heartRateFltdValid() = true;
	_data_log->speedFltdValid() = true;
	_data_log->cadenceFltdValid() = true;
	_data_log->altFltdValid() = true;
	_data_log->gradientFltdValid() = true;
}
