#ifndef LOGDIRECTORYSUMMARY_H
#define LOGDIRECTORYSUMMARY_H

#include <QString.h>
#include <vector>

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
	void addLog(const LogSummary& log_summary);

	void readFromFile(const QString& filename);
	void writeToFile(const QString& filename) const;
	void addLogsToSummary(const std::vector<DataLog*> data_logs);
 private:

	QString _log_directory;
	std::vector<LogSummary> _logs;
 };

#endif // LOGDIRECTORYSUMMARY_H