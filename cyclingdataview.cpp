// cyclingdataview.cpp : Defines the entry point for the console application.

#include <qtgui/qapplication>
#include "mainwindow.h"

int main( int argc, char ** argv )
{
	QApplication a( argc, argv );
    MainWindow* mw = new MainWindow();
    mw->show();
    return a.exec();
}