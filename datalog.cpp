#include "datalog.h"
#include <cassert>
#include <numeric>
#include <algorithm>

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
double& DataLog::altSmooth(int idx)
{ 
	assert(idx >= 0); 
	assert(idx < _num_points);
	return _alt_smooth[idx]; 
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

	_time_to_index.clear();
	_dist_to_index.clear();

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
	if (it == _time_to_index.end())
		return numPoints()-1;
	else
		return it.value();
}

/****************************************/
void DataLog::computePower()
{

}

/****************************************/
void DataLog::smoothSignal(
	const std::vector<double>& signal,
	std::vector<double>& smoothed,
	int window_size)
{
	assert(signal.size() > 1);

	// Smooth altitude with averaging filter
	smoothed.resize(signal.size());
	std::vector<double>::const_iterator signal_it = signal.begin();
	for (int i=0; i < (int)signal.size(); ++i)
	{
		int x = i - std::max(0, i - window_size/2);
		int y = std::min((int)signal.size(), i + window_size/2) - i;
		smoothed[i] = std::accumulate(signal_it+i-x, signal_it+i+y,0)/double(x + y);
		
	}
}

/****************************************/
void DataLog::computeGradient(
	const std::vector<double>& alt,
	const std::vector<double>& dist,
	std::vector<double>& grad)
{
	assert(alt.size() == dist.size());
	assert(alt.size() > 1);

	// Compute gradient from altitude
	grad.resize(alt.size());
	for (uint i=1; i < alt.size(); ++i)
	{
		if (dist[i] - dist[i-1] > 5)
			grad[i] = 100*(alt[i] - alt[i-1])/(dist[i] - dist[i-1]);
	}
}

/****************************************/
void DataLog::computeSpeed(
	const std::vector<double>& time,
	const std::vector<double>& dist,
	std::vector<double>& speed)
{
	assert(time.size() == dist.size());
	assert(time.size() > 2);

	// Compute speed from distance over time
	speed.resize(time.size());
	for (uint i=2; i < time.size(); ++i)
	{
		if (time[i] - time[i-2] > 0)
			speed[i] = 3.6*(dist[i] - dist[i-2])/(time[i] - time[i-2]);
	}
}

/****************************************/
double DataLog::computeAverage(
	std::vector<double>::const_iterator& start,
	std::vector<double>::const_iterator& end)
{
	double avg = 0;
	if (end-start > 0)
		avg = std::accumulate(start, end,0)/(end-start);
	return avg;
}

/****************************************/
double DataLog::computeMax(
	std::vector<double>::const_iterator& start,
	std::vector<double>::const_iterator& end)
{
	std::vector<double>::const_iterator max_it;
	max_it = std::max_element(start, end);
	return *max_it;
}

/****************************************/
double DataLog::computeGain(
	std::vector<double>::const_iterator& start,
	std::vector<double>::const_iterator& end)
{
	double gain = 0;
	for (std::vector<double>::const_iterator it=++start; it != end; ++it)
	{
		double val1 = *it;
		double val2 = *(it-1);
		if (val1 > val2)
			gain += val1 - val2;
	}
	return gain;
}

/****************************************/
double DataLog::computeLoss(
	std::vector<double>::const_iterator& start,
	std::vector<double>::const_iterator& end)
{
	double loss = 0;
	for (std::vector<double>::const_iterator it=++start; it != end; ++it)
	{
		double val1 = *it;
		double val2 = *(it-1);
		if (val1 < val2)
			loss += val2 - val1;
	}
	return loss;
}

