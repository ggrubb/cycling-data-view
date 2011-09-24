// cyclingdataview.cpp : Defines the entry point for the console application.

#include <qtgui/qapplication>
#include "mainwindow.h"

int main( int argc, char ** argv )
{
	QApplication a( argc, argv );
    ImageViewer* iv = new ImageViewer();
    iv->show();
    return a.exec();
}