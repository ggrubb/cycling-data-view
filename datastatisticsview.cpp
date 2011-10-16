#include "datastatisticsview.h"
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
		<< "Time (s)" << "Distance (km)" << "Elevation Gain (m)" << "Elevation Loss (m)"  << " "
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

	// Compute totals
	double time = _data_log->totalTime();
	double dist = _data_log->totalDist()/1000.0;
	double elev_gain = DataLog::computeGain(_data_log->alt().begin(), _data_log->alt().end());
	double elev_loss = DataLog::computeLoss(_data_log->alt().begin(), _data_log->alt().end());
	
	// Set totals column
	_table->item(0,0)->setText(QString::number(time, 'f', 0));
	_table->item(1,0)->setText(QString::number(dist, 'f', 2));
	_table->item(2,0)->setText(QString::number(elev_gain, 'f', 1));
	_table->item(3,0)->setText(QString::number(elev_loss, 'f', 1));

	_table->item(5,0)->setText(QString::number(_data_log->avgSpeed(), 'f', 1));
	_table->item(6,0)->setText(QString::number(_data_log->avgHeartRate(), 'f', 2));
	_table->item(7,0)->setText(QString::number(_data_log->avgGradient(), 'f', 2));
	_table->item(8,0)->setText(QString::number(_data_log->avgCadence(), 'f', 2));
	_table->item(9,0)->setText(QString::number(_data_log->avgPower(), 'f', 2));
	
	_table->item(11,0)->setText(QString::number(_data_log->maxSpeed(), 'f', 1));
	_table->item(12,0)->setText(QString::number(_data_log->maxHeartRate()));
	_table->item(13,0)->setText(QString::number(_data_log->maxGradient(), 'f', 2));
	_table->item(14,0)->setText(QString::number(_data_log->maxCadence()));
	_table->item(15,0)->setText(QString::number(_data_log->maxPower(), 'f', 2));

	show();
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
void DataStatisticsView::setSelection(int idx_start, int idx_end)
{
	// Compute stats
	double time = _data_log->time(idx_end) - _data_log->time(idx_start);
	double dist = _data_log->dist(idx_end)/1000.0 - _data_log->dist(idx_start)/1000.0;
	double avg_speed = DataLog::computeAverage(_data_log->speed().begin() + idx_start, _data_log->speed().begin() + idx_end);
	double avg_hr = DataLog::computeAverage(_data_log->heartRate().begin() + idx_start, _data_log->heartRate().begin() + idx_end);
	double avg_grad = DataLog::computeAverage(_data_log->gradient().begin() + idx_start, _data_log->gradient().begin() + idx_end);
	double avg_cadence = DataLog::computeAverage(_data_log->cadence().begin() + idx_start, _data_log->cadence().begin() + idx_end);
	double avg_power = DataLog::computeAverage(_data_log->power().begin() + idx_start, _data_log->power().begin() + idx_end);
	double elev_gain = DataLog::computeGain(_data_log->alt().begin() + idx_start, _data_log->alt().begin() + idx_end);
	double elev_loss = DataLog::computeLoss(_data_log->alt().begin() + idx_start, _data_log->alt().begin() + idx_end);
	double max_speed = DataLog::computeMax(_data_log->speed().begin() + idx_start, _data_log->speed().begin() + idx_end);
	double max_hr = DataLog::computeMax(_data_log->heartRate().begin() + idx_start, _data_log->heartRate().begin() + idx_end);
	double max_gradient = DataLog::computeMax(_data_log->gradient().begin() + idx_start, _data_log->gradient().begin() + idx_end);
	double max_cadence = DataLog::computeMax(_data_log->cadence().begin() + idx_start, _data_log->cadence().begin() + idx_end);
	double max_power = DataLog::computeMax(_data_log->power().begin() + idx_start, _data_log->power().begin() + idx_end);
	
	// Set selection column
	_table->item(0,1)->setText(QString::number(time, 'f', 0));
	_table->item(1,1)->setText(QString::number(dist, 'f', 2));
	_table->item(2,1)->setText(QString::number(elev_gain, 'f', 1));
	_table->item(3,1)->setText(QString::number(elev_loss, 'f', 1));

	_table->item(5,1)->setText(QString::number(avg_speed, 'f', 1));
	_table->item(6,1)->setText(QString::number(avg_hr, 'f', 2));
	_table->item(7,1)->setText(QString::number(avg_grad, 'f', 2));
	_table->item(8,1)->setText(QString::number(avg_cadence, 'f', 2));
	_table->item(9,1)->setText(QString::number(avg_power, 'f', 2));

	_table->item(11,1)->setText(QString::number(max_speed, 'f', 1));
	_table->item(12,1)->setText(QString::number(max_hr));
	_table->item(13,1)->setText(QString::number(max_gradient, 'f', 2));
	_table->item(14,1)->setText(QString::number(max_cadence));
	_table->item(15,1)->setText(QString::number(max_power, 'f', 2));
}