#include "totalswindow.h"
#include "user.h"
#include "logdirectorysummary.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>

#include <QDateTime.h>
#include <QIcon.h>
#include <QBoxLayout.h>
#include <QCheckBox.h>
#include <QComboBox.h>

#include <iostream>

#define TIME_COLOUR Qt::darkBlue
#define DIST_COLOUR Qt::darkRed

/******************************************************/
TotalsWindow::TotalsWindow(User* user):
QWidget()
{
	_user = user;

	setWindowTitle(tr("Totals"));
	setWindowIcon(QIcon("./resources/rideviewer_head128x128.ico")); 

	// Create the plot
	_plot = new QwtPlot();
	_plot->enableAxis(QwtPlot::yRight,true);

	// Set the curves
	QColor c;

	_histogram_yearly_time = new QwtPlotCurve("Yearly Time");
	_histogram_yearly_time->attach(_plot);
	_histogram_yearly_time->setYAxis(QwtPlot::yLeft);
	c = TIME_COLOUR;
	_histogram_yearly_time->setPen(c);

	_histogram_yearly_dist = new QwtPlotCurve("Yearly Dist");
	_histogram_yearly_dist->attach(_plot);
	_histogram_yearly_dist->setYAxis(QwtPlot::yRight);
	c = DIST_COLOUR;
	_histogram_yearly_dist->setPen(c);
	
	_histogram_monthly_time = new QwtPlotCurve("Monthly Time");
	_histogram_monthly_time->attach(_plot);
	_histogram_monthly_time->setYAxis(QwtPlot::yLeft);
	c = TIME_COLOUR;
	_histogram_monthly_time->setPen(c);

	_histogram_monthly_dist = new QwtPlotCurve("Monthly Dist");
	_histogram_monthly_dist->attach(_plot);
	_histogram_monthly_dist->setYAxis(QwtPlot::yRight);
	c = DIST_COLOUR;
	_histogram_monthly_dist->setPen(c);
	
	_histogram_weekly_time = new QwtPlotCurve("Weekly Time");
	_histogram_weekly_time->attach(_plot);
	_histogram_weekly_time->setYAxis(QwtPlot::yLeft);
	c = TIME_COLOUR;
	_histogram_weekly_time->setPen(c);

	_histogram_weekly_dist = new QwtPlotCurve("Weekly Dist");
	_histogram_weekly_dist->attach(_plot);
	_histogram_weekly_dist->setYAxis(QwtPlot::yRight);
	c = DIST_COLOUR;
	_histogram_weekly_dist->setPen(c);

	// Creat GUI widgets
	_dist_cb = new QCheckBox("Distance metric");
	_time_cb = new QCheckBox("Time metric");
	_time_group_selector = new QComboBox();
	_time_group_selector->addItem("Weekly");
	_time_group_selector->addItem("Monthly");
	_time_group_selector->addItem("Yearly");
	connect(_dist_cb, SIGNAL(stateChanged(int)),this,SLOT(updateCurves()));
	connect(_time_cb, SIGNAL(stateChanged(int)),this,SLOT(updateCurves()));
	connect(_dist_cb, SIGNAL(currentIndexChanged(int)),this,SLOT(updateCurves()));

	QWidget* controls = new QWidget;
	QHBoxLayout* hlayout = new QHBoxLayout(controls);
	hlayout->addSpacing(200);
	hlayout->addWidget(_dist_cb);
	hlayout->addWidget(_time_cb);
	hlayout->addWidget(_time_group_selector);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_plot);
	layout->addWidget(controls);
	setLayout(layout);

	resize(800,350);
	show();

	computeHistogramData();
	computePlotCurves();
	_plot->replot();
}
 
/******************************************************/
TotalsWindow::~TotalsWindow()
{

}

/******************************************************/
void TotalsWindow::computeHistogramData()
{
	// Read summary from file
	LogDirectorySummary log_dir_summary(_user->logDirectory());
	log_dir_summary.readFromFile();

	for (unsigned int i=0; i < log_dir_summary.numLogs(); ++i)
	{
		QString tmp = log_dir_summary.log(i)._date.split(' ')[0]; // split at the ' ' to get date only (no time)
		QDate date = QDate::fromString(tmp,Qt::ISODate);

		QMap<int, double>::iterator yearly_time_it = _yearly_time.find(date.year());
		QMap<int, double>::iterator yearly_dist_it = _yearly_dist.find(date.year());
		QMap<std::pair<int,int>, double>::iterator monthly_time_it = _monthly_time.find(std::make_pair(date.year(), date.month()));
		QMap<std::pair<int,int>, double>::iterator monthly_dist_it = _monthly_dist.find(std::make_pair(date.year(), date.month()));
		QMap<std::pair<int,int>, double>::iterator weekly_time_it = _weekly_time.find(std::make_pair(date.year(), date.weekNumber()));
		QMap<std::pair<int,int>, double>::iterator weekly_dist_it = _weekly_dist.find(std::make_pair(date.year(), date.weekNumber()));

		// Increment the yearly totals
		if (yearly_time_it != _yearly_time.end())
		{
			yearly_time_it.value() += log_dir_summary.log(i)._time;
			yearly_dist_it.value() += log_dir_summary.log(i)._dist;
		}
		else
		{
			_yearly_time.insert(date.year(), log_dir_summary.log(i)._time);
			_yearly_dist.insert(date.year(), log_dir_summary.log(i)._dist);
		}

		// Increment the monthly totals
		if (monthly_time_it != _monthly_time.end())
		{
			monthly_time_it.value() += log_dir_summary.log(i)._time;
			monthly_dist_it.value() += log_dir_summary.log(i)._dist;
		}
		else
		{
			_monthly_time.insert(std::make_pair(date.year(), date.month()), log_dir_summary.log(i)._time);
			_monthly_dist.insert(std::make_pair(date.year(), date.month()), log_dir_summary.log(i)._dist);
		}

		// Increment the weekly totals
		if (weekly_time_it != _weekly_time.end())
		{
			weekly_time_it.value() += log_dir_summary.log(i)._time;
			weekly_dist_it.value() += log_dir_summary.log(i)._dist;
		}
		else
		{
			_weekly_time.insert(std::make_pair(date.year(), date.weekNumber()), log_dir_summary.log(i)._time);
			_weekly_dist.insert(std::make_pair(date.year(), date.weekNumber()), log_dir_summary.log(i)._dist);
		}
	}

	//std::cout << "weeks " << _weekly_time.size() << std::endl;
	//std::cout << "months " << _monthly_time.size() << std::endl;
	//std::cout << "years " << _yearly_time.size() << std::endl;
}

/******************************************************/
void TotalsWindow::computePlotCurves()
{
	QVector<std::pair<int,int> > tmp = QVector<std::pair<int,int> >::fromList(_monthly_time.keys());
	QVector<double> time_data(tmp.size());
	for (int i=0; i < tmp.size(); ++i)
	{
		time_data[i] = tmp[i].second;
	}

	QVector<double> y_data1 = QVector<double>::fromList(_monthly_time.values());
	_histogram_monthly_time->setSamples(time_data, y_data1);

	QVector<double> y_data2 = QVector<double>::fromList(_monthly_dist.values());
	_histogram_monthly_dist->setSamples(time_data, y_data2);
}

/******************************************************/
void TotalsWindow::updateCurves()
{

}
