#include "datalog.h"
#include <cassert>

/****************************************/
DataLog::DataLog():
_name(""),
_date(""),
_total_time(0.0),
_total_dist(0.0),
_max_speed(0.0),
_max_heart_rate(0.0),
_max_cadence(0.0),
_avg_speed(0.0),
_avg_heart_rate(0.0),
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

//****************************************/
void DataLog::resize(int size)
{
	assert(size >= 0);

	_num_points = size;
	_time.resize(size);
	_ltd.resize(size);
	_lgd.resize(size);
	_alt.resize(size);
	_dist.resize(size);
	_heart_rate.resize(size);
	_cadence.resize(size);
	_speed.resize(size);
}