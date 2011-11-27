#ifndef FITPARSER_H
#define FITPARSER_H

#include "baseparser.h"

#include <fstream>

#include <QDateTime.h>

#include "garminfitsdk/fit_mesg_broadcaster.hpp"

class DataLog;
class QString;

//***********************************************************
class Listener : public fit::RecordMesgListener, public fit::LapMesgListener
{
public:
	Listener(DataLog* data_log);
	int numPointsRead();

	void OnMesg(fit::RecordMesg& mesg);
	void OnMesg(fit::LapMesg& mesg);

private:
	DataLog* _data_log;
	int _track_point_index;
	int _start_time; // secs
	double _pos_factor;
	QDateTime _base_date;
};

//***********************************************************
class FitParser : public BaseParser
 {
 public:
	FitParser();
	~FitParser();

	// Parses data from .fit in filename. Returns true if file was parsed successfully
	bool parse(const QString& filename, DataLog& data_log);

 protected:
	bool parseRideDetails(DataLog& data_log);

 private:
	fit::MesgBroadcaster* _mesg_broadcaster;
	std::fstream* _file;
	Listener* _listener;
 };

#endif // FITPARSER_H