#include "rideselectionwindow.h"
#include "datalog.h"
#include "tcxparser.h"
#include "dataprocessing.h"

#include <QTreeView.h>
#include <QStandardItemModel.h>
#include <QDir.h>
#include <iostream>

/******************************************************/
RideSelectionWindow::RideSelectionWindow()
{
	// Create dummy model with just the headers
	QStandardItemModel* model = new QStandardItemModel;
	QStandardItem* header0 = new QStandardItem(QString("Date"));
	QStandardItem* header1 = new QStandardItem(QString("Time"));
	QStandardItem* header2 = new QStandardItem(QString("Distance"));
	model->setHorizontalHeaderItem(0,header0);
	model->setHorizontalHeaderItem(1,header1);
	model->setHorizontalHeaderItem(2,header2);
	
	// Create and show the tree view
	_tree = new QTreeView(this);
	_tree->setModel(model);
	_tree->setAlternatingRowColors(true);
	_tree->setColumnWidth(0,123);
	_tree->setColumnWidth(1,55);
	_tree->setColumnWidth(2,55);
	_tree->setFixedSize(300,290);
	_tree->show();

	// Setup the data log parser and read directory
	_parser = new TcxParser();
	_data_logs.resize(0);
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

	for (int i=0; i < std::min(filenames.size(),3); ++i)
	{
		DataLog* data_log = new DataLog;
		const bool parse_summary_only = true;
		const QString filename_with_path = _log_directory->path() + "/" + filenames[i];
		std::cout << "reading: " << filename_with_path.toStdString(); 
		if (_parser->parse(filename_with_path, *data_log, parse_summary_only))
		{
			_data_logs.push_back(data_log);
			std::cout << " success!";
		}
		std::cout << std::endl;
	}

	populateWithRides(_data_logs);
}

/******************************************************/
void RideSelectionWindow::populateWithRides(const std::vector<DataLog*>& data_logs)
{
	_model = new QStandardItemModel;
	
	QStandardItem *parent_item = _model->invisibleRootItem();
	for (int i = 0; i < data_logs.size(); ++i) {
		// Data and time
		QString date = data_logs[i]->date();
		date.chop(3); // remove seconds
		QStandardItem *ride_name = new QStandardItem(date);
		ride_name->setFlags(ride_name->flags() & ~Qt::ItemIsEditable);

		// Ride time length
		const std::pair<int,int> minutes = DataProcessing::minutesFromSeconds(data_logs[i]->totalTime());
		QStandardItem *ride_time = new QStandardItem(QString::number(minutes.first) + ":" + QString::number(minutes.second));
		ride_time->setFlags(ride_time->flags() & ~Qt::ItemIsEditable);

		// Ride distance
		const double dist_km = DataProcessing::kmFromMeters(data_logs[i]->totalDist());
		QStandardItem *ride_dist = new QStandardItem(QString::number(dist_km,'f',2));
		ride_dist->setFlags(ride_dist->flags() & ~Qt::ItemIsEditable);

		// Index of ride in vector of all rides
		QStandardItem *ride_index = new QStandardItem(QString::number(i));

		QList<QStandardItem*> ride_list;
		ride_list << ride_name << ride_time << ride_dist << ride_index;
		parent_item->appendRow(ride_list);

		for (int lap = 0; lap < data_logs[i]->numLaps(); ++lap)
		{
			QStandardItem *lap_name = new QStandardItem(QString::number(lap));
			lap_name->setFlags(lap_name->flags() & ~Qt::ItemIsEditable);

			QStandardItem *lap_time = new QStandardItem(QString("%0 min").arg(34));
			lap_name->setFlags(lap_name->flags() & ~Qt::ItemIsEditable);

			QStandardItem *lap_dist = new QStandardItem(QString("%0 km").arg(5.3));
			lap_dist->setFlags(lap_dist->flags() & ~Qt::ItemIsEditable);

			// Index of ride in vector of all rides
			QStandardItem *ride_index = new QStandardItem(QString::number(i));

			QList<QStandardItem*> lap_list;
			lap_list << lap_name << lap_time << lap_dist << ride_index;
			ride_name->appendRow(lap_list);
		}
	}

	QStandardItem* header0 = new QStandardItem(QString("Date"));
	QStandardItem* header1 = new QStandardItem(QString("Time"));
	QStandardItem* header2 = new QStandardItem(QString("Distance"));
	_model->setHorizontalHeaderItem(0,header0);
	_model->setHorizontalHeaderItem(1,header1);
	_model->setHorizontalHeaderItem(2,header2);

	_tree->setModel(_model);
	_tree->setColumnHidden(3, true); // hide the index column
	_tree->sortByColumn(0,Qt::DescendingOrder);
	connect(_tree, SIGNAL(clicked(const QModelIndex&)),this,SLOT(rideSelected(const QModelIndex&)));
	_tree->setCurrentIndex(_model->index(0,0));
	rideSelected(_tree->currentIndex());
}

/******************************************************/
void RideSelectionWindow::rideSelected(const QModelIndex& index)
{
	// Get the item which represents the index
	QStandardItem* item = _model->item(index.row(),3);

	// Parse complete ride details
	DataLog* data_log = _data_logs[item->text().toInt()];
	_parser->parse(data_log->filename(), *data_log);

	// Notify to display the selected ride
	emit displayRide(data_log);
}