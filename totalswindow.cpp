#include "totalswindow.h"
#include "user.h"
#include "logdirectorysummary.h"
#include "barchartitem.h"
#include "dateselectorwidget.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>
#include <qwt_scale_draw.h>

#include <QDateTime.h>
#include <QIcon.h>
#include <QBoxLayout.h>
#include <QCheckBox.h>
#include <QComboBox.h>

#include <iostream>

#define TIME_COLOUR Qt::darkBlue
#define DIST_COLOUR Qt::darkRed


/******************************************************/
class DateScaleDraw: public QwtScaleDraw
{
public:
	DateScaleDraw(QString& fmt):
	  _format(fmt)
	  {}
 
    virtual QwtText label(double v) const
    {
		QDateTime t = QDateTime::fromTime_t((int)v);
		if (_format.compare("weeks")==0)
		{
			int year = t.date().year();
			int week = t.date().weekNumber();
			return QString::number(year) + " wk" + QString::number(week);
		}
		else if (_format.compare("months")==0)
		{
			int year = t.date().year();
			int month = t.date().month();
			return QString::number(year) + "/" + QString::number(month);
		}
		else if (_format.compare("years")==0)
		{
			return t.toString("yyyy");
		}
		else
			return QString("error");
    }

private:
	const QString _format;
};

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

	QwtText axis_text;
	QFont font =  _plot->axisFont(QwtPlot::xBottom);
	font.setPointSize(8);
	axis_text.setFont(font);

	axis_text.setText("Time (hours)");
	_plot->setAxisTitle(QwtPlot::yLeft,axis_text);
	axis_text.setText("Distance (km)");
	_plot->setAxisTitle(QwtPlot::yRight,axis_text);
	
	_hist_monthly_dist = new BarChartItem();
	_hist_monthly_dist->setColour(DIST_COLOUR);
	_hist_monthly_dist->setTimeWidth(3600*24*7); // a week in secs
	_hist_monthly_dist->setYAxis(QwtPlot::yRight);
	_hist_monthly_dist->attach(_plot);
	
	_hist_monthly_time = new BarChartItem();
	_hist_monthly_time->setColour(TIME_COLOUR);
	_hist_monthly_time->setTimeWidth(3600*24*7); // a week in secs
	_hist_monthly_time->setTimeOffset(3600*24*7); // a week in secs
	_hist_monthly_time->setYAxis(QwtPlot::yLeft);
	_hist_monthly_time->attach(_plot);

	_hist_weekly_dist = new BarChartItem();
	_hist_weekly_dist->setColour(DIST_COLOUR);
	_hist_weekly_dist->setTimeWidth(3600*24*2); // two days in secs
	_hist_weekly_dist->setYAxis(QwtPlot::yRight);
	_hist_weekly_dist->attach(_plot);
	
	_hist_weekly_time = new BarChartItem();
	_hist_weekly_time->setColour(TIME_COLOUR);
	_hist_weekly_time->setTimeWidth(3600*24*2); // two days in secs
	_hist_weekly_time->setTimeOffset(3600*24*2); // two days in secs
	_hist_weekly_time->setYAxis(QwtPlot::yLeft);
	_hist_weekly_time->attach(_plot);

	_hist_yearly_dist = new BarChartItem();
	_hist_yearly_dist->setColour(DIST_COLOUR);
	_hist_yearly_dist->setTimeWidth(3600*24*7*15); // 15 weeks in secs
	_hist_yearly_dist->setYAxis(QwtPlot::yRight);
	_hist_yearly_dist->attach(_plot);
	
	_hist_yearly_time = new BarChartItem();
	_hist_yearly_time->setColour(TIME_COLOUR);
	_hist_yearly_time->setTimeWidth(3600*24*7*15); // 15 weeks in secs
	_hist_yearly_time->setTimeOffset(3600*24*7*15); // 15 weeks in secs
	_hist_yearly_time->setYAxis(QwtPlot::yLeft);
	_hist_yearly_time->attach(_plot);

	// Creat GUI widgets
	_dist_cb = new QCheckBox("Distance metric");
	_time_cb = new QCheckBox("Time metric");
	_time_group_selector = new QComboBox();
	_time_group_selector->addItem("Weekly");
	_time_group_selector->addItem("Monthly");
	_time_group_selector->addItem("Yearly");
	_dist_cb->setChecked(true);
	_time_cb->setChecked(true);

	QPalette plt;
	plt.setColor(QPalette::WindowText, TIME_COLOUR);
	_time_cb->setPalette(plt);
	plt.setColor(QPalette::WindowText, DIST_COLOUR);
	_dist_cb->setPalette(plt);

	LogDirectorySummary log_dir_summary(_user->logDirectory());
	log_dir_summary.readFromFile();
	_date_selector_widget = new DateSelectorWidget();
	_date_selector_widget->setRangeDates(log_dir_summary.firstLog().date(),log_dir_summary.lastLog().date());

	connect(_dist_cb, SIGNAL(stateChanged(int)),this,SLOT(updatePlot()));
	connect(_time_cb, SIGNAL(stateChanged(int)),this,SLOT(updatePlot()));
	connect(_time_group_selector, SIGNAL(currentIndexChanged(int)),this,SLOT(updatePlot()));
	connect(_date_selector_widget, SIGNAL(datesChanged()),this,SLOT(recomputePlotData()));

	// Layout the GUI
	QWidget* metric_ckboxs = new QWidget;
	QVBoxLayout* vlayout = new QVBoxLayout(metric_ckboxs);
	vlayout->addWidget(_dist_cb);
	vlayout->addWidget(_time_cb);

	QWidget* controls = new QWidget;
	QHBoxLayout* hlayout = new QHBoxLayout(controls);
	hlayout->addWidget(_date_selector_widget);
	hlayout->addWidget(metric_ckboxs);
	hlayout->addWidget(_time_group_selector);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_plot);
	layout->addWidget(controls);
	setLayout(layout);

	resize(800,400);
	show();

	recomputePlotData();
}
 
/******************************************************/
TotalsWindow::~TotalsWindow()
{

}

/******************************************************/
void TotalsWindow::recomputePlotData()
{
	_yearly_time.clear();
	_yearly_dist.clear();
	_monthly_time.clear();
	_monthly_dist.clear();
	_weekly_time.clear();
	_weekly_dist.clear();

	computeHistogramData();
	computeCurves();
	updatePlot();
}

/******************************************************/
void TotalsWindow::computeHistogramData()
{
	// Read summary from file
	LogDirectorySummary log_dir_summary(_user->logDirectory());
	log_dir_summary.readFromFile();

	for (int i=0; i < log_dir_summary.numLogs(); ++i)
	{
		QDate date = log_dir_summary.log(i).date();

		// Check that the date is within the user selected range
		if (date >= _date_selector_widget->minDate() && date <= _date_selector_widget->maxDate())
		{
			QMap<int, double>::iterator yearly_time_it = _yearly_time.find(date.year());
			QMap<int, double>::iterator yearly_dist_it = _yearly_dist.find(date.year());
			QMap<std::pair<int,int>, double>::iterator monthly_time_it = _monthly_time.find(std::make_pair(date.year(), date.month()));
			QMap<std::pair<int,int>, double>::iterator monthly_dist_it = _monthly_dist.find(std::make_pair(date.year(), date.month()));
			QMap<std::pair<int,int>, double>::iterator weekly_time_it = _weekly_time.find(std::make_pair(date.year(), date.weekNumber()));
			QMap<std::pair<int,int>, double>::iterator weekly_dist_it = _weekly_dist.find(std::make_pair(date.year(), date.weekNumber()));

			// Increment the yearly totals
			if (yearly_time_it != _yearly_time.end())
			{
				yearly_time_it.value() += log_dir_summary.log(i)._time/3600.0; //hours
				yearly_dist_it.value() += log_dir_summary.log(i)._dist/1000.0; //kms
			}
			else
			{
				_yearly_time.insert(date.year(), log_dir_summary.log(i)._time/3600.0);
				_yearly_dist.insert(date.year(), log_dir_summary.log(i)._dist/1000.0);
			}

			// Increment the monthly totals
			if (monthly_time_it != _monthly_time.end())
			{
				monthly_time_it.value() += log_dir_summary.log(i)._time/3600.0; //hours
				monthly_dist_it.value() += log_dir_summary.log(i)._dist/1000.0; //kms
			}
			else
			{
				_monthly_time.insert(std::make_pair(date.year(), date.month()), log_dir_summary.log(i)._time/3600.0);
				_monthly_dist.insert(std::make_pair(date.year(), date.month()), log_dir_summary.log(i)._dist/1000.0);
			}

			// Increment the weekly totals
			if (weekly_time_it != _weekly_time.end())
			{
				weekly_time_it.value() += log_dir_summary.log(i)._time/3600.0; //hours
				weekly_dist_it.value() += log_dir_summary.log(i)._dist/1000.0; //kms
			}
			else
			{
				_weekly_time.insert(std::make_pair(date.year(), date.weekNumber()), log_dir_summary.log(i)._time/3600.0);
				_weekly_dist.insert(std::make_pair(date.year(), date.weekNumber()), log_dir_summary.log(i)._dist/1000.0);
			}
		} // if 
	} // for
}

/******************************************************/
void TotalsWindow::computeCurves()
{
	// Weeks
	QVector<std::pair<int,int> > tmp1 = QVector<std::pair<int,int> >::fromList(_weekly_time.keys());
	QVector<double> x_data_weeks(tmp1.size());
	for (int i=0; i < tmp1.size(); ++i)
	{
		QDate date(tmp1[i].first,1,1);
		date = date.addDays(tmp1[i].second * 7);
		QDateTime date_time(date);
		x_data_weeks[i] = date_time.toTime_t();
	}

	QList< QPair<int, int> > dist_bar_heights;
	QList< QPair<int, int> > time_bar_heights;
	QVector<double> y_data_time_weeks = QVector<double>::fromList(_weekly_time.values());
	QVector<double> y_data_dist_weeks = QVector<double>::fromList(_weekly_dist.values());
	for (int i=0; i < y_data_dist_weeks.size(); ++i)
	{
		dist_bar_heights.append(QPair<int, int>(y_data_dist_weeks.at(i), x_data_weeks.at(i) ));
		time_bar_heights.append(QPair<int, int>(y_data_time_weeks.at(i), x_data_weeks.at(i) ));
	}
	_hist_weekly_dist->setData(dist_bar_heights);
	_hist_weekly_time->setData(time_bar_heights);

	// Months
	QVector<std::pair<int,int> > tmp2 = QVector<std::pair<int,int> >::fromList(_monthly_time.keys());
	QVector<double> x_data_months(tmp2.size());
	for (int i=0; i < tmp2.size(); ++i)
	{
		QDateTime date_time(QDate(tmp2[i].first, tmp2[i].second, 15));
		x_data_months[i] = date_time.toTime_t();
	}

	time_bar_heights.clear();
	dist_bar_heights.clear();
	QVector<double> y_data_time_months = QVector<double>::fromList(_monthly_time.values());
	QVector<double> y_data_dist_months = QVector<double>::fromList(_monthly_dist.values());
	for (int i=0; i < y_data_dist_months.size(); ++i)
	{
		dist_bar_heights.append(QPair<int, int>(y_data_dist_months.at(i), x_data_months.at(i) ));
		time_bar_heights.append(QPair<int, int>(y_data_time_months.at(i), x_data_months.at(i) ));
	}
	_hist_monthly_dist->setData(dist_bar_heights);
	_hist_monthly_time->setData(time_bar_heights);
	
	// Years
	QVector<int> tmp3 = QVector<int>::fromList(_yearly_time.keys());
	QVector<double> x_data_years(tmp3.size());
	for (int i=0; i < tmp3.size(); ++i)
	{
		QDateTime date_time(QDate(tmp3[i], 6, 15));
		x_data_years[i] = date_time.toTime_t();
	}

	time_bar_heights.clear();
	dist_bar_heights.clear();
	QVector<double> y_data_time_years = QVector<double>::fromList(_yearly_time.values());
	QVector<double> y_data_dist_years = QVector<double>::fromList(_yearly_dist.values());
	for (int i=0; i < y_data_dist_years.size(); ++i)
	{
		dist_bar_heights.append(QPair<int, int>(y_data_dist_years.at(i), x_data_years.at(i) ));
		time_bar_heights.append(QPair<int, int>(y_data_time_years.at(i), x_data_years.at(i) ));
	}
	_hist_yearly_dist->setData(dist_bar_heights);
	_hist_yearly_time->setData(time_bar_heights);
}

/******************************************************/
void TotalsWindow::updatePlot()
{
	switch (_time_group_selector->currentIndex()) 
	{
	case 0: // weekly
		_plot->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(tr("weeks")));
		
		// Update x-axis style	
		_plot->setAxisLabelRotation(QwtPlot::xBottom, -90.0);
		_plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
		_plot->setAxisMaxMajor(QwtPlot::xBottom,_hist_weekly_time->numElements()+2);	
		_plot->setAxisMaxMinor(QwtPlot::xBottom,2);

		if (_time_cb->isChecked())
			_hist_weekly_time->show();
		else
			_hist_weekly_time->hide();
			
		if (_dist_cb->isChecked())
			_hist_weekly_dist->show();
		else
			_hist_weekly_dist->hide();

		_hist_monthly_time->hide();
		_hist_monthly_dist->hide();
		_hist_yearly_time->hide();
		_hist_yearly_dist->hide();

		_hist_weekly_dist->scalePlotAxis();
		_hist_weekly_time->scalePlotAxis();
		break;
	case 1: // monthly

		_plot->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(tr("months")));
		
		// Update x-axis style	
		_plot->setAxisLabelRotation(QwtPlot::xBottom, -90.0);
		_plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
		_plot->setAxisMaxMajor(QwtPlot::xBottom,_hist_monthly_time->numElements()+2);	
		_plot->setAxisMaxMinor(QwtPlot::xBottom,2);

		_hist_weekly_time->hide();
		_hist_weekly_dist->hide();
		if (_time_cb->isChecked())
			_hist_monthly_time->show();
		else
			_hist_monthly_time->hide();

		if (_dist_cb->isChecked())
			_hist_monthly_dist->show();
		else
			_hist_monthly_dist->hide();

		_hist_yearly_time->hide();
		_hist_yearly_dist->hide();
		
		_hist_monthly_dist->scalePlotAxis();
		_hist_monthly_time->scalePlotAxis();
		break;

	case 2: // yearly
		_plot->setAxisScaleDraw(QwtPlot::xBottom, new DateScaleDraw(tr("years")));
		
		// Update x-axis style	
		_plot->setAxisLabelRotation(QwtPlot::xBottom, -90.0);
		_plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);
		_plot->setAxisMaxMajor(QwtPlot::xBottom,_hist_yearly_time->numElements()+2);	
		_plot->setAxisMaxMinor(QwtPlot::xBottom,2);

		_hist_weekly_time->hide();
		_hist_weekly_dist->hide();
		_hist_monthly_time->hide();
		_hist_monthly_dist->hide();
		if (_time_cb->isChecked())
			_hist_yearly_time->show();
		else
			_hist_yearly_time->hide();

		if (_dist_cb->isChecked())
			_hist_yearly_dist->show();
		else
			_hist_yearly_dist->hide();

		_hist_yearly_dist->scalePlotAxis();
		_hist_yearly_time->scalePlotAxis();
		break;
	}

	_plot->replot();
	
}
