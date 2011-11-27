#ifndef TCXPARSER_H
#define TCXPARSER_H

#include "baseparser.h"

#include <qtxml/qdomdocument>

class DataLog;

class TcxParser : public BaseParser
 {
 public:
	TcxParser();
	~TcxParser();

	// Parses data from .tcx in filename. Returns true if file was parsed successfully
	bool parse(const QString& filename, DataLog& data_log);

 protected:
	bool parseRideDetails(DataLog& data_log);

 private:
	QDomDocument _dom_document;
 };

#endif // TCXPARSER_H