#include "rideselectionwindow.h"
#include "datalog.h"
#include "tcxparser.h"
#include "dataprocessing.h"

#include <QTreeView.h>
#include <QStandardItemModel.h>
#include <QDir.h>
#include <QProgressBar.h>
#include <iostream>

/******************************************************/
RideSelectionWindow::RideSelectionWindow()
{
	// Create dummy model with just the headers
	QStandardItemModel* model = new QStandardItemModel;
	QStandardItem* header0 = new QStandardItem(QString("Date"));
	QStandardItem* header1 = new QStandardItem(QString("Time (min)"));
	QStandardItem* header2 = new QStandardItem(QString("Distance (km)"));
	model->setHorizontalHeaderItem(0,header0);
	model->setHorizontalHeaderItem(1,header1);
	model->setHorizontalHeaderItem(2,header2);
	
	// Create and show the tree view
	_tree = new QTreeView(this);
	_tree->setModel(model);
	_tree->setAlternatingRowColors(true);
	_tree->setColumnWidth(0,123);
	_tree->setColumnWidth(1,60);
	_tree->setColumnWidth(2,60);
	_tree->setFixedSize(300,290);
	_tree->show();

	// Setup the data log parser and read directory
	_parser = new TcxParser();
	_data_logs.resize(0);
	_current_data_log = 0;
	_log_directory = new QDir;
	QStringList filter;
	filter << "*.tcx";
	_log_directory->setNameFilters(filter);

}

/******************************************************/
RideSelectionWindow::~RideSelectionWindow()
{
	
}

/******************************************************/
void RideSelectionWindow::setLogDirectory(const QString& path)
{
	_log_directory->setPath(path);
	QStringList filenames;
	filenames = _log_directory->entryList();

	// Create a small progress bar
	QProgressBar* load_progress = new QProgressBar();
	load_progress->setWindowFlags(Qt::SplashScreen);
	QPalette palette(load_progress->palette());
	palette.setColor(QPalette::Window, Qt::white);
	load_progress->setPalette(palette);
	load_progress->move(50,150);
	load_progress->setMinimum(0);
	load_progress->setMaximum(filenames.size());
	load_progress->show();

	// Load log files in the directory
	for (int i=0; i < std::min(filenames.size(),99); ++i)
	{
		DataLog* data_log = new DataLog;
		const bool parse_summary_only = false;
		const QString filename_with_path = _log_directory->path() + "/" + filenames[i];
		std::cout << "reading: " << filename_with_path.toStdString(); 
		if (_parser->parse(filename_with_path, *data_log, parse_summary_only))
		{
			_data_logs.push_back(data_log);
			std::cout << " success!";
		}
		std::cout << std::endl;
		load_progress->setValue(i);
	}
	load_progress->hide();
	delete load_progress;

	populateWithRides(_data_logs);
}

/******************************************************/
void RideSelectionWindow::populateWithRides(const std::vector<DataLog*>& data_logs)
{
	_model = new QStandardItemModel;
	
	QStandardItem *parent_item = _model->invisibleRootItem();
	for (unsigned int i = 0; i < data_logs.size(); ++i) {
		// Data and time
		QString date = data_logs[i]->date();
		date.chop(3); // remove seconds
		QStandardItem *ride_name = new QStandardItem(date);
		ride_name->setFlags(ride_name->flags() & ~Qt::ItemIsEditable);

		// Ride time length
		QStandardItem *ride_time = new QStandardItem(DataProcessing::minsFromSecs(data_logs[i]->totalTime()));
		ride_time->setFlags(ride_time->flags() & ~Qt::ItemIsEditable);

		// Ride distance
		QStandardItem *ride_dist = new QStandardItem(DataProcessing::kmFromMeters(data_logs[i]->totalDist()));
		ride_dist->setFlags(ride_dist->flags() & ~Qt::ItemIsEditable);

		// Index of ride in vector of all rides
		QStandardItem *ride_index = new QStandardItem(QString::number(i));

		QList<QStandardItem*> ride_list;
		ride_list << ride_name << ride_time << ride_dist << ride_index;
		parent_item->appendRow(ride_list);

		if (data_logs[i]->numLaps() > 1) // all rides are 1 lap, so only show laps for rides with > 1 lap
		{
			for (int lap = 0; lap < data_logs[i]->numLaps(); ++lap)
			{
				QStandardItem *lap_name = new QStandardItem("Lap " + QString::number(lap+1));
				lap_name->setFlags(lap_name->flags() & ~Qt::ItemIsEditable);
				
				// Compute lap summary info
				std::pair<int, int> lap_indecies = data_logs[i]->lap(lap);
				const double time = data_logs[i]->time(lap_indecies.second) - data_logs[i]->time(lap_indecies.first);
				const double dist = data_logs[i]->dist(lap_indecies.second) - data_logs[i]->dist(lap_indecies.first);
				
				QStandardItem *lap_time = new QStandardItem(DataProcessing::minsFromSecs(time));
				lap_name->setFlags(lap_name->flags() & ~Qt::ItemIsEditable);

				QStandardItem *lap_dist = new QStandardItem(DataProcessing::kmFromMeters(dist));
				lap_dist->setFlags(lap_dist->flags() & ~Qt::ItemIsEditable);

				// Index of ride in vector of all rides
				QStandardItem *lap_index = new QStandardItem(QString::number(lap));

				QList<QStandardItem*> lap_list;
				lap_list << lap_name << lap_time << lap_dist << lap_index;
				ride_name->appendRow(lap_list);
			}
		}
	}

	QStandardItem* header0 = new QStandardItem(QString("Date"));
	QStandardItem* header1 = new QStandardItem(QString("Time (min)"));
	QStandardItem* header2 = new QStandardItem(QString("Distance (km)"));
	_model->setHorizontalHeaderItem(0,header0);
	_model->setHorizontalHeaderItem(1,header1);
	_model->setHorizontalHeaderItem(2,header2);

	_tree->setModel(_model);
	//_tree->setColumnHidden(3, true); // hide the index column
	_tree->sortByColumn(0,Qt::DescendingOrder);
	connect(_tree, SIGNAL(clicked(const QModelIndex&)),this,SLOT(rideSelected(const QModelIndex&)));
	_tree->setCurrentIndex(_model->index(0,0));
	rideSelected(_tree->currentIndex());
}

/******************************************************/
void RideSelectionWindow::rideSelected(const QModelIndex& index)
{
	if (index.parent() == QModelIndex()) // user selected the entire ride
	{
		// Get the item which represents the index
		QStandardItem* item = _model->item(index.row(),3); // 4th element is data log index (not displayed)

		// Parse complete ride details
		if (_current_data_log != _data_logs[item->text().toInt()])
		{
			_current_data_log = _data_logs[item->text().toInt()];
			_parser->parse(_current_data_log->filename(), *_current_data_log);
		}

		// Notify to display the selected ride
		emit displayRide(_current_data_log);
	}
	else // user seletected a lap
	{
		// Get the item which represents the index
		QStandardItem* item = _model->item(index.parent().row())->child(index.row(),3); // 4th element is lap index (not displayed)

		// Notif to display the selected lap
		emit displayLap(item->text().toInt());
	}
}