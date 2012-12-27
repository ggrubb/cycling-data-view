#ifndef FITPARSER_H
#define FITPARSER_H

#include "baseparser.h"

#include <fstream>

#include <QDateTime.h>

#include "garminfitsdk/fit_mesg_broadcaster.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

class DataLog;
class QString;

//***********************************************************
class Listener : public fit::RecordMesgListener, public fit::LapMesgListener
{
public:
	Listener(boost::shared_ptr<DataLog> data_log);
	int numPointsRead();

	void OnMesg(fit::RecordMesg& mesg);
	void OnMesg(fit::LapMesg& mesg);

private:
	boost::shared_ptr<DataLog> _data_log;
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
	bool parse(const QString& filename, boost::shared_ptr<DataLog> data_log);

 protected:
	bool parseRideDetails(boost::shared_ptr<DataLog> data_log);

 private:
	boost::scoped_ptr<fit::MesgBroadcaster> _mesg_broadcaster;
	boost::scoped_ptr<std::fstream> _file;
	boost::scoped_ptr<Listener> _listener;
 };

#endif // FITPARSER_H