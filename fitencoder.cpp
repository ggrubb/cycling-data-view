#include "fitencoder.h"
#include "datalog.h"
#include "dataprocessing.h"

#include <iostream>
#include <cassert>
#include <math.h>
#include <fstream>

#include "garminfitsdk/fit_encode.hpp"

/******************************************************/
FitEncoder::FitEncoder()
{
	_pos_factor = 180.0 / pow(2.0,31.0); // degrees = semicircles * ( 180 / 2^31 )
	_base_date = QDateTime(QDate(1989,12,31)); // start date of all .fit dates
}

/******************************************************/
FitEncoder::~FitEncoder()
{}

/******************************************************/
bool FitEncoder::encode(const QString& filename, DataLog& data_log)
{
	// Define the file to read
	std::fstream file;
    file.open(filename.toStdString().c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc );
	
	if (!file.is_open())
	{
		return false;
	}

	fit::Encode encode;
	encode.Open(file);

	const int secs_offset = _base_date.secsTo(data_log.date());
	for (int i=0; i < data_log.numPoints(); ++i)
	{
		fit::RecordMesg msg;
		
		msg.SetTimestamp(data_log.time(i) + secs_offset);

		msg.SetPositionLat(data_log.ltd(i)/_pos_factor);
		msg.SetPositionLong(data_log.lgd(i)/_pos_factor);
		msg.SetAltitude(data_log.alt(i));
		msg.SetHeartRate(data_log.heartRate(i));
		msg.SetCadence(data_log.cadence(i));
		msg.SetDistance(data_log.dist(i));
		msg.SetSpeed(data_log.speed(i)/3.6);
		msg.SetPower(data_log.power(i));
		msg.SetTemperature(data_log.temp(i));

		encode.Write(msg);
	}

	for (int i=0; i < data_log.numLaps(); ++i)
	{
		fit::LapMesg msg;
		msg.SetStartTime(data_log.lap(i).first + secs_offset);
		msg.SetTimestamp(data_log.lap(i).second + secs_offset);

		encode.Write(msg);
	}

	if (!encode.Close())
	{
	  return false;
	}

	file.close();
	return true;
}