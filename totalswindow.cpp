#include "totalswindow.h"
#include "user.h"
#include "logdirectorysummary.h"

#include <QDateTime.h>
#include <QIcon.h>

/******************************************************/
TotalsWindow::TotalsWindow(User* user):
QWidget()
{
	_user = user;

	setWindowTitle(tr("Totals"));
	setWindowIcon(QIcon("./resources/rideviewer_head128x128.ico")); 

	LogDirectorySummary log_dir_summary(_user->logDirectory());
	log_dir_summary.readFromFile();

	std::vector<double> ride_time;
	std::vector<double> ride_dist;
	std::vector<int> time;
	for (unsigned int i=0; i < log_dir_summary.numLogs(); ++i)
	{
		QString tmp = log_dir_summary.log(i)._date.split(' ')[0]; // split at the ' ' to get date only (no time)
		QDate date = QDate::fromString(tmp,Qt::ISODate);
	}

	show();
}
 
/******************************************************/
TotalsWindow::~TotalsWindow()
{

}
