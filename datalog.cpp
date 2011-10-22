#include "datalog.h"
#include <cassert>
#include <numeric>
#include <algorithm>
#include <iostream>

/****************************************/
DataLog::DataLog():
_filename(""),
_date(""),
_total_time(0.0),
_total_dist(0.0),
_max_speed(0.0),
_max_heart_rate(0.0),
_max_gradient(0.0),
_max_cadence(0.0),
_avg_speed(0.0),
_avg_heart_rate(0.0),
_avg_gradient(0.0),
_avg_cadence(0.0)
{
	// Initialise members
	resize(0);
}

/****************************************/
DataLog::~DataLog()
{

}

/****************************************/
double& DataLog::time(int idx) 
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _time[idx]; 
}

/****************************************/
double& DataLog::ltd(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _ltd[idx]; 
}

/****************************************/
double& DataLog::lgd(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _lgd[idx]; 
}

/****************************************/
double& DataLog::alt(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _alt[idx]; 
}

/****************************************/
double& DataLog::dist(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _dist[idx]; 
}

/****************************************/
double& DataLog::heartRate(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _heart_rate[idx]; 
}

/****************************************/
double& DataLog::cadence(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _cadence[idx]; 
}

/****************************************/
double& DataLog::speed(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _speed[idx]; 
}

/****************************************/
double& DataLog::gradient(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _gradient[idx]; 
}

/****************************************/
double& DataLog::power(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _power[idx]; 
}

/****************************************/
double& DataLog::altMap(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _alt_map[idx]; 
}

/****************************************/
double& DataLog::altSmooth(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _alt_smooth[idx]; 
}

/****************************************/
void DataLog::setNumLaps(int num_laps)
{
	_num_laps = num_laps;
}

/****************************************/
void DataLog::resize(int size)
{
	assert(size >= 0);

	_num_points = size;
	_num_laps = 0;
	_time.resize(size);
	_ltd.resize(size);
	_lgd.resize(size);
	_alt.resize(size);
	_dist.resize(size);
	_heart_rate.resize(size);
	_cadence.resize(size);
	_speed.resize(size);
	_gradient.resize(size);
	_power.resize(size);
	_alt_map.resize(size);

	_time_to_index.clear();
	_dist_to_index.clear();

	_time_valid = false;
	_ltd_valid = false;
	_lgd_valid = false;
	_alt_valid = false;
	_dist_valid = false;
	_heart_rate_valid = false;
	_cadence_valid = false;
	_speed_valid = false;
	_gradient_valid = false;
	_power_valid = false;
	_alt_map_valid = false;
	_alt_smooth_valid = false;
}

/****************************************/
void DataLog::computeMaps()
{
	// Time to index
	for (int i=0; i < numPoints(); ++i)
	{
		_time_to_index.insert(time(i), i);
	}

	// Time to distance
	for (int i=0; i < numPoints(); ++i)
	{
		_dist_to_index.insert(dist(i), i);
	}
}

/****************************************/
int DataLog::indexFromTime(double time)
{
	QMap<double,int>::iterator it = _time_to_index.lowerBound(std::max(time,0.0));
	if (it == _time_to_index.end())
		return numPoints()-1;
	else
		return it.value();
}

/****************************************/
int DataLog::indexFromDist(double dist)
{
	QMap<double,int>::iterator it = _dist_to_index.lowerBound(std::max(dist,0.0));
	if (it == _dist_to_index.end())
		return numPoints()-1;
	else
		return it.value();
}