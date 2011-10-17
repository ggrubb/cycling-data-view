#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <vector>

class DataProcessing
 {
 public:
	static void computePower();

	static void computeGradient(
		const std::vector<double>& alt,
		const std::vector<double>& dist,
		std::vector<double>& grad);

	static void computeSpeed(
		const std::vector<double>& time,
		const std::vector<double>& dist,
		std::vector<double>& speed);

	static void lowPassFilterSignal(
		const std::vector<double>& signal,
		std::vector<double>& filtered,
		int window_size = 10);

	static double computeAverage(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);

	static double computeMax(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);

	static double computeGain(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);

	static double computeLoss(
		std::vector<double>::const_iterator& start,
		std::vector<double>::const_iterator& end);
 };

#endif // DATAPROCESSING_H