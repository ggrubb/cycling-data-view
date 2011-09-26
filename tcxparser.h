#ifndef TCXPARSER_H
#define TCXPARSER_H

#include <qtxml/qdomdocument>

class DataLog;

class TcxParser
 {
 public:
	TcxParser();
	~TcxParser();

	// Returns true if file was parsed
	bool parse(const QString& flename, DataLog& data_log);

 private:

	 void parseRideDetails(DataLog& data_log);
	 void parseRideSummary(DataLog& data_log);

	 QDomDocument _dom_document;
 };

#endif // TCXPARSER_H