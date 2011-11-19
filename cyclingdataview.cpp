// cyclingdataview.cpp : Defines the entry point for the console application.

#include <QApplication.h>
#include <QSplashScreen.h>
#include <windows.h>
#include "mainwindow.h"

int main( int argc, char ** argv )
{
	QApplication a( argc, argv );

	QPixmap pixmap("resources/splash_screen.png");
	QSplashScreen splash(pixmap);
	splash.show();
	Sleep(2000);

    MainWindow* mw = new MainWindow();
    mw->show();
    return a.exec();
}