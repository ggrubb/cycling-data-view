#ifndef LOGDIRECTORYSUMMARY_H
#define LOGDIRECTORYSUMMARY_H

#include <QString.h>
#include <QDateTime.h>
#include <QStringList.h>

#include <vector>

#include <boost/shared_ptr.hpp>

class DataLog;

/**********************************/
struct LapSummary
{
	double _time;
	double _dist;
};

/**********************************/
struct LogSummary
{
	QString _filename;
	QString _date;
	double _time;
	double _dist;
	std::vector<LapSummary> _laps;

	QDate date() const
	{
		QString tmp = _date.split(' ')[0]; // split at the ' ' to get date only (no time)
		return QDate::fromString(tmp,Qt::ISODate);
	};
};

/**********************************/
class LogDirectorySummary
 {
 public:
	LogDirectorySummary(const QString& log_dir);
	~LogDirectorySummary();

	const QString& logDirectory() const;
	const LogSummary& log(int idx) const;
	int numLogs() const;

	void addLogsToSummary(const std::vector<boost::shared_ptr<DataLog> > data_logs);
	bool removeLogByName(const QString& filename);

	void readFromFile();
	void writeToFile() const;

	LogSummary firstLog() const; // chronologically first log
	LogSummary lastLog() const; // chronologically last log

 private:
	void addLog(const LogSummary& log_summary);

	QString _log_directory;
	std::vector<LogSummary> _logs;
 };

#endif // LOGDIRECTORYSUMMARY_H