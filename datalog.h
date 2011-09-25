#ifndef DATALOG_H
#define DATALOG_H

#include <qstring.h>
#include <vector>

class DataLog
 {
 public:
	DataLog();
	~DataLog();

	void resize(int size);

	QString& name() { return _name; };
	QString& date() { return _date; };
	double& totalTime() { return _total_time; };
	double& totalDist() { return _total_dist; };
	double& maxSpeed() { return _max_speed; };
	double& maxHeartRate() { return _max_heart_rate; };
	double& maxCadence() { return _max_cadence; };
	double& avgSpeed() { return _avg_speed; };
	double& avgHeartRate() { return _avg_heart_rate; };
	double& avgCadence() { return _avg_cadence; };
	int& numPoints() { return _num_points; };

	double& time(int idx);
	double& ltd(int idx);
	double& lgd(int idx);
	double& alt(int idx);
	double& dist(int idx);
	double& heartRate(int idx);
	double& cadence(int idx);
	double& speed(int idx);

 private:

	// Summary data
	QString _name;
	QString _date;
	double _total_time;
	double _total_dist;
	double _max_speed;
	double _max_heart_rate;
	double _max_cadence;
	double _avg_speed;
	double _avg_heart_rate;
	double _avg_cadence;
	int _num_points;

	// The vectors of data
	std::vector<double> _time;
	std::vector<double> _ltd;
	std::vector<double> _lgd;
	std::vector<double> _alt;
	std::vector<double> _dist;
	std::vector<double> _heart_rate;
	std::vector<double> _cadence;
	std::vector<double> _speed;
 };

#endif // DATALOG_H