#ifndef TCXPARSER_H
#define TCXPARSER_H

#include <qtxml/qdomdocument>

class DataLog;

class TcxParser
 {
 public:
	TcxParser();
	~TcxParser();

	// Parses data from .tcx in filename. Returns true if file was parsed successfully
	bool parse(const QString& filename, DataLog& data_log);

 private:
	void setDataValidFlags(DataLog& data_log);
	bool parseRideDetails(DataLog& data_log);
	void computeAdditionalDetailts(DataLog& data_log);

	QDomDocument _dom_document;
 };

#endif // TCXPARSER_H