#include "user.h"

#include <fstream>

#include <qtxml/qdomdocument>
#include <QFile.h>

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
User::User()
{

}

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

/****************************************/
void User::readFromFile(const QString& filename)
{
	QDomDocument dom_document;
	QString error_msg;
	int error_line, error_column;
	QFile file(filename);
	bool read_success = dom_document.setContent(&file, &error_msg, &error_line, &error_column);
	
	if (read_success)
	{
		QDomElement user = dom_document.documentElement();
	
		_name = user.firstChildElement("Name").firstChild().nodeValue();
		_log_directory = user.firstChildElement("LogDirectory").firstChild().nodeValue();
		_weight = user.firstChildElement("Weight").firstChild().nodeValue().toDouble();
		_bike_weight = user.firstChildElement("BikeWeight").firstChild().nodeValue().toDouble();
		_hr_zone1 = user.firstChildElement("HRZone1").firstChild().nodeValue().toDouble();
		_hr_zone2 = user.firstChildElement("HRZone2").firstChild().nodeValue().toDouble();
		_hr_zone3 = user.firstChildElement("HRZone3").firstChild().nodeValue().toDouble();
		_hr_zone4 = user.firstChildElement("HRZone4").firstChild().nodeValue().toDouble();
		_hr_zone5 = user.firstChildElement("HRZone5").firstChild().nodeValue().toDouble();
	}
}

/****************************************/
void User::writeToFile(const QString& filename) const
{
	QDomDocument dom_document;
	QDomElement user = dom_document.createElement("User");
	dom_document.appendChild(user);

	QDomElement name = dom_document.createElement("Name");
	user.appendChild(name);
	QDomText text = dom_document.createTextNode(_name);
	name.appendChild(text);

	QDomElement log_dir = dom_document.createElement("LogDirectory");
	user.appendChild(log_dir);
	text = dom_document.createTextNode(_log_directory);
	log_dir.appendChild(text);

	QDomElement weight = dom_document.createElement("Weight");
	user.appendChild(weight);
	text = dom_document.createTextNode(QString::number(_weight,'f',2));
	weight.appendChild(text);

	QDomElement bike_weight = dom_document.createElement("BikeWeight");
	user.appendChild(bike_weight);
	text = dom_document.createTextNode(QString::number(_bike_weight,'f',2));
	bike_weight.appendChild(text);

	QDomElement hr_zone1 = dom_document.createElement("HRZone1");
	user.appendChild(hr_zone1);
	text = dom_document.createTextNode(QString::number(_hr_zone1,'f',2));
	hr_zone1.appendChild(text);

	QDomElement hr_zone2 = dom_document.createElement("HRZone2");
	user.appendChild(hr_zone2);
	text = dom_document.createTextNode(QString::number(_hr_zone2,'f',2));
	hr_zone2.appendChild(text);

	QDomElement hr_zone3 = dom_document.createElement("HRZone3");
	user.appendChild(hr_zone3);
	text = dom_document.createTextNode(QString::number(_hr_zone3,'f',2));
	hr_zone3.appendChild(text);

	QDomElement hr_zone4 = dom_document.createElement("HRZone4");
	user.appendChild(hr_zone4);
	text = dom_document.createTextNode(QString::number(_hr_zone4,'f',2));
	hr_zone4.appendChild(text);

	QDomElement hr_zone5 = dom_document.createElement("HRZone5");
	user.appendChild(hr_zone5);
	text = dom_document.createTextNode(QString::number(_hr_zone5,'f',2));
	hr_zone5.appendChild(text);

	const int indent = 4;
	QString xml = dom_document.toString(indent);
	std::ofstream file;
	file.open(filename.toStdString().c_str());
	file << xml.toStdString();
	file.close();
}