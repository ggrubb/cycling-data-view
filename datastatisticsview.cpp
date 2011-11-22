#include "datastatisticsview.h"
#include "dataprocessing.h"
#include "datalog.h"
#include "user.h"

#include <qtgui/qtablewidget>
#include <qtgui/qvboxlayout>
#include <qtgui/qlabel>
#include <iostream>
#include <cassert>

/******************************************************/
DataStatisticsView::DataStatisticsView()
{
	_table = new QTableWidget(19,2,this);
	_table->setSelectionMode(QAbstractItemView::NoSelection);
	_table->setAlternatingRowColors(true);

	QStringList column_headers, row_headers;
	row_headers 
		<< "Time - min" << "Distance - km" << "Elevation Gain - m" << "Elevation Loss - m"
		<< "Mean Speed - km/h" << "Mean Heart Rate - bpm" << "Mean Gradient - %" << "Mean Cadence - rpm" << "Mean Power - W"
		<< "Max Speed - km/h" << "Max Heart Rate - bpm" << "Max Gradient - %" << "Max Cadence - rpm" << "Max Power - W"
		<< "Time HR Zone1 - min" << "Time HR Zone2 - min" << "Time HR Zone3 - min" << "Time HR Zone4 - min" << "Time HR Zone5 - min";
	column_headers << " Overall " << "Selection";
	_table->setVerticalHeaderLabels(row_headers);
	_table->setHorizontalHeaderLabels(column_headers);
	for (int r = 0; r < _table->rowCount(); ++r)
	{
		_table->verticalHeaderItem(r)->setSizeHint(QSize(136,16));
		_table->setRowHeight(r,16);
	}

	_head_label = new QLabel;
	_head_label->setTextFormat(Qt::RichText);
	_head_label->setText("<b>Ride Statistics</b>");

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_head_label);
	layout->addWidget(_table);
	setLayout(layout);
	setFixedSize(270,365);
	
	clearTable();
}

/******************************************************/
DataStatisticsView::~DataStatisticsView()
{
	
}

/******************************************************/
void DataStatisticsView::displayRide(DataLog* data_log, User* user)
{
	_user = user;

	if (data_log != _data_log)
	{
		_data_log = data_log;

		_head_label->setText("<b>Ride Statistics: " + _data_log->date() + "</b>");
		displayCompleteRideStats();
		show();
	}
	else
	{
		clearSelectionColumn();
	}
}

/******************************************************/
void DataStatisticsView::displayCompleteRideStats()
{
	assert(_user);
	assert(_data_log);

	// Compute totals
	double time = _data_log->totalTime();
	double dist = _data_log->totalDist();
	double elev_gain = DataProcessing::computeGain(_data_log->altFltd().begin(), _data_log->altFltd().end());
	double elev_loss = DataProcessing::computeLoss(_data_log->altFltd().begin(), _data_log->altFltd().end());

	// Compute avgs
	double avg_speed = DataProcessing::computeAverage(_data_log->speedFltd().begin(), _data_log->speedFltd().end());
	double avg_hr = DataProcessing::computeAverage(_data_log->heartRateFltd().begin(), _data_log->heartRateFltd().end());
	double avg_grad = DataProcessing::computeAverage(_data_log->gradientFltd().begin(), _data_log->gradientFltd().end());
	double avg_cadence = DataProcessing::computeAverage(_data_log->cadenceFltd().begin(), _data_log->cadenceFltd().end());
	double avg_power = DataProcessing::computeAverage(_data_log->powerFltd().begin(), _data_log->powerFltd().end());
	
	// Compute maxs
	double max_speed = DataProcessing::computeMax(_data_log->speedFltd().begin(), _data_log->speedFltd().end());
	double max_hr = DataProcessing::computeMax(_data_log->heartRateFltd().begin(), _data_log->heartRateFltd().end());
	double max_gradient = DataProcessing::computeMax(_data_log->gradientFltd().begin(), _data_log->gradientFltd().end());
	double max_cadence = DataProcessing::computeMax(_data_log->cadenceFltd().begin(), _data_log->cadenceFltd().end());
	double max_power = DataProcessing::computeMax(_data_log->powerFltd().begin(), _data_log->powerFltd().end());
	
	// Compute HR zone times
	double hr_zone1 = DataProcessing::computeTimeInHRZone(
		_data_log->heartRate(), _data_log->time(), _user->zone1(), _user->zone2());
	double hr_zone2 = DataProcessing::computeTimeInHRZone(
		_data_log->heartRate(), _data_log->time(), _user->zone2(), _user->zone3());
	double hr_zone3 = DataProcessing::computeTimeInHRZone(
		_data_log->heartRate(), _data_log->time(), _user->zone3(), _user->zone4());
	double hr_zone4 = DataProcessing::computeTimeInHRZone(
		_data_log->heartRate(), _data_log->time(), _user->zone4(), _user->zone5());
	double hr_zone5 = DataProcessing::computeTimeInHRZone(
		_data_log->heartRate(), _data_log->time(), _user->zone5(), 1000.0);

	// Update the data log with these stats
	_data_log->avgSpeed() = avg_speed;
	_data_log->avgHeartRate() = avg_hr;
	_data_log->avgGradient() = avg_grad;
	_data_log->avgCadence() = avg_cadence;
	_data_log->avgPower() = avg_power;

	_data_log->maxSpeed() = max_speed;
	_data_log->maxHeartRate() = max_hr;
	_data_log->maxGradient() = max_gradient;
	_data_log->maxCadence() = max_cadence;
	_data_log->maxPower() = max_power;

	// Set totals column
	_table->item(0,0)->setText(DataProcessing::minsFromSecs(time));
	_table->item(1,0)->setText(DataProcessing::kmFromMeters(dist));
	_table->item(2,0)->setText(QString::number(elev_gain, 'f', 1));
	_table->item(3,0)->setText(QString::number(elev_loss, 'f', 1));

	_table->item(4,0)->setText(QString::number(avg_speed, 'f', 1));
	_table->item(5,0)->setText(QString::number(avg_hr, 'f', 1));
	_table->item(6,0)->setText(QString::number(avg_grad, 'f', 2));
	_table->item(7,0)->setText(QString::number(avg_cadence, 'f', 1));
	_table->item(8,0)->setText(QString::number(avg_power, 'f', 1));
	
	_table->item(9,0)->setText(QString::number(max_speed, 'f', 1));
	_table->item(10,0)->setText(QString::number(max_hr, 'f', 0));
	_table->item(11,0)->setText(QString::number(max_gradient, 'f', 2));
	_table->item(12,0)->setText(QString::number(max_cadence, 'f', 0));
	_table->item(13,0)->setText(QString::number(max_power, 'f', 2));

	_table->item(14,0)->setText(DataProcessing::minsFromSecs(hr_zone1));
	_table->item(15,0)->setText(DataProcessing::minsFromSecs(hr_zone2));
	_table->item(16,0)->setText(DataProcessing::minsFromSecs(hr_zone3));
	_table->item(17,0)->setText(DataProcessing::minsFromSecs(hr_zone4));
	_table->item(18,0)->setText(DataProcessing::minsFromSecs(hr_zone5));
}

/******************************************************/
void DataStatisticsView::clearTable()
{
	clearTotalsColumn();
	clearSelectionColumn();
	_table->resizeColumnsToContents();
}

/******************************************************/
void DataStatisticsView::clearTotalsColumn()
{
	for (int r = 0; r < _table->rowCount(); ++r)
	{
		QTableWidgetItem *item = new QTableWidgetItem("-");
		item->setTextAlignment(Qt::AlignCenter);
		_table->setItem(r,0,item);
	}
}

/******************************************************/
void DataStatisticsView::clearSelectionColumn()
{
	for (int r = 0; r < _table->rowCount(); ++r)
	{
		QTableWidgetItem *item = new QTableWidgetItem("-");
		item->setTextAlignment(Qt::AlignCenter);
		_table->setItem(r,1,item);
	}
}

/******************************************************/
void DataStatisticsView::deleteSelection()
{
	clearSelectionColumn();
}

/******************************************************/
void DataStatisticsView::displaySelectedRideStats(int idx_start, int idx_end)
{
	assert(_user);
	assert(_data_log);

	// Compute totals
	double time = _data_log->time(idx_end) - _data_log->time(idx_start);
	double dist = _data_log->dist(idx_end) - _data_log->dist(idx_start);
	double elev_gain = DataProcessing::computeGain(_data_log->altFltd().begin() + idx_start, _data_log->altFltd().begin() + idx_end);
	double elev_loss = DataProcessing::computeLoss(_data_log->altFltd().begin() + idx_start, _data_log->altFltd().begin() + idx_end);
	
	// Compute avgs
	double avg_speed = DataProcessing::computeAverage(_data_log->speedFltd().begin() + idx_start, _data_log->speedFltd().begin() + idx_end);
	double avg_hr = DataProcessing::computeAverage(_data_log->heartRateFltd().begin() + idx_start, _data_log->heartRateFltd().begin() + idx_end);
	double avg_grad = DataProcessing::computeAverage(_data_log->gradientFltd().begin() + idx_start, _data_log->gradientFltd().begin() + idx_end);
	double avg_cadence = DataProcessing::computeAverage(_data_log->cadenceFltd().begin() + idx_start, _data_log->cadenceFltd().begin() + idx_end);
	double avg_power = DataProcessing::computeAverage(_data_log->powerFltd().begin() + idx_start, _data_log->powerFltd().begin() + idx_end);
	
	// Compute maxs
	double max_speed = DataProcessing::computeMax(_data_log->speedFltd().begin() + idx_start, _data_log->speedFltd().begin() + idx_end);
	double max_hr = DataProcessing::computeMax(_data_log->heartRateFltd().begin() + idx_start, _data_log->heartRateFltd().begin() + idx_end);
	double max_gradient = DataProcessing::computeMax(_data_log->gradientFltd().begin() + idx_start, _data_log->gradientFltd().begin() + idx_end);
	double max_cadence = DataProcessing::computeMax(_data_log->cadenceFltd().begin() + idx_start, _data_log->cadenceFltd().begin() + idx_end);
	double max_power = DataProcessing::computeMax(_data_log->powerFltd().begin() + idx_start, _data_log->powerFltd().begin() + idx_end);
	
	// Compute HR zone times
	std::vector<double> sub_heart_rate(_data_log->heartRate().begin() + idx_start, _data_log->heartRate().begin() + idx_end);
	std::vector<double> sub_time(_data_log->time().begin() + idx_start, _data_log->time().begin() + idx_end);
	double hr_zone1 = DataProcessing::computeTimeInHRZone(sub_heart_rate, sub_time, _user->zone1(), _user->zone2());
	double hr_zone2 = DataProcessing::computeTimeInHRZone(sub_heart_rate, sub_time, _user->zone2(), _user->zone3());
	double hr_zone3 = DataProcessing::computeTimeInHRZone(sub_heart_rate, sub_time, _user->zone3(), _user->zone4());
	double hr_zone4 = DataProcessing::computeTimeInHRZone(sub_heart_rate, sub_time, _user->zone4(), _user->zone5());
	double hr_zone5 = DataProcessing::computeTimeInHRZone(sub_heart_rate, sub_time, _user->zone5(), 1000.0);

	// Set selection column
	_table->item(0,1)->setText(DataProcessing::minsFromSecs(time));
	_table->item(1,1)->setText(DataProcessing::kmFromMeters(dist));
	_table->item(2,1)->setText(QString::number(elev_gain, 'f', 1));
	_table->item(3,1)->setText(QString::number(elev_loss, 'f', 1));

	_table->item(4,1)->setText(QString::number(avg_speed, 'f', 1));
	_table->item(5,1)->setText(QString::number(avg_hr, 'f', 1));
	_table->item(6,1)->setText(QString::number(avg_grad, 'f', 2));
	_table->item(7,1)->setText(QString::number(avg_cadence, 'f', 1));
	_table->item(8,1)->setText(QString::number(avg_power, 'f', 1));

	_table->item(9,1)->setText(QString::number(max_speed, 'f', 1));
	_table->item(10,1)->setText(QString::number(max_hr, 'f', 0));
	_table->item(11,1)->setText(QString::number(max_gradient, 'f', 2));
	_table->item(12,1)->setText(QString::number(max_cadence, 'f', 0));
	_table->item(13,1)->setText(QString::number(max_power, 'f', 2));

	_table->item(14,1)->setText(DataProcessing::minsFromSecs(hr_zone1));
	_table->item(15,1)->setText(DataProcessing::minsFromSecs(hr_zone2));
	_table->item(16,1)->setText(DataProcessing::minsFromSecs(hr_zone3));
	_table->item(17,1)->setText(DataProcessing::minsFromSecs(hr_zone4));
	_table->item(18,1)->setText(DataProcessing::minsFromSecs(hr_zone5));
}