#ifndef TOTALSWINDOW_H
#define TOTALSWINDOW_H
 
#include <Qwidget.h>

class User;

class TotalsWindow : public QWidget
{
	Q_OBJECT
public:
	TotalsWindow(User* user);
	~TotalsWindow();

private:
	User* _user;

};
 
#endif // TOTALSWINDOW_H