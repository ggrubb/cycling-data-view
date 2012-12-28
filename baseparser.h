#ifndef BASEPARSER_H
#define BASEPARSER_H

#include <boost/shared_ptr.hpp>

class DataLog;
class QString;

/* Base class for all data file parsers */
class BaseParser
 {
 public:
	BaseParser();
	~BaseParser();

	// Parses data from filename. Returns true if file was parsed successfully
	virtual bool parse(const QString& filename, boost::shared_ptr<DataLog> data_log) = 0;

	static void setDataValidFlags(DataLog& data_log);
	static void computeAdditionalDetailts(DataLog& data_log);
 protected:
	virtual bool parseRideDetails(boost::shared_ptr<DataLog> data_log) = 0;
	
 };

#endif // BASEPARSER_H