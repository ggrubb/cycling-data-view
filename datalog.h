#ifndef DATALOG_H
#define DATALOG_H

#include <qstring.h>
#include <qmap.h>
#include <vector>

class DataLog
 {
 public:
	DataLog();
	~DataLog();

	void resize(int size);

	QString& filename() { return _filename; };
	QString& date() { return _date; };
	double& totalTime() { return _total_time; };
	double& totalDist() { return _total_dist; };
	double& maxSpeed() { return _max_speed; };
	double& maxHeartRate() { return _max_heart_rate; };
	double& maxGradient() { return _max_gradient; };
	double& maxCadence() { return _max_cadence; };
	double& maxPower() { return _max_power; };
	double& avgSpeed() { return _avg_speed; };
	double& avgHeartRate() { return _avg_heart_rate; };
	double& avgGradient() { return _avg_gradient; };
	double& avgCadence() { return _avg_cadence; };
	double& avgPower() { return _avg_power; };
	int numPoints() { return _num_points; };
	int numLaps() { return _lap_indecies.size(); };

	std::pair<int, int>& lap(int lap_index);
	void addLap(std::pair<int, int>& lap);

	double& time(int idx);
	double& ltd(int idx);
	double& lgd(int idx);
	double& alt(int idx);
	double& dist(int idx);
	double& heartRate(int idx);
	double& cadence(int idx);
	double& speed(int idx);
	double& gradient(int idx);
	double& power(int idx);

	double& altFltd(int idx);
	double& heartRateFltd(int idx);
	double& cadenceFltd(int idx);
	double& speedFltd(int idx);
	double& gradientFltd(int idx);
	double& powerFltd(int idx);

	std::vector<double>& time() { return _time; }
	std::vector<double>& ltd() { return _ltd; }
	std::vector<double>& lgd() { return _lgd; }
	std::vector<double>& alt() { return _alt; }
	std::vector<double>& dist() { return _dist; }
	std::vector<double>& heartRate() { return _heart_rate; }
	std::vector<double>& cadence() { return _cadence; }
	std::vector<double>& speed() { return _speed; }
	std::vector<double>& gradient() { return _gradient; }
	std::vector<double>& power() { return _power; }

	std::vector<double>& altFltd() { return _alt_fltd; }
	std::vector<double>& heartRateFltd() { return _heart_rate_fltd; }
	std::vector<double>& cadenceFltd() { return _cadence_fltd; }
	std::vector<double>& speedFltd() { return _speed_fltd; }
	std::vector<double>& gradientFltd() { return _gradient_fltd; }
	std::vector<double>& powerFltd() { return _power_fltd; }

	bool& timeValid() { return _time_valid; }
	bool& ltdValid() { return _ltd_valid; }
	bool& lgdValid() { return _lgd_valid; }
	bool& altValid() { return _alt_valid; }
	bool& distValid() { return _dist_valid; }
	bool& heartRateValid() { return _heart_rate_valid; }
	bool& cadenceValid() { return _cadence_valid; }
	bool& speedValid() { return _speed_valid; }
	bool& gradientValid() { return _gradient_valid; }
	bool& powerValid() { return _power_valid; }

	bool& altFltdValid() { return _alt_fltd_valid; }
	bool& heartRateFltdValid() { return _heart_rate_fltd_valid; }
	bool& cadenceFltdValid() { return _cadence_fltd_valid; }
	bool& speedFltdValid() { return _speed_fltd_valid; }
	bool& gradientFltdValid() { return _gradient_fltd_valid; }
	bool& powerFltdValid() { return _power_fltd_valid; }

	// Compute mappings from time to index and dist to index
	void computeMaps();
	// Return the index at the specified time
	int indexFromTime(double time);
	// Return the index at the specified distance
	int indexFromDist(double dist);

 private:

	// Summary data
	QString _filename;
	QString _date;
	double _total_time;
	double _total_dist;
	double _max_speed;
	double _max_heart_rate;
	double _max_gradient;
	double _max_cadence;
	double _max_power;
	double _avg_speed;
	double _avg_heart_rate;
	double _avg_gradient;
	double _avg_cadence;
	double _avg_power;
	int _num_points;

	// The vectors of data
	std::vector<double> _time; //sec
	std::vector<double> _ltd; //deg
	std::vector<double> _lgd; //deg
	std::vector<double> _alt; //m
	std::vector<double> _dist; //m
	std::vector<double> _heart_rate; //bpm
	std::vector<double> _cadence; //rpm
	std::vector<double> _speed; //kmh
	std::vector<double> _gradient; //%
	std::vector<double> _power; //W

	std::vector<double> _alt_fltd; //m
	std::vector<double> _heart_rate_fltd; //bpm
	std::vector<double> _cadence_fltd; //rpm
	std::vector<double> _speed_fltd; //kmh
	std::vector<double> _gradient_fltd; //%
	std::vector<double> _power_fltd; //W

	// Lap indexes (first = start index, second = end index)
	std::vector<std::pair<int, int> > _lap_indecies;

	// Maps from time to data index, and distance to index
	QMap<double,int> _time_to_index;
	QMap<double,int> _dist_to_index;

	// Flags to indicate which data vectors contain valid data
	bool _time_valid;
	bool _ltd_valid;
	bool _lgd_valid;
	bool _alt_valid;
	bool _dist_valid;
	bool _heart_rate_valid;
	bool _cadence_valid;
	bool _speed_valid;
	bool _gradient_valid;
	bool _power_valid;

	bool _alt_fltd_valid;
	bool _heart_rate_fltd_valid;
	bool _cadence_fltd_valid;
	bool _speed_fltd_valid;
	bool _gradient_fltd_valid;
	bool _power_fltd_valid;
 };

#endif // DATALOG_H