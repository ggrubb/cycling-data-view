#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <QString.h>
#include <vector>

namespace DataProcessing
{
	void computePower();

	double computeTimeInHRZone(
		const std::vector<double>& hr,
		const std::vector<double>& time,
		double min_hr,
		double max_hr);

	void lowPassFilterSignal(
		const std::vector<double>& signal,
		std::vector<double>& filtered,
		int window_size = 10);

	void computeGradient(
		const std::vector<double>& alt,
		const std::vector<double>& dist,
		std::vector<double>& grad);

	void computeSpeed(
		const std::vector<double>& time,
		const std::vector<double>& dist,
		std::vector<double>& speed);

	double computeAverage(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);

	double computeNthPercentile(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end,
		double N);

	double computeMax(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);

	double computeMin(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);

	double computeGain(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);

	double computeLoss(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);

	// Some functions to convert data to text
	const QString minsFromSecs(int seconds);
	const QString kmFromMeters(double meters);
 };

#endif // DATAPROCESSING_H