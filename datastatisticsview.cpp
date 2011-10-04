#include "datastatisticsview.h"
#include "datalog.h"

#include <qtgui/qtablewidget>
#include <qtgui/qvboxlayout>
#include <iostream>

/******************************************************/
DataStatisticsView::DataStatisticsView()
{
	_table = new QTableWidget(13,2,this);

	QStringList column_headers, row_headers;
	column_headers 
		<< "Time (s)" << "Distance (km)" 
		<< "Avg Speed (km/h)" << "Avg Heart Rate (bpm)" << "Avg Gradient (%)" << "Avg Cadence (rpm)" << "Avg Power (W)" 
		<< "Elevation Gain (m)" << "Elevation Loss (m)" 
		<< "Max Speed (km/h)" << "Max Heart Rate (bpm)" << "Max Cadence (rpm)" << "Max Power (W)";
	row_headers << "Overall" << "Selection";
	_table->setVerticalHeaderLabels(column_headers);
	_table->setHorizontalHeaderLabels(row_headers);
	_table->resizeRowsToContents();
	_table->setColumnWidth(2,50);
	_table->setColumnWidth(3,50);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_table);
	setLayout(layout);
	resize(360,325);
	
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
	double avg_speed = DataLog::computeAverage(_data_log->speed().begin(), _data_log->speed().end());
	double avg_hr = DataLog::computeAverage(_data_log->heartRate().begin(), _data_log->heartRate().end());
	double avg_grad = DataLog::computeAverage(_data_log->gradient().begin(), _data_log->gradient().end());
	double avg_cadence = DataLog::computeAverage(_data_log->cadence().begin(), _data_log->cadence().end());
	double avg_power = DataLog::computeAverage(_data_log->power().begin(), _data_log->power().end());
	double elev_gain = DataLog::computeGain(_data_log->alt().begin(), _data_log->alt().end());
	double elev_loss = DataLog::computeLoss(_data_log->alt().begin(), _data_log->alt().end());
	double max_speed = DataLog::computeMax(_data_log->speed().begin(), _data_log->speed().end());
	double max_hr = DataLog::computeMax(_data_log->heartRate().begin(), _data_log->heartRate().end());
	double max_cadence = DataLog::computeMax(_data_log->cadence().begin(), _data_log->cadence().end());
	double max_power = DataLog::computeMax(_data_log->power().begin(), _data_log->power().end());
	
	// Set totals column
	_table->item(0,0)->setText(QString::number(time));
	_table->item(1,0)->setText(QString::number(dist));
	_table->item(2,0)->setText(QString::number(avg_speed));
	_table->item(3,0)->setText(QString::number(avg_hr));
	_table->item(4,0)->setText(QString::number(avg_grad));
	_table->item(5,0)->setText(QString::number(avg_cadence));
	_table->item(6,0)->setText(QString::number(avg_power));
	_table->item(7,0)->setText(QString::number(elev_gain));
	_table->item(8,0)->setText(QString::number(elev_loss));
	_table->item(9,0)->setText(QString::number(max_speed));
	_table->item(10,0)->setText(QString::number(max_hr));
	_table->item(11,0)->setText(QString::number(max_cadence));
	_table->item(12,0)->setText(QString::number(max_power));

	show();
}

/******************************************************/
void DataStatisticsView::clearTable()
{
	clearTotalsColumn();
	clearSelectionColumn();
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
void DataStatisticsView::setSelection(int idx_start, int idx_end)
{
	if (idx_start == 0 && idx_end == _data_log->numPoints()-1) // check if zoomed to full view
	{
		clearSelectionColumn();
	}
	else
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
		double max_cadence = DataLog::computeMax(_data_log->cadence().begin() + idx_start, _data_log->cadence().begin() + idx_end);
		double max_power = DataLog::computeMax(_data_log->power().begin() + idx_start, _data_log->power().begin() + idx_end);
		
		// Set selection column
		_table->item(0,1)->setText(QString::number(time));
		_table->item(1,1)->setText(QString::number(dist));
		_table->item(2,1)->setText(QString::number(avg_speed));
		_table->item(3,1)->setText(QString::number(avg_hr));
		_table->item(4,1)->setText(QString::number(avg_grad));
		_table->item(5,1)->setText(QString::number(avg_cadence));
		_table->item(6,1)->setText(QString::number(avg_power));
		_table->item(7,1)->setText(QString::number(elev_gain));
		_table->item(8,1)->setText(QString::number(elev_loss));
		_table->item(9,1)->setText(QString::number(max_speed));
		_table->item(10,1)->setText(QString::number(max_hr));
		_table->item(11,1)->setText(QString::number(max_cadence));
		_table->item(12,1)->setText(QString::number(max_power));
	}
}