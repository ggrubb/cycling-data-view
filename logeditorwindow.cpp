#include "logeditorwindow.h"
#include "dataprocessing.h"
#include "datalog.h"
#include "user.h"

#include <QTableWidget.h>
#include <QBoxLayout.h>
#include <QLabel.h>
#include <QItemDelegate.h>
#include <QPainter.h>
#include <QMessageBox.h> // debug only
#include <QProgressDialog.h>

#include <iostream>
#include <cassert>

/******************************************************/
class ItemDelegate: public QItemDelegate
{
public:
	ItemDelegate()
	{}

	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const 
	{
		QPalette pal = option.palette;
		QStyleOptionViewItem  view_option(option);
		if (index.row() <= 1) 
		{
			painter->fillRect(option.rect, QColor(233,233,233));
		} 
		
		QItemDelegate::paint(painter, view_option, index);
	}
};

/******************************************************/
LogEditorWindow::LogEditorWindow(User* user, DataLog* data_log):
_data_log(data_log),
_user(user)
{
	const int num_data_pts = data_log->numPoints();
	const int num_columns = 9;

	_table = new QTableWidget(num_data_pts,num_columns,this);
	_table->setSelectionMode(QAbstractItemView::NoSelection);

	QStringList column_headers, row_headers;
	column_headers 
		<< "Time" << "Distance" << "Speed" << "Heart rate" << "Cadence" << "Altitude" << "Latitude" << "Longitude" << "Temperature";

	_table->setVerticalHeaderLabels(row_headers);
	_table->setHorizontalHeaderLabels(column_headers);

	_head_label = new QLabel;
	_head_label->setTextFormat(Qt::RichText);
	_head_label->setText("<b>Ride Data</b>");

	// Format the table
	displayRide();
	_table->resizeColumnsToContents();

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_head_label);
	layout->addWidget(_table);
	setLayout(layout);
	
	setMinimumSize(670,400);
	show();
}

/******************************************************/
LogEditorWindow::~LogEditorWindow()
{}

/******************************************************/
void LogEditorWindow::displayRide()
{
	assert(_user);
	assert(_data_log);

	_head_label->setText("<b>Ride Data For:</b> " + _data_log->dateString());

	QProgressDialog load_progress("Loading log: " + _data_log->dateString(), "Cancel load", 0, _data_log->numPoints()-1, this);
	load_progress.setWindowModality(Qt::WindowModal);
	load_progress.setMinimumDuration(0); //msec
	load_progress.setWindowTitle("RideLogEditor");

	for (int r=0; r < _data_log->numPoints(); ++r)
	{
		if (r%200 == 0)
			load_progress.setValue(r);
		if (load_progress.wasCanceled())
			break;

		_table->setRowHeight(r,16);

		QTableWidgetItem *item1 = new QTableWidgetItem(DataProcessing::minsFromSecs(_data_log->time(r)));
		_table->setItem(r,0,item1);
		
		QTableWidgetItem *item2 = new QTableWidgetItem(DataProcessing::kmFromMeters(_data_log->dist(r)));
		_table->setItem(r,1,item2);

		QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(_data_log->speed(r), 'f', 1));
		_table->setItem(r,2,item3);

		QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(_data_log->heartRate(r), 'f', 0));
		_table->setItem(r,3,item4);

		QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(_data_log->cadence(r), 'f', 0));
		_table->setItem(r,4,item5);

		QTableWidgetItem *item6 = new QTableWidgetItem(QString::number(_data_log->alt(r), 'f', 0));
		_table->setItem(r,5,item6);

		QTableWidgetItem *item7 = new QTableWidgetItem(QString::number(_data_log->ltd(r), 'f', 6));
		_table->setItem(r,6,item7);

		QTableWidgetItem *item8 = new QTableWidgetItem(QString::number(_data_log->lgd(r), 'f', 6));
		_table->setItem(r,7,item8);

		QTableWidgetItem *item9 = new QTableWidgetItem(QString::number(_data_log->temp(r), 'f', 0));
		_table->setItem(r,8,item9);
	}
}
