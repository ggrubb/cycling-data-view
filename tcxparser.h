#ifndef TCXPARSER_H
#define TCXPARSER_H

#include "baseparser.h"

#include <qtxml/qdomdocument>

#include <boost/shared_ptr.hpp>

class DataLog;

class TcxParser : public BaseParser
 {
 public:
	TcxParser();
	~TcxParser();

	// Parses data from .tcx in filename. Returns true if file was parsed successfully
	bool parse(const QString& filename, boost::shared_ptr<DataLog> data_log);

 protected:
	bool parseRideDetails(boost::shared_ptr<DataLog> data_log);

 private:
	QDomDocument _dom_document;
 };

#endif // TCXPARSER_H