#include "logeditorwindow.h"
#include "dataprocessing.h"
#include "datalog.h"
#include "user.h"
#include "fitencoder.h"

#include <QTableWidget.h>
#include <QBoxLayout.h>
#include <QLabel.h>
#include <QItemDelegate.h>
#include <QPainter.h>
#include <QMessageBox.h>
#include <QProgressDialog.h>
#include <QComboBox.h>
#include <QSpinBox.h>
#include <QPushButton.h>
#include <QGroupBox.h>
#include <QScrollBar.h>
#include <QFile.h>
#include <QString.h>

#include <iostream>
#include <cassert>

/******************************************************/
LogEditorWindow::LogEditorWindow(boost::shared_ptr<User> user, boost::shared_ptr<DataLog> data_log):
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
	QPushButton* save = new QPushButton("Save Changes");
	//QPushButton* split = new QPushButton("Split Log");
	QPushButton* exit = new QPushButton("Exit Editor");
	connect(save, SIGNAL(clicked()),this, SLOT(save()));
	//connect(split, SIGNAL(clicked()),this, SLOT(split()));
	connect(exit, SIGNAL(clicked()),this, SLOT(close()));

	QWidget* file_operator_buttons = new QWidget;
	QHBoxLayout* h_layout4 = new QHBoxLayout;
	h_layout4->addWidget(save);
	//h_layout4->addWidget(split);
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
	_search_value->setRange(-500,3000);
	_search_value->setDecimals(6);
	_search_value->setSingleStep(1.0);

	QPushButton* find = new QPushButton("Find");
	QPushButton* next = new QPushButton("Next >>");
	QPushButton* clear = new QPushButton("Clear");
	connect(find, SIGNAL(clicked()),this, SLOT(find()));
	connect(next, SIGNAL(clicked()),this, SLOT(next()));
	connect(clear, SIGNAL(clicked()),this, SLOT(clear()));

	QWidget* search_criteria = new QWidget;
	QHBoxLayout* h_layout1 = new QHBoxLayout;
	h_layout1->addWidget(_field_selection);
	h_layout1->addWidget(_equality_selection);
	h_layout1->addWidget(_search_value);
	h_layout1->setSpacing(0);
	h_layout1->setMargin(3);
	search_criteria->setLayout(h_layout1);

	QWidget* search_buttons = new QWidget;
	QHBoxLayout* h_layout2 = new QHBoxLayout;
	h_layout2->addWidget(find);
	h_layout2->addWidget(next);
	h_layout2->addWidget(clear);
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
	
	setMinimumSize(640,400);
	show();
}

/******************************************************/
LogEditorWindow::~LogEditorWindow()
{
	for (int r=0; r < _table->rowCount(); ++r)
	{
		for (int c=2; c < _table->columnCount(); ++c)
		{
			delete _table->item(r, c);
		}
	}
	delete _table;
}

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

/******************************************************/
bool LogEditorWindow::searchComparison(double left, double right)
{
	// 0 = "equals"
	// 1 = "is less than"
	// 2 = "is greater than"
	if (_equality_selection->currentIndex() == 0)
		return left == right;
	else if (_equality_selection->currentIndex() == 1)
		return left < right;
	else
		return left > right;
}

/******************************************************/
void LogEditorWindow::find()
{
	clear();

	// 0 = "Speed"
	// 1 = "Heart rate"
	// 2 = "Cadence"
	// 3 = "Altitude"
	// 4 = "Latitude"
	// 5 = "Longitude"
	// 6 = "Temperature"
	const int index = _field_selection->currentIndex();
	const double value = _search_value->value();
	_search_result_indecies.clear();

	for (int r=0; r < _table->rowCount(); ++r)
	{
		QTableWidgetItem* item = _table->item(r, index + 2);
		if (searchComparison(item->text().toDouble(), value))
		{		
			item->setBackgroundColor(Qt::red);
			_search_result_indecies.push_back(r);
		}
	}

	// Scroll to the first search result
	if (_search_result_indecies.size() > 0)
	{
		_table->verticalScrollBar()->setValue(_search_result_indecies[0]);
	}
	_search_result_index = 0;

}

/******************************************************/
void LogEditorWindow::clear()
{
	for (int r=0; r < _table->rowCount(); ++r)
	{
		for (int c=2; c < _table->columnCount(); ++c)
		{
			_table->item(r, c)->setBackgroundColor(Qt::white);
		}
	}
	_search_result_indecies.clear();
	_search_result_index = 0;
	_table->verticalScrollBar()->setValue(0);
}

/******************************************************/
void LogEditorWindow::next()
{
	_search_result_index++;
	if (_search_result_index < (int)_search_result_indecies.size())
		_table->verticalScrollBar()->setValue(_search_result_indecies[_search_result_index]);
	else
		QMessageBox::information(this, tr("RideLogEditor"), tr("Reached end of search results."));
}

/******************************************************/
void LogEditorWindow::save()
{
	// Get the current values in the UI and store in the date log
	int index = 0;
	QTableWidgetItem* item;
	for (int r=0; r < _table->rowCount(); ++r)
	{
		index = 0; // 0 = "Speed"
		item = _table->item(r, index + 2);
		_data_log->speed(r) = item->text().toDouble();

		index = 1; // 1 = "Heart rate"
		item = _table->item(r, index + 2);
		_data_log->heartRate(r) = item->text().toDouble();

		index = 2; // 2 = "Cadence"
		item = _table->item(r, index + 2);
		_data_log->cadence(r) = item->text().toDouble();

		index = 3; // 3 = "Altitude"
		item = _table->item(r, index + 2);
		_data_log->alt(r) = item->text().toDouble();

		index = 4; // 4 = "Latitude"
		item = _table->item(r, index + 2);
		_data_log->ltd(r) = item->text().toDouble();

		index = 5; // 5 = "Longitude"
		item = _table->item(r, index + 2);
		_data_log->lgd(r) = item->text().toDouble();

		index = 6; // 6 = "Temperature"
		item = _table->item(r, index + 2);
		_data_log->temp(r) = item->text().toDouble();
	}

	// Now write the log to disk
	QString source_file = _data_log->filename();
	source_file.replace("/", "\\");

	QString target_file = _data_log->filename();
	target_file.chop(3);
	target_file.append("orig");
	target_file.replace("/", "\\");
	
	// Rename original file
	bool original_backed_up = false;
	if (QFile::exists(target_file))
	{
		original_backed_up = true;
		QMessageBox::information(this, tr("RideLogEditor"), tr("Original file is already backed up - it will not be saved again"));
	}

	if (QFile::copy(source_file,target_file) && ! original_backed_up)
	{
		original_backed_up = true;
		QMessageBox::information(this, tr("RideLogEditor"), tr("This will rename the original file:\n  ") + source_file + "\nto:\n  " + target_file +  "\nand save your edits in the current filename.");
	}

	if (original_backed_up)
	{
		// Encode to existing file
		FitEncoder fit_encoder;
		if (!fit_encoder.encode(_data_log->filename(), *_data_log))
			QMessageBox::warning(this, tr("RideLogEditor"), tr("Failed to write file."));
	}
	//_data_log->saveToTextFile("saved_log.txt");

	// Update the plots
	_data_log->setModified(true); // log is modified so the plots will be updated
	emit dataLogUpdated(_data_log);
	_data_log->setModified(false); // plots are updated, so set modified flag back to false
}

/******************************************************/
void LogEditorWindow::split()
{
	QMessageBox::information(this, tr("RideLogEditor"), tr("This will save a copy of the file to ") + _data_log->filename() + ".pt1 " + _data_log->filename() + ".pt2.");
}