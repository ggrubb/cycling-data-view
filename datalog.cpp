#include "datalog.h"
#include <cassert>
#include <numeric>

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
	_gradient.resize(size);
	_power.resize(size);
	_alt_map.resize(size);
}

/****************************************/
void DataLog::computePower()
{

}

/****************************************/
void DataLog::computeGradient(
	const std::vector<double>& alt,
	const std::vector<double>& dist,
	std::vector<double>& grad)
{
	assert(alt.size() == dist.size());
	assert(alt.size() > 1);

	// Smooth altitude with averaging filter
	const int n = 10; // window size over which to smooth alt
	std::vector<double> alt_smoothed;
	alt_smoothed.resize(alt.size());
	std::vector<double>::const_iterator alt_it = alt.begin();
	for (int i=0; i < (int)alt.size(); ++i)
	{
		int x = i - std::max(0, i - n/2);
		int y = std::min((int)alt.size(), i + n/2) - i;
		alt_smoothed[i] = std::accumulate(alt_it+i-x, alt_it+i+y,0)/double(x + y);
		
	}

	// Compute gradient from smoothed altitude
	grad.resize(alt.size());
	for (uint i=1; i < alt_smoothed.size(); ++i)
	{
		if (dist[i] - dist[i-1] > 5)
			grad[i] = 100*(alt_smoothed[i] - alt_smoothed[i-1])/(dist[i] - dist[i-1]);
	}
	//grad = alt_smoothed;
}