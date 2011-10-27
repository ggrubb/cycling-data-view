#include "datastatisticsview.h"
#include "dataprocessing.h"
#include "datalog.h"

#include <qtgui/qtablewidget>
#include <qtgui/qvboxlayout>
#include <iostream>

/******************************************************/
DataStatisticsView::DataStatisticsView()
{
	_table = new QTableWidget(16,2,this);
	_table->setSelectionMode(QAbstractItemView::NoSelection);

	QStringList column_headers, row_headers;
	row_headers 
		<< "Time (min)" << "Distance (km)" << "Elevation Gain (m)" << "Elevation Loss (m)"  << " "
		<< "Avg Speed (km/h)" << "Avg Heart Rate (bpm)" << "Avg Gradient (%)" << "Avg Cadence (rpm)" << "Avg Power (W)" << " "
		<< "Max Speed (km/h)" << "Max Heart Rate (bpm)" << "Max Gradient (%)" << "Max Cadence (rpm)" << "Max Power (W)";
	column_headers << " Overall " << "Selection";
	_table->setVerticalHeaderLabels(row_headers);
	_table->setHorizontalHeaderLabels(column_headers);
	_table->resizeRowsToContents();
	_table->setRowHeight(4,2); // seperator row
	_table->setRowHeight(10,2); // seperator row

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_table);
	setLayout(layout);
	setMaximumSize(270,340);
	
	clearTable();
}

/******************************************************/
DataStatisticsView::~DataStatisticsView()
{
	
}

/******************************************************/
void DataStatisticsView::displayRide(DataLog* data_log)
{
	_data_log = data_log;

	displayCompleteRideStats();
	show();
}

/******************************************************/
void DataStatisticsView::displayLap(int lap_index)
{
	displaySelectedRideStats(_data_log->lap(lap_index).first,_data_log->lap(lap_index).second);
}

/******************************************************/
void DataStatisticsView::displayCompleteRideStats()
{
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

	_table->item(5,0)->setText(QString::number(avg_speed, 'f', 1));
	_table->item(6,0)->setText(QString::number(avg_hr, 'f', 1));
	_table->item(7,0)->setText(QString::number(avg_grad, 'f', 2));
	_table->item(8,0)->setText(QString::number(avg_cadence, 'f', 1));
	_table->item(9,0)->setText(QString::number(avg_power, 'f', 1));
	
	_table->item(11,0)->setText(QString::number(max_speed, 'f', 1));
	_table->item(12,0)->setText(QString::number(max_hr, 'f', 0));
	_table->item(13,0)->setText(QString::number(max_gradient, 'f', 2));
	_table->item(14,0)->setText(QString::number(max_cadence, 'f', 0));
	_table->item(15,0)->setText(QString::number(max_power, 'f', 2));
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
		if (r != 4 && r != 10)
		{
			QTableWidgetItem *item = new QTableWidgetItem("-");
			item->setTextAlignment(Qt::AlignCenter);
			_table->setItem(r,0,item);
		}
	}
}

/******************************************************/
void DataStatisticsView::clearSelectionColumn()
{
	for (int r = 0; r < _table->rowCount(); ++r)
	{
		if (r != 4 && r != 10)
		{
			QTableWidgetItem *item = new QTableWidgetItem("-");
			item->setTextAlignment(Qt::AlignCenter);
			_table->setItem(r,1,item);
		}
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
	
	// Set selection column
	_table->item(0,1)->setText(DataProcessing::minsFromSecs(time));
	_table->item(1,1)->setText(DataProcessing::kmFromMeters(dist));
	_table->item(2,1)->setText(QString::number(elev_gain, 'f', 1));
	_table->item(3,1)->setText(QString::number(elev_loss, 'f', 1));

	_table->item(5,1)->setText(QString::number(avg_speed, 'f', 1));
	_table->item(6,1)->setText(QString::number(avg_hr, 'f', 1));
	_table->item(7,1)->setText(QString::number(avg_grad, 'f', 2));
	_table->item(8,1)->setText(QString::number(avg_cadence, 'f', 1));
	_table->item(9,1)->setText(QString::number(avg_power, 'f', 1));

	_table->item(11,1)->setText(QString::number(max_speed, 'f', 1));
	_table->item(12,1)->setText(QString::number(max_hr, 'f', 0));
	_table->item(13,1)->setText(QString::number(max_gradient, 'f', 2));
	_table->item(14,1)->setText(QString::number(max_cadence, 'f', 0));
	_table->item(15,1)->setText(QString::number(max_power, 'f', 2));
}