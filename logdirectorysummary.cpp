#include "logdirectorysummary.h"
#include "datalog.h"

#include <cassert>
#include <fstream>

#include <qtxml/qdomdocument>
#include <QFile.h>

#define LOG_SUMMARY_FILENAME "logsummary.xml"

/****************************************/
LogDirectorySummary::LogDirectorySummary(const QString& log_directory):
_log_directory(log_directory)
{}

/****************************************/
LogDirectorySummary::~LogDirectorySummary()
{}

/****************************************/
const QString& LogDirectorySummary::logDirectory() const
{
	return _log_directory;
}

/****************************************/
const LogSummary& LogDirectorySummary::log(int idx) const
{
	assert(idx < (int)_logs.size());
	assert(idx >= 0);

	return _logs[idx];
}

/****************************************/
int LogDirectorySummary::numLogs() const
{
	return _logs.size();
}

/****************************************/
void LogDirectorySummary::addLog(const LogSummary& log_summary)
{
	_logs.push_back(log_summary);
}

/****************************************/
void LogDirectorySummary::readFromFile()
{
	const QString filename = _log_directory + "/" LOG_SUMMARY_FILENAME;

	QDomDocument dom_document;
	QString error_msg;
	int error_line, error_column;
	QFile file(filename);
	bool read_success = dom_document.setContent(&file, &error_msg, &error_line, &error_column);
	
	if (read_success)
	{
		QDomElement doc = dom_document.documentElement();

		QDomNode log = doc.firstChildElement("Log");
		while (!log.isNull())
		{
			LogSummary log_summary;
			log_summary._filename = log.firstChildElement("Filename").firstChild().nodeValue();
			log_summary._date = log.firstChildElement("Date").firstChild().nodeValue();
			log_summary._time = log.firstChildElement("Time").firstChild().nodeValue().toDouble();
			log_summary._dist = log.firstChildElement("Distance").firstChild().nodeValue().toDouble();

			QDomNode lap = log.firstChildElement("Laps").firstChild();
			while (!lap.isNull())
			{
				LapSummary lap_summary;
				lap_summary._time = lap.firstChildElement("Time").firstChild().nodeValue().toDouble();
				lap_summary._dist = lap.firstChildElement("Distance").firstChild().nodeValue().toDouble();
				log_summary._laps.push_back(lap_summary);
				
				lap = lap.nextSibling();
			}
			
			addLog(log_summary);
			log = log.nextSibling();
		}
	}
}

/****************************************/
void LogDirectorySummary::writeToFile() const
{
	const QString filename = _log_directory + "/" LOG_SUMMARY_FILENAME;

	QDomDocument dom_document;
	QDomElement doc = dom_document.createElement("LogSummary");
	dom_document.appendChild(doc);

	for (unsigned int i=0; i < _logs.size(); ++i)
	{
		QDomElement log = dom_document.createElement("Log");
		doc.appendChild(log);

		QDomElement log_filename = dom_document.createElement("Filename");
		log.appendChild(log_filename);
		QDomText text = dom_document.createTextNode(_logs[i]._filename);
		log_filename.appendChild(text);

		QDomElement date = dom_document.createElement("Date");
		log.appendChild(date);
		text = dom_document.createTextNode(_logs[i]._date);
		date.appendChild(text);

		QDomElement time = dom_document.createElement("Time");
		log.appendChild(time);
		text = dom_document.createTextNode(QString::number(_logs[i]._time,'f',2));
		time.appendChild(text);

		QDomElement dist = dom_document.createElement("Distance");
		log.appendChild(dist);
		text = dom_document.createTextNode(QString::number(_logs[i]._dist,'f',2));
		dist.appendChild(text);

		QDomElement laps = dom_document.createElement("Laps");
		log.appendChild(laps);

		for (unsigned int j=0; j < _logs[i]._laps.size(); ++j)
		{
			QDomElement lap = dom_document.createElement("Lap");
			laps.appendChild(lap);

			QDomElement id = dom_document.createElement("Id");
			lap.appendChild(id);
			text = dom_document.createTextNode(QString::number(j));
			id.appendChild(text);

			QDomElement time = dom_document.createElement("Time");
			lap.appendChild(time);
			text = dom_document.createTextNode(QString::number(_logs[i]._laps[j]._time,'f',2));
			time.appendChild(text);

			QDomElement dist = dom_document.createElement("Distance");
			lap.appendChild(dist);
			text = dom_document.createTextNode(QString::number(_logs[i]._laps[j]._dist,'f',2));
			dist.appendChild(text);
		}
	}

	const int indent = 4;
	QString xml = dom_document.toString(indent);
	std::ofstream file;
	file.open(filename.toStdString().c_str());
	file << xml.toStdString();
	file.close();
}

/******************************************************/
void LogDirectorySummary::addLogsToSummary(const std::vector<boost::shared_ptr<DataLog> > data_logs)
{
	for (unsigned int lg = 0; lg < data_logs.size(); ++lg)
	{
		LogSummary log_summary;
		log_summary._filename = data_logs[lg]->filename();
		log_summary._date = data_logs[lg]->dateString();
		log_summary._time = data_logs[lg]->totalTime();
		log_summary._dist = data_logs[lg]->totalDist();
		for (int i=0; i < data_logs[lg]->numLaps(); ++i)
		{
			LapSummary lap_summary;
			std::pair<int, int> lap_indecies = data_logs[lg]->lap(i);
			lap_summary._time = data_logs[lg]->time(lap_indecies.second) - data_logs[lg]->time(lap_indecies.first);
			lap_summary._dist = data_logs[lg]->dist(lap_indecies.second) - data_logs[lg]->dist(lap_indecies.first);
			log_summary._laps.push_back(lap_summary);
		}
		addLog(log_summary);
	}
}

/******************************************************/
LogSummary LogDirectorySummary::firstLog() const
{
	QDate min_date(2100, 1, 1); // arbitrarily chosen later date after now
	int min_index = 0;

	for (int i=0; i < numLogs(); ++i)
	{
		QString tmp = log(i)._date.split(' ')[0]; // split at the ' ' to get date only (no time)
		QDate date = QDate::fromString(tmp,Qt::ISODate);

		if (date < min_date)
		{
			min_date = date;
			min_index = i;
		}
	}

	return log(min_index);
}

/******************************************************/
LogSummary LogDirectorySummary::lastLog() const
{
	QDate max_date(1970, 1, 1); // arbitrarily chosen ealier date before now
	int max_index = 0;

	for (int i=0; i < numLogs(); ++i)
	{
		QDate date = log(i).date();

		if (date > max_date)
		{
			max_date = date;
			max_index = i;
		}
	}

	return log(max_index);
}