#include "user.h"

/****************************************/
User::User(
	const QString& name,
	const QString& log_directory,
	double weight,
	double bike_weight,
	int hr_zone1,
	int hr_zone2,
	int hr_zone3,
	int hr_zone4,
	int hr_zone5):
_name(name),
_log_directory(log_directory),
_weight(weight),
_bike_weight(bike_weight),
_hr_zone1(hr_zone1),
_hr_zone2(hr_zone2),
_hr_zone3(hr_zone3),
_hr_zone4(hr_zone4),
_hr_zone5(hr_zone5)
{}

/****************************************/
User::~User()
{}

/****************************************/
const QString& User::name() const
{
	return _name;
}

/****************************************/
const QString& User::logDirectory() const
{
	return _log_directory;
}

/****************************************/
double User::weight() const
{
	return _weight;
}

/****************************************/
double User::bikeWeight() const
{
	return _bike_weight;
}

/****************************************/
int User::zone1() const
{
	return _hr_zone1;
}

/****************************************/
int User::zone2() const
{
	return _hr_zone2;
}

/****************************************/
int User::zone3() const
{
	return _hr_zone3;
}

/****************************************/
int User::zone4() const
{
	return _hr_zone4;
}

/****************************************/
int User::zone5() const
{
	return _hr_zone5;
}