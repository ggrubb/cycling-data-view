#include "datastatisticsview.h"

#include <qtgui/qtablewidget>
#include <qtgui/qvboxlayout>
#include <iostream>

/******************************************************/
DataStatisticsView::DataStatisticsView()
{
	_table = new QTableWidget(12,2,this);

	QStringList column_headers, row_headers;
	column_headers 
		<< "Time (s)" << "Distance (km)" 
		<< "Avg Speed (km/h)" << "Avg Heart Rate (bpm)" << "Avg Gradient (%)" << "Avg Cadence (rpm)" << "Avg Power (W)" 
		<< "Elevation Gain (m)" << "Elevation Loss (m)" 
		<< "Max Speed (km/h)" << "Max Cadence (rpm)" << "Max Heart Rate (bpm)" << "Max Power (W)";
	row_headers << "Overall" << "Selection";
	_table->setVerticalHeaderLabels(column_headers);
	_table->setHorizontalHeaderLabels(row_headers);
	_table->resizeRowsToContents();
	_table->setColumnWidth(2,50);
	_table->setColumnWidth(3,50);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_table);
	setLayout(layout);
	resize(360,310);
	show();

	clearTable();
}

/******************************************************/
DataStatisticsView::~DataStatisticsView()
{

}

/******************************************************/
void DataStatisticsView::setTotals()
{

}

/******************************************************/
void DataStatisticsView::clearTable()
{
	clearTotals();
	clearSelection();
}

/******************************************************/
void DataStatisticsView::clearTotals()
{
	for (int r = 0; r < _table->rowCount(); ++r)
	{
		QTableWidgetItem *item = new QTableWidgetItem("-");
		item->setTextAlignment(Qt::AlignCenter);
		_table->setItem(r,0,item);
	}
}

/******************************************************/
void DataStatisticsView::clearSelection()
{
	for (int r = 0; r < _table->rowCount(); ++r)
	{
		QTableWidgetItem *item = new QTableWidgetItem("-");
		item->setTextAlignment(Qt::AlignCenter);
		_table->setItem(r,1,item);
	}
}