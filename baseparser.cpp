#include "baseparser.h"
#include "datalog.h"
#include "dataprocessing.h"

#include <fstream>
#include <iostream>
#include <cassert>
#include <math.h>

#include <QDateTime.h>

/******************************************************/
BaseParser::BaseParser()
{}

/******************************************************/
BaseParser::~BaseParser()
{}

/******************************************************/
void BaseParser::setDataValidFlags(DataLog& data_log)
{
	for (int i=0; i < data_log.numPoints(); ++i)
	{
		if (data_log.time(i) != 0.0)
		{
			data_log.timeValid() = true;
		}

		if (data_log.ltd(i) != 0.0 || data_log.lgd(i) != 0.0)
		{
			data_log.lgdValid() = true;
			data_log.ltdValid() = true;
		}

		if (data_log.alt(i) != 0.0)
		{
			data_log.altValid() = true;
		}

		if (data_log.speed(i) != 0.0)
		{
			data_log.speedValid() = true;
		}

		if (data_log.heartRate(i) != 0.0)
		{
			data_log.heartRateValid() = true;
		}

		if (data_log.cadence(i) != 0.0)
		{
			data_log.cadenceValid() = true;
		}

		if (data_log.dist(i) != 0.0)
		{
			data_log.distValid() = true;
		}

		if (data_log.temp(i) != 0.0)
		{
			data_log.tempValid() = true;
		}
	}

}

/******************************************************/
void BaseParser::computeAdditionalDetailts(DataLog& data_log)
{
	// Compute grad from smoothed gradient
	if (data_log.altValid())
	{
		DataProcessing::lowPassFilterSignal(data_log.alt(), data_log.altFltd());
		data_log.altFltdValid() = true;
		DataProcessing::computeGradient(data_log.altFltd(), data_log.dist(), data_log.gradient());
		data_log.gradientValid() = true;
	}
	
	// Compute speed if not already measured
	if (!data_log.speedValid())
	{
		DataProcessing::computeSpeed(data_log.time(), data_log.dist(), data_log.speed());
		data_log.speedValid() = true;
	}
	
	// Compute max and avg of all signals
	data_log.avgSpeed() = DataProcessing::computeAverage(data_log.speed().begin(), data_log.speed().end());
	data_log.avgHeartRate() = DataProcessing::computeAverage(data_log.heartRate().begin(), data_log.heartRate().end());
	data_log.avgGradient() = DataProcessing::computeAverage(data_log.gradient().begin(), data_log.gradient().end());
	data_log.avgCadence() = DataProcessing::computeAverage(data_log.cadence().begin(), data_log.cadence().end());
	data_log.avgPower() = DataProcessing::computeAverage(data_log.power().begin(), data_log.power().end());

	data_log.maxSpeed() = DataProcessing::computeMax(data_log.speed().begin(), data_log.speed().end());
	data_log.maxHeartRate() = DataProcessing::computeMax(data_log.heartRate().begin(), data_log.heartRate().end());
	data_log.maxGradient() = DataProcessing::computeMax(data_log.gradient().begin(), data_log.gradient().end());
	data_log.maxCadence() = DataProcessing::computeMax(data_log.cadence().begin(), data_log.cadence().end());
	data_log.maxPower() = DataProcessing::computeMax(data_log.power().begin(), data_log.power().end());

	// Totals
	data_log.totalTime() = data_log.time(data_log.numPoints()-1);
	data_log.totalDist() = data_log.dist(data_log.numPoints()-1);
}