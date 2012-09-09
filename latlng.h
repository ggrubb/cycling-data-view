#ifndef LATLNG_H
#define LATLNG_H

#include <math.h>
#include <iostream>

#define M_PI 3.14159265358979323846
#define DISTANCE_THRESHOLD 300.0 // metres for comparison operator

class LatLng
{
public:
	LatLng(double lati, double lngi):
	  lat(lati),
	  lng(lngi)
	{}
	LatLng(){}

	double lat;
	double lng;

	bool operator==(const LatLng &other) const
	{
		// lat2 = other
		// lat1 = this
		const double R = 6371000.0; // m, radius of the earth
		const double d_lat = (other.lat-lat)*M_PI/180.0;
		const double d_lon = (other.lng-lng)*M_PI/180.0;
		const double lat1 = lat*M_PI/180.0;
		const double lat2 = other.lat*M_PI/180.0;

		const double a = sin(d_lat/2.0) * sin(d_lat/2.0) +
				sin(d_lon/2.0) * sin(d_lon/2.0) * cos(lat1) * cos(lat2); 
		const double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a)); 
		const double dist = R * c;
		
		//std::cout << "this: " << lat << ", " << lng << " other: " << other.lat << ", " << other.lng << " dist: " << dist << std::endl;
		if (abs(dist) < DISTANCE_THRESHOLD)
			return true;
		else
			return false;
	}
};

#endif // LATLNG_H