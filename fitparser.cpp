#include "fitparser.h"
#include "datalog.h"
#include "dataprocessing.h"

#include <iostream>
#include <cassert>
#include <math.h>

#include "garminfitsdk/fit_decode.hpp"

/******************************************************/
Listener::Listener(DataLog* data_log):
	_data_log(data_log),
	_track_point_index(0),
	_start_time(0)
{
	_pos_factor = 180.0 / pow(2.0,31.0); // degrees = semicircles * ( 180 / 2^31 )
	_base_date = QDateTime(QDate(1989,12,31)); // start date of all .fit dates
}

/******************************************************/
int Listener::numPointsRead()
{
	return _track_point_index; 
}

/******************************************************/
void Listener::OnMesg(fit::RecordMesg& mesg)
{
	if (mesg.GetTimestamp() != FIT_DATE_TIME_INVALID)
	{
		QDateTime date_time = _base_date.addSecs((int)mesg.GetTimestamp());
		if (_track_point_index == 0)
		{
			const int max_size = 40000; // 11.1 hrs, 1 sample per sec
			_data_log->resize(max_size);
			_data_log->date() = _base_date.addSecs((int)mesg.GetTimestamp());
			_start_time = (int)mesg.GetTimestamp();
		}
		_data_log->time(_track_point_index) = (int)mesg.GetTimestamp() - _start_time;
	}
	if (mesg.GetPositionLat() != FIT_SINT32_INVALID)
		_data_log->ltd(_track_point_index) = mesg.GetPositionLat()*_pos_factor;
	
	if (mesg.GetPositionLong() != FIT_SINT32_INVALID)
		_data_log->lgd(_track_point_index) = mesg.GetPositionLong()*_pos_factor;
	
	if (mesg.GetAltitude() != FIT_UINT8_INVALID)
		_data_log->alt(_track_point_index) = (float)mesg.GetAltitude();
		
	if (mesg.GetHeartRate() != FIT_UINT8_INVALID)
		_data_log->heartRate(_track_point_index) = (int)mesg.GetHeartRate();

	if (mesg.GetCadence() != FIT_UINT8_INVALID)
		_data_log->cadence(_track_point_index) = (int)mesg.GetCadence();

	if (mesg.GetDistance() != FIT_FLOAT32_INVALID)
		_data_log->dist(_track_point_index) = (double)mesg.GetDistance();

	if (mesg.GetSpeed() != FIT_FLOAT32_INVALID)
		_data_log->speed(_track_point_index) = (double)mesg.GetSpeed()*3.6;

	if (mesg.GetPower() != FIT_UINT16_INVALID)
		_data_log->power(_track_point_index) = (int)mesg.GetPower();

	if (mesg.GetTemperature() != FIT_SINT8_INVALID)
		_data_log->temp(_track_point_index) = (int)mesg.GetTemperature();

	_track_point_index++;
}

/******************************************************/
void Listener::OnMesg(fit::LapMesg& mesg)
{
	int lap_start_time = 0;
	int lap_end_time = 0;
	if (mesg.GetTimestamp() != FIT_DATE_TIME_INVALID) // end time
	{
		lap_end_time = (int)mesg.GetTimestamp() - _start_time;
	}

	if (mesg.GetStartTime() != FIT_DATE_TIME_INVALID) // start time
	{
		lap_start_time = (int)mesg.GetStartTime() - _start_time + 1;
	}

	_data_log->addLap(std::make_pair(lap_start_time, lap_end_time));
}

/******************************************************/
FitParser::FitParser()
{}

/******************************************************/
FitParser::~FitParser()
{}

/******************************************************/
bool FitParser::parseRideDetails(DataLog& data_log)
{
	try
	{
		// Parse with Garmin fit SDK
		_mesg_broadcaster->Run(*_file);

		// Perform some clean up on the data

		// Cull unused points
		data_log.resize(_listener->numPointsRead());
		data_log.computeMaps();

		// Convert laps from time to index
		for (int i=0; i < data_log.numLaps(); ++i)
		{
			data_log.lap(i).first = data_log.indexFromTime(data_log.lap(i).first);
			data_log.lap(i).second = data_log.indexFromTime(data_log.lap(i).second);
		}

		// Clean up sporadic empty GPS points
		for (int i=1; i < data_log.numPoints(); ++i)
		{
			if (data_log.lgd(i) == 0 && data_log.ltd(i) == 0 && data_log.dist(i) != 0)
			{
				data_log.lgd(i) = data_log.lgd(i-1);
				data_log.ltd(i) = data_log.ltd(i-1);
			}
		}

		setDataValidFlags(data_log);

		return true;
	}
	catch (const fit::RuntimeException&)
	{
		return false;
	}
}

/******************************************************/
bool FitParser::parse(const QString& filename, DataLog& data_log)
{
	bool read_success = false;

	// Define the file to read
	_file = new std::fstream;
    _file->open(filename.toStdString().c_str(), std::ios::in | std::ios::binary);
	
	fit::Decode decode;
	read_success = _file->is_open() && decode.CheckIntegrity(*_file);
	
	_mesg_broadcaster = new fit::MesgBroadcaster;
	_listener = new Listener(&data_log);
	_mesg_broadcaster->AddListener((fit::RecordMesgListener &)*_listener);
	_mesg_broadcaster->AddListener((fit::LapMesgListener &)*_listener);
	
	// Extract the data
	if (read_success)
	{
		data_log.filename() = filename;
		read_success = parseRideDetails(data_log);
		if (read_success)
		{
			computeAdditionalDetailts(data_log);
		}
	}

	return read_success;
}