#ifndef USER_H
#define USER_H

#include <QString.h>

/* Class to represent a single rider */

class User
 {
 public:
	User(const QString& name,
		 const QString& log_dir,
		 double weight,
		 int hr_zone1,
		 int hr_zone2,
		 int hr_zone3,
		 int hr_zone4,
		 int hr_zone5);
	User();
	~User();

	const QString& name() const;
	const QString& logDirectory() const;
	double weight() const;
	int zone1() const;
	int zone2() const;
	int zone3() const;
	int zone4() const;
	int zone5() const;

	bool readFromFile(const QString& filename);
	void writeToFile(const QString& filename) const;

 private:
	 QString _name;
	 QString _log_directory;
	 double _weight; // kg
	 int _hr_zone1; // recovery
	 int _hr_zone2; // endurance
	 int _hr_zone3; // tempo
	 int _hr_zone4; // threshold
	 int _hr_zone5; // V02 max
 };

#endif // USER_H