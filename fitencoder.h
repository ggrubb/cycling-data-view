#ifndef FITENCODER_H
#define FITENCODER_H

#include "baseparser.h"

#include <QDateTime.h>

#include "garminfitsdk/fit_mesg_broadcaster.hpp"

class DataLog;
class QString;

//***********************************************************
class FitEncoder 
{
 public:
	FitEncoder();
	~FitEncoder();

	// Encode data in log to fit file filename. Returns true if file was parsed successfully
	bool encode(const QString& filename, DataLog& data_log);

 private:
	double _pos_factor;
	QDateTime _base_date;
 };

#endif // FITENCODER_H