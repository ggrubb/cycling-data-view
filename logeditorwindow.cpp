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
#include <QComboBox.h>
#include <QSpinBox.h>
#include <QPushButton.h>
#include <QGroupBox.h>

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

	// Setup the table
	_table = new QTableWidget(num_data_pts,num_columns,this);
	_table->setSelectionMode(QAbstractItemView::NoSelection);

	QStringList column_headers, row_headers;
	column_headers 
		<< "Time" << "Distance" << "Speed" << "Heart rate" << "Cadence" << "Altitude" << "Latitude" << "Longitude" << "Temperature";

	_table->setVerticalHeaderLabels(row_headers);
	_table->setHorizontalHeaderLabels(column_headers);

	// Setup the title label
	_head_label = new QLabel;
	_head_label->setTextFormat(Qt::RichText);
	_head_label->setText("<b>Ride Data</b>");

	// Log file options
	QPushButton* save = new QPushButton("Save");
	QPushButton* split = new QPushButton("Split Log");
	QPushButton* exit = new QPushButton("Exit");

	QWidget* file_operator_buttons = new QWidget;
	QHBoxLayout* h_layout4 = new QHBoxLayout;
	h_layout4->addWidget(save);
	h_layout4->addWidget(split);
	h_layout4->addWidget(exit);
	file_operator_buttons->setLayout(h_layout4);

	// Setup the search box
	_field_selection = new QComboBox();
	_field_selection->insertItem(0, "Speed");
	_field_selection->insertItem(1, "Heart rate");
	_field_selection->insertItem(2, "Cadence");
	_field_selection->insertItem(3, "Altitude");
	_field_selection->insertItem(4, "Latitude");
	_field_selection->insertItem(5, "Longitude");
	_field_selection->insertItem(6, "Temperature");

	_equality_selection = new QComboBox();
	_equality_selection->insertItem(0, "equals");
	_equality_selection->insertItem(1, "is less than");
	_equality_selection->insertItem(2, "is greater than");

	_search_value = new QDoubleSpinBox();
	_search_value->setDecimals(6);
	_search_value->setSingleStep(1.0);

	QPushButton* find = new QPushButton("Find");
	QPushButton* next = new QPushButton("Next >>");
	QPushButton* clear = new QPushButton("Clear");

	QWidget* search_criteria = new QWidget;
	QHBoxLayout* h_layout1 = new QHBoxLayout;
	h_layout1->addWidget(_field_selection);
	h_layout1->addWidget(_equality_selection);
	h_layout1->addWidget(_search_value);
	//h_layout1->addStretch();
	h_layout1->setSpacing(0);
	h_layout1->setMargin(3);
	search_criteria->setLayout(h_layout1);

	QWidget* search_buttons = new QWidget;
	QHBoxLayout* h_layout2 = new QHBoxLayout;
	h_layout2->addWidget(find);
	h_layout2->addWidget(next);
	h_layout2->addWidget(clear);
	//h_layout2->addStretch();
	h_layout2->setSpacing(0);
	h_layout2->setMargin(3);
	search_buttons->setLayout(h_layout2);

	QVBoxLayout* v_layout = new QVBoxLayout();
	v_layout->addWidget(search_criteria);
	v_layout->addWidget(search_buttons);
	v_layout->setSpacing(0);
	v_layout->setMargin(2);
	
	QGroupBox* search_box = new QGroupBox("Search");
	search_box->setLayout(v_layout);

	// Format the table
	displayRide();
	_table->resizeColumnsToContents();

	// Layout the entire window
	QWidget* user_controls = new QWidget;
	QHBoxLayout* h_layout3 = new QHBoxLayout;
	h_layout3->addWidget(search_box);
	h_layout3->addWidget(file_operator_buttons,0,Qt::AlignTop);
	h_layout3->addStretch();
	user_controls->setLayout(h_layout3);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_head_label);
	layout->addWidget(user_controls);
	layout->addWidget(_table);
	layout->setSpacing(0);
	layout->setMargin(2);
	setLayout(layout);

	setWindowTitle("RideEditor");
	setWindowIcon(QIcon("./resources/rideviewer_head128x128.ico")); 
	
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
