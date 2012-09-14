#ifndef LATLNG_H
#define LATLNG_H

#include <math.h>
#include <iostream>

#define M_PI 3.14159265358979323846
#define DEFAULT_DIST_THRESHOLD 300.0 // metres for comparison operator
#define DEFAULT_APPROX_DIST_THRESHOLD 0.007 // lat/long approx

// Various equations for lat/long calculations come from the follow reference
// http://www.movable-type.co.uk/scripts/latlong.html

class LatLng
{
public:
	LatLng(
		double lat, 
		double lng, 
		double dist_threshold = DEFAULT_DIST_THRESHOLD,
		double approx_dist_threshold = DEFAULT_APPROX_DIST_THRESHOLD
		):
	  _lat(lat),
	  _lng(lng),
	  _dist_threshld(dist_threshold),
	  _approx_dist_threshold(approx_dist_threshold)
	{}
	LatLng(){}

	// Threshold for accurate equality estimate (fast)
	void setDistThreshold(double thshld)
	{
		_dist_threshld = thshld;
	};

	// Threshold for approximate equality estimate (slow)
	void setApproxDistThreshold(double thshld)
	{
		_approx_dist_threshold = thshld;
	};

	double _lat;
	double _lng;

	double angle(const LatLng& other) const
	{
		// lat2 = other
		// lat1 = this
		const double lat1 = _lat*M_PI/180.0;
		const double lat2 = other._lat*M_PI/180.0;
		const double d_lon = (other._lng-_lng)*M_PI/180.0;

		const double y = sin(d_lon) * cos(lat2);
		const double x = cos(lat1)* sin(lat2) - sin(lat1)*cos(lat2)*cos(d_lon);

		return  atan2(y, x)*180.0/M_PI;
	}

	double dist(const LatLng& other) const
	{
		// lat2 = other
		// lat1 = this
		const double R = 6371000.0; // m, radius of the earth
		const double d_lat = (other._lat-_lat)*M_PI/180.0;
		const double d_lon = (other._lng-_lng)*M_PI/180.0;
		const double lat1 = _lat*M_PI/180.0;
		const double lat2 = other._lat*M_PI/180.0;

		const double a = sin(d_lat/2.0) * sin(d_lat/2.0) +
				sin(d_lon/2.0) * sin(d_lon/2.0) * cos(lat1) * cos(lat2); 
		const double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a)); 
		const double dist = R * c;

		//std::cout << "this: " << lat << ", " << lng << " other: " << other.lat << ", " << other.lng << " dist: " << dist << std::endl;

		return dist;
	}

	// Determine if the points are approximately equal, but quickly
	bool approxEqual(const LatLng& other) const
	{
		if ( (abs(other._lat - _lat) < _approx_dist_threshold) && 
		     (abs(other._lng - _lng) < _approx_dist_threshold) )
			return true;
		else
			return false;
	}

	// More accurate estimate if the points are equal, but slowly
	bool operator==(const LatLng& other) const
	{
		const double d = dist(other);
		
		if (abs(d) < _dist_threshld)
			return true;
		else
			return false;
	}

private:
	double _dist_threshld;
	double _approx_dist_threshold;

};

#endif // LATLNG_H