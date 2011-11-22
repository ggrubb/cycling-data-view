#include "dataprocessing.h"
#include <cassert>
#include <numeric>
#include <algorithm>
#include <iostream>

/****************************************/
void DataProcessing::computePower()
{

}

/****************************************/
double DataProcessing::computeTimeInHRZone(
	const std::vector<double>& hr,
	const std::vector<double>& time,
	double min_hr,
	double max_hr)
{
	assert(hr.size() == time.size());
	assert(hr.size() > 1);

	double time_in_zone = 0.0;
	for (unsigned int i=1; i < hr.size(); ++i)
	{
		if (hr[i] >= min_hr && hr[i] < max_hr)
		{
			time_in_zone += time[i] - time[i-1];
		}
	}
	return time_in_zone;
}

/****************************************/
void DataProcessing::lowPassFilterSignal(
	const std::vector<double>& signal,
	std::vector<double>& filtered,
	int window_size)
{
	assert(signal.size() > 1);

	filtered.resize(signal.size());
	if (window_size > 2)
	{
		// Smooth altitude with averaging filter
		std::vector<double>::const_iterator signal_it = signal.begin();
		for (int i=0; i < (int)signal.size(); ++i)
		{
			int x = i - std::max(0, i - window_size/2);
			int y = std::min((int)signal.size(), i + window_size/2) - i;
			filtered[i] = std::accumulate(signal_it+i-x, signal_it+i+y,0.0)/double(x + y);	
		}
	}
	else
	{
		std::copy(signal.begin(),signal.end(), filtered.begin());
	}
}

/****************************************/
void DataProcessing::computeGradient(
	const std::vector<double>& alt,
	const std::vector<double>& dist,
	std::vector<double>& grad)
{
	assert(alt.size() == dist.size());
	assert(alt.size() > 1);

	// Compute gradient from altitude
	grad.resize(alt.size());
	for (unsigned int i=1; i < alt.size(); ++i)
	{
		if (dist[i] - dist[i-1] > 5)
			grad[i] = 100*(alt[i] - alt[i-1])/(dist[i] - dist[i-1]);
	}
}

/****************************************/
void DataProcessing::computeSpeed(
	const std::vector<double>& time,
	const std::vector<double>& dist,
	std::vector<double>& speed)
{
	assert(time.size() == dist.size());
	assert(time.size() > 2);

	// Compute speed from distance over time
	speed.resize(time.size());
	for (unsigned int i=2; i < time.size(); ++i)
	{
		if (time[i] - time[i-2] > 0)
			speed[i] = 3.6*(dist[i] - dist[i-2])/(time[i] - time[i-2]);
	}
}

/****************************************/
double DataProcessing::computeAverage(
	std::vector<double>::const_iterator& start,
	std::vector<double>::const_iterator& end)
{
	double avg = 0;
	if (end-start > 0)
		avg = std::accumulate(start, end,0.0)/double(end-start);
	return avg;
}

/****************************************/
double DataProcessing::computeNthPercentile(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end,
		double N)
{
	assert(N > 0.0 && N < 1.0);

	double nth_percentile = 0.0;
	if (end-start > 0)
	{
		std::vector<double> sorted(end-start);
		std::copy(start, end, sorted.begin());
		std::sort(sorted.begin(), sorted.end());
		nth_percentile = *(sorted.begin() + (int)(N*(double)sorted.size()) );
	}
	return nth_percentile;
}
/****************************************/
double DataProcessing::computeMax(
	std::vector<double>::const_iterator& start,
	std::vector<double>::const_iterator& end)
{
	return *std::max_element(start, end);
}

/****************************************/
double DataProcessing::computeMin(
	std::vector<double>::const_iterator& start,
	std::vector<double>::const_iterator& end)
{
	return *std::min_element(start, end);
}

/****************************************/
double DataProcessing::computeGain(
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
double DataProcessing::computeLoss(
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

/****************************************/
const QString DataProcessing::minsFromSecs(int seconds)
{
	int mins  = seconds/60;
	int secs = seconds%60;

	if (secs < 10)
		return QString::number(mins) + ":0" + QString::number(secs);
	else
		return QString::number(mins) + ":" + QString::number(secs);
}

/****************************************/
const QString DataProcessing::kmFromMeters(double meters, int prec)
{
	return QString::number(meters/1000.0,'f',prec);
}
