#include "rideselectionwindow.h"
#include "datalog.h"
#include "tcxparser.h"
#include "dataprocessing.h"
#include "logdirectorysummary.h"

#include <QTreeView.h>
#include <QStandardItemModel.h>
#include <QDir.h>
#include <QProgressBar.h>
#include <QBoxLayout.h>
#include <iostream>

#define LOG_SUMMARY_FILENAME "logsummary.xml"

/******************************************************/
RideSelectionWindow::RideSelectionWindow()
{
	// Create dummy model with just the headers
	QStandardItemModel* model = new QStandardItemModel;
	QStandardItem* header0 = new QStandardItem(QString("Date"));
	QStandardItem* header1 = new QStandardItem(QString("Time (min)"));
	QStandardItem* header2 = new QStandardItem(QString("Length (km)"));
	model->setHorizontalHeaderItem(0,header0);
	model->setHorizontalHeaderItem(1,header1);
	model->setHorizontalHeaderItem(2,header2);
	
	// Create and show the tree view
	_tree = new QTreeView(this);
	_tree->setModel(model);
	formatTreeView();
	_tree->show();

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(_tree);
	setLayout(layout);
	setFixedSize(270,290);

	// Create parser and setup log directory summary
	_parser = new TcxParser();
	_current_data_log = 0;
	_log_dir_summary = 0;

}

/******************************************************/
RideSelectionWindow::~RideSelectionWindow()
{

}

/******************************************************/
void RideSelectionWindow::formatTreeView()
{
	_tree->setAlternatingRowColors(true);
	_tree->setColumnHidden(3,true);
	_tree->setColumnWidth(0,123);
	_tree->setColumnWidth(1,60);
	_tree->setColumnWidth(2,60);
	_tree->sortByColumn(0,Qt::DescendingOrder);
}

/******************************************************/
void RideSelectionWindow::setLogDirectory(const QString& path)
{
	// Read tcx files from specified directory
	QDir log_directory;
	QStringList filter;
	filter << "*.tcx";
	log_directory.setNameFilters(filter);
	log_directory.setPath(path);
	QStringList filenames = log_directory.entryList();

	// Try to load log summary file if it exists
	_log_dir_summary = new LogDirectorySummary(path);
	_log_dir_summary->readFromFile(path + "/" LOG_SUMMARY_FILENAME);

	// Compare files in directory with those in summary and remove if already in summary
	for (int j=0; j < _log_dir_summary->numLogs(); ++j)
	{
		QString filename_without_path = _log_dir_summary->log(j)._filename;
		filename_without_path.remove(QString(path + "/"));
		filenames.removeAll(filename_without_path);
	}

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

	// Load new log files in the directory
	std::vector<DataLog*> data_logs;
	for (int i=0; i < filenames.size(); ++i)
	{
		DataLog* data_log = new DataLog;
		const QString filename_with_path = log_directory.path() + "/" + filenames[i];
		std::cout << "reading: " << filename_with_path.toStdString()<< std::endl; 
		if (_parser->parse(filename_with_path, *data_log))
		{
			data_logs.push_back(data_log);
			_current_data_log = data_log;
		}
		load_progress->setValue(i);
	}
	delete load_progress;

	// Add the newly read rides to the summary
	_log_dir_summary->addLogsToSummary(data_logs);
	_log_dir_summary->writeToFile(log_directory.path() + "/" LOG_SUMMARY_FILENAME);

	populateTableWithRides();
}

/******************************************************/
void RideSelectionWindow::populateTableWithRides()
{
	_model = new QStandardItemModel;
	
	QStandardItem *parent_item = _model->invisibleRootItem();
	for (int i = 0; i < _log_dir_summary->numLogs(); ++i) 
	{
		// Data and time
		QString date = _log_dir_summary->log(i)._date;
		date.chop(3); // remove seconds
		QStandardItem *ride_name = new QStandardItem(date);
		ride_name->setFlags(ride_name->flags() & ~Qt::ItemIsEditable);

		// Ride time length
		QStandardItem *ride_time = new QStandardItem(DataProcessing::minsFromSecs(_log_dir_summary->log(i)._time));
		ride_time->setFlags(ride_time->flags() & ~Qt::ItemIsEditable);

		// Ride distance
		QStandardItem *ride_dist = new QStandardItem(DataProcessing::kmFromMeters(_log_dir_summary->log(i)._dist));
		ride_dist->setFlags(ride_dist->flags() & ~Qt::ItemIsEditable);

		// Index of ride in vector of all rides
		QStandardItem *ride_index = new QStandardItem(QString::number(i));

		QList<QStandardItem*> ride_list;
		ride_list << ride_name << ride_time << ride_dist << ride_index;
		parent_item->appendRow(ride_list);

		if (_log_dir_summary->log(i)._laps.size() > 1) // all rides are 1 lap, so only show laps for rides with > 1 lap
		{
			for (int lap = 0; lap < _log_dir_summary->log(i)._laps.size(); ++lap)
			{
				QStandardItem *lap_name = new QStandardItem("Lap " + QString::number(lap+1));
				lap_name->setFlags(lap_name->flags() & ~Qt::ItemIsEditable);
				
				// Compute lap summary info
				const double time = _log_dir_summary->log(i)._laps[lap]._time;
				const double dist = _log_dir_summary->log(i)._laps[lap]._dist;
				
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
	QStandardItem* header2 = new QStandardItem(QString("Length (km)"));
	_model->setHorizontalHeaderItem(0,header0);
	_model->setHorizontalHeaderItem(1,header1);
	_model->setHorizontalHeaderItem(2,header2);

	_tree->setModel(_model);
	formatTreeView();
	
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
		if (_current_data_log == 0)
		{
			_current_data_log = new DataLog;
			_parser->parse(_log_dir_summary->log(item->text().toInt())._filename, *_current_data_log);
		}
		else if (_current_data_log->filename() != _log_dir_summary->log(item->text().toInt())._filename)
		{
			_current_data_log = new DataLog;
			_parser->parse(_log_dir_summary->log(item->text().toInt())._filename, *_current_data_log);
		}

		// Notify to display the selected ride
		emit displayRide(_current_data_log);
	}
	else // user seletected a lap
	{
		// Get the item which represents the index
		QStandardItem* ride_item = _model->item(index.parent().row(),3); // 4th element is data log index (not displayed)

		if (_current_data_log == 0)
		{
			_current_data_log = new DataLog;
			_parser->parse(_log_dir_summary->log(ride_item->text().toInt())._filename, *_current_data_log);
			
			// Notify to display the selected ride
			emit displayRide(_current_data_log);
		}
		else if (_current_data_log->filename() != _log_dir_summary->log(ride_item->text().toInt())._filename)
		{
			_current_data_log = new DataLog;
			_parser->parse(_log_dir_summary->log(ride_item->text().toInt())._filename, *_current_data_log);
		
			// Notify to display the selected ride
			emit displayRide(_current_data_log);
		}

		// Get the item which represents the index
		QStandardItem* lap_item = _model->item(index.parent().row())->child(index.row(),3); // 4th element is lap index (not displayed)

		// Notif to display the selected lap
		emit displayLap(lap_item->text().toInt());
	}
}