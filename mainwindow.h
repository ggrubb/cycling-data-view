#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qtgui/qmainwindow>

 class QAction;
 class QLabel;
 class QMenu;
 class GoogleMap;
 class PlotWindow;
 class DataLog;
 class DataStatisticsView;
 class RideSelectionWindow;

 class MainWindow : public QMainWindow
 {
    Q_OBJECT

 public:
    MainWindow();
    ~MainWindow();

 private slots:
    void open();
    void about();
	void setRide(DataLog* data_log);

 private:
    void createActions();
    void createMenus();

    QAction* _open_act;
    QAction* _exit_act;
    QAction* _about_act;

    QMenu* _file_menu;
    QMenu* _help_menu;

	GoogleMap* _google_map;
	PlotWindow* _plot_window;
	DataLog* _data_log;
	DataStatisticsView* _stats_view;
	RideSelectionWindow* _ride_selector;
 };

 #endif // MAINWINDOW_H