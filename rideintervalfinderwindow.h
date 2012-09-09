#ifndef RIDEINTERVALFINDERWINDOW_H
#define RIDEINTERVALFINDERWINDOW_H

#include <qtxml/qdomdocument>
#include <QWidget.h>
#include <QMap.h>

class DataLog;
class TcxParser;
class FitParser;
class DateSelectorWidget;
class User;

class RideIntervalFinderWindow : public QWidget
{
	//Q_OBJECT

 public:
	RideIntervalFinderWindow(User* user);
	~RideIntervalFinderWindow();

 private:
	
	bool parse(const QString filename, DataLog* data_log);

	TcxParser* _tcx_parser;
	FitParser* _fit_parser;

	DateSelectorWidget* _date_selector_widget;

	User* _user;

};

#endif // RIDEINTERVALFINDERWINDOW_H