#include "tcxparser.h"
#include "datalog.h"
#include <QStringList.h>
#include <QFile.h>

/******************************************************/
TcxParser::TcxParser()
{

}

/******************************************************/
TcxParser::~TcxParser()
{

}

/******************************************************/
void TcxParser::parseRideSummary(DataLog& data_log)
{
	QDomElement doc = _dom_document.documentElement();

	QDomElement lap = doc.firstChild().firstChild().firstChildElement("Lap");
	QDomElement	total_time_seconds = lap.firstChildElement("TotalTimeSeconds");
	QDomElement	distance_meters = lap.firstChildElement("DistanceMeters");
	QDomElement	max_speed = lap.firstChildElement("MaximumSpeed");
	QDomNode	max_heart_rate = lap.firstChildElement("MaximumHeartRateBpm").firstChild();
	QDomNode	max_cadence = lap.firstChildElement("Extensions").firstChild().firstChild().nextSibling();
	QDomNode	avg_heart_rate = lap.firstChildElement("AverageHeartRateBpm").firstChild();
	QDomNode	avg_speed = lap.firstChildElement("Extensions").firstChild().firstChild();
	QDomElement	avg_cadence = lap.firstChildElement("Cadence");

	data_log.date() = lap.attributes().item(0).nodeValue();
	data_log.totalTime() = total_time_seconds.firstChild().nodeValue().toFloat();
	data_log.totalDist() = distance_meters.firstChild().nodeValue().toFloat();
	data_log.avgCadence() = avg_cadence.firstChild().nodeValue().toFloat();
	data_log.avgHeartRate() = avg_heart_rate.firstChild().nodeValue().toFloat();
	data_log.avgSpeed() = avg_speed.firstChild().nodeValue().toFloat();
	data_log.maxSpeed() = max_speed.firstChild().nodeValue().toFloat();
	data_log.maxHeartRate() = max_heart_rate.firstChild().nodeValue().toFloat();
	data_log.maxCadence() = 0;

}

/******************************************************/
void TcxParser::parseRideDetails(DataLog& data_log)
{
	QDomElement doc = _dom_document.documentElement();

	QDomNode track = doc.firstChild().firstChild().firstChildElement("Lap").firstChildElement("Track");
	
	int track_point_idx = 0;
	int total_track_points = 0;
	int num_empty_track_points = 0;
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
				data_log.speed(track_point_idx) = track_point.firstChildElement("Extensions").firstChild().firstChild().nodeValue().toFloat();
				data_log.lgd(track_point_idx) = track_point.firstChildElement("Position").firstChildElement("LongitudeDegrees").firstChild().nodeValue().toDouble();//.toFloat();
				data_log.ltd(track_point_idx) = track_point.firstChildElement("Position").firstChildElement("LatitudeDegrees").firstChild().nodeValue().toDouble();//.toFloat();
				data_log.heartRate(track_point_idx) = track_point.firstChildElement("HeartRateBpm").firstChild().firstChild().nodeValue().toFloat();
				data_log.dist(track_point_idx) = track_point.firstChildElement("DistanceMeters").firstChild().nodeValue().toDouble();//.toFloat();
				data_log.cadence(track_point_idx) = track_point.firstChildElement("Cadence").firstChild().nodeValue().toFloat();
				data_log.alt(track_point_idx) = track_point.firstChildElement("AltitudeMeters").firstChild().nodeValue().toDouble();//.toFloat();
			}
			track_point = track_point.nextSibling();

			// Sometimes the xml contains empty trackpoint nodes, with just a time, but no data.
			// Here we check this, and don't increment counter if the trackpoint was empty
			bool valid_track_point = true;
			if (data_log.lgd(track_point_idx) == 0 && data_log.ltd(track_point_idx) == 0)
			{
				valid_track_point = false;
				num_empty_track_points++;
			}

			if (valid_track_point)
				track_point_idx++;
		}

		track = track.nextSibling();
	}

	// Resize to account for empty trackpoints
	total_track_points -= num_empty_track_points;
	data_log.resize(total_track_points);

	// Clean up the ride time
	for (int i=data_log.numPoints()-1; i >= 0; --i)
	{
		data_log.time(i) = data_log.time(i) - data_log.time(0);
	}

}

/******************************************************/
void TcxParser::computeAdditionalDetailts(DataLog& data_log)
{
	DataLog::smoothSignal(data_log.alt(), data_log.altSmooth());
	DataLog::computeGradient(data_log.altSmooth(), data_log.dist(), data_log.gradient());
	if (true)
		DataLog::computeSpeed(data_log.time(), data_log.dist(), data_log.speed());
}

/******************************************************/
bool TcxParser::parse(const QString& flename, DataLog& data_log)
{
	// Define the file to read
	QString error_msg;
	int error_line, error_column;
	QFile file(flename);
	bool read_success = _dom_document.setContent(&file, &error_msg, &error_line, &error_column);
	QDomElement doc = _dom_document.documentElement();

	// Extract the data
	data_log.name() = flename;
	if (read_success)
	{
		parseRideSummary(data_log);
		parseRideDetails(data_log);
		computeAdditionalDetailts(data_log);
	}

	return read_success;
}