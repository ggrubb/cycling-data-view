#include "tcxparser.h"
#include "datalog.h"
#include "dataprocessing.h"

#include <QStringList.h>
#include <QFile.h>
#include <iostream>

/******************************************************/
TcxParser::TcxParser()
{}

/******************************************************/
TcxParser::~TcxParser()
{}

/******************************************************/
void TcxParser::parseRideDetails(DataLog& data_log)
{
	QDomElement doc = _dom_document.documentElement();

	QDomNode lap = doc.firstChild().firstChild().firstChildElement("Lap");
	
	int track_point_idx = 0;
	int total_track_points = 0;
	int num_empty_track_points = 0;

	int lap_start_idx = 0;
	int lap_end_idx = 0;
	while (!lap.isNull())
	{
		QDomNode track = lap.firstChildElement("Track");
		while (!track.isNull())
		{
			QDomNode track_point = track.firstChild();

			// Count the number of track points to allocate sufficient space
			int num_track_pts = 0;
			while (!track_point.isNull())
			{
				num_track_pts++;
				track_point = track_point.nextSibling();
			}
			total_track_points += num_track_pts;

			// Allocate space
			data_log.resize(total_track_points);

			// Now extract all the data
			track_point = track.firstChild();
			for (int i=0; i < num_track_pts; ++i)
			{
				QStringList tmp_sl = track_point.firstChildElement("Time").firstChild().nodeValue().split('T');
				if (tmp_sl.size() > 1) // check to ensure the time format is as expected
				{
					QString tmp_s = tmp_sl.at(1);
					tmp_s.chop(1);
					QStringList time_strings = tmp_s.split(':');
					data_log.time(track_point_idx) = time_strings.at(0).toInt()*3600 + time_strings.at(1).toInt()*60 + time_strings.at(2).toInt();
					data_log.speed(track_point_idx) = track_point.firstChildElement("Extensions").firstChild().firstChild().nodeValue().toDouble();
					data_log.lgd(track_point_idx) = track_point.firstChildElement("Position").firstChildElement("LongitudeDegrees").firstChild().nodeValue().toDouble();
					data_log.ltd(track_point_idx) = track_point.firstChildElement("Position").firstChildElement("LatitudeDegrees").firstChild().nodeValue().toDouble();
					data_log.heartRate(track_point_idx) = track_point.firstChildElement("HeartRateBpm").firstChild().firstChild().nodeValue().toDouble();
					data_log.dist(track_point_idx) = track_point.firstChildElement("DistanceMeters").firstChild().nodeValue().toDouble();
					data_log.cadence(track_point_idx) = track_point.firstChildElement("Cadence").firstChild().nodeValue().toDouble();
					data_log.alt(track_point_idx) = track_point.firstChildElement("AltitudeMeters").firstChild().nodeValue().toDouble();
				}
				track_point = track_point.nextSibling();

				// Sometimes the xml contains empty trackpoint nodes, with just a time, but no data.
				// Here we check this, and don't increment counter if the trackpoint was empty
				bool valid_track_point = true;
				if (data_log.lgd(track_point_idx) == 0 && data_log.ltd(track_point_idx) == 0 && 
					data_log.dist(track_point_idx) == 0)
				{
					valid_track_point = false;
					num_empty_track_points++;
				}

				// Sometimes the xml contains a track point with no GPS. Here we check this and re-use
				// the previous GPS if the dropout was only one sample.
				if (data_log.lgd(track_point_idx) == 0 && data_log.ltd(track_point_idx) == 0 && 
					data_log.dist(track_point_idx) != 0)
				{
					if (track_point_idx > 0) 
					{
						if (data_log.lgd(track_point_idx-1) != 0 && data_log.ltd(track_point_idx-1) != 0) // recover by using prev pos
						{
							data_log.lgd(track_point_idx) = data_log.lgd(track_point_idx-1);
							data_log.ltd(track_point_idx) = data_log.ltd(track_point_idx-1);
						}
						else // give up on this point
						{
							valid_track_point = false;
							num_empty_track_points++;
						}
					}
					else // give up on this point
					{
						valid_track_point = false;
						num_empty_track_points++;
					}
				}

				if (valid_track_point)
					track_point_idx++;
			}

			track = track.nextSibling();
		}
		lap = lap.nextSibling();

		if (lap_start_idx <  track_point_idx-1)
		{
			lap_end_idx = track_point_idx-1;
			data_log.addLap(std::make_pair(lap_start_idx, lap_end_idx));
			lap_start_idx = lap_end_idx;
		}
	}

	// Resize to account for empty trackpoints
	total_track_points -= num_empty_track_points;
	data_log.resize(total_track_points);

	// Clean up the ride time
	for (int i=data_log.numPoints()-1; i >= 0; --i)
	{
		data_log.time(i) = data_log.time(i) - data_log.time(0);
	}

	// Set flags to indicate vailidity of data read
	setDataValidFlags(data_log);
}

/******************************************************/
void TcxParser::setDataValidFlags(DataLog& data_log)
{
	for (int i=0; i < data_log.numPoints(); ++i)
	{
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
	}

}

/******************************************************/
void TcxParser::computeAdditionalDetailts(DataLog& data_log)
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

/******************************************************/
bool TcxParser::parse(const QString& filename, DataLog& data_log)
{
	bool read_success = false;

	// Define the file to read
	QFile file(filename);

	QString error_msg;
	int error_line, error_column;
	read_success = _dom_document.setContent(&file, &error_msg, &error_line, &error_column);
	
	// Extract the data
	if (read_success)
	{
		data_log.filename() = filename;
		parseRideDetails(data_log);
		computeAdditionalDetailts(data_log);
		data_log.computeMaps();
	}

	return read_success;
}