#ifndef TOTALSWINDOW_H
#define TOTALSWINDOW_H
 
#include <Qwidget.h>
#include <QMap.h>

class User;
class QwtPlotCurve;
class QwtPlot;
class QCheckBox;
class QComboBox;

class TotalsWindow : public QWidget
{
	Q_OBJECT
public:
	TotalsWindow(User* user);
	~TotalsWindow();

private slots:
	void updatePlot();

private:
	void computeHistogramData();
	void computeCurves();

	User* _user;

	QwtPlotCurve* _histogram_yearly_time;
	QwtPlotCurve* _histogram_yearly_dist;
	QwtPlotCurve* _histogram_monthly_time;
	QwtPlotCurve* _histogram_monthly_dist;
	QwtPlotCurve* _histogram_weekly_time;
	QwtPlotCurve* _histogram_weekly_dist;

	QwtPlot* _plot;

	QMap<int, double> _yearly_time; // year as int
	QMap<int, double> _yearly_dist; // year as int
	QMap<std::pair<int,int>, double> _monthly_time; // month as pair<year,month>
	QMap<std::pair<int,int>, double> _monthly_dist; // month as pair<year,month>
	QMap<std::pair<int,int>, double> _weekly_time; // week as pair<year, week num>
	QMap<std::pair<int,int>, double> _weekly_dist; // week as pair<year, week num>

	QCheckBox* _dist_cb;
	QCheckBox* _time_cb;
	QComboBox* _time_group_selector;
};
 
#endif // TOTALSWINDOW_H