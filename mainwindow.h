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
 class User;

 class MainWindow : public QMainWindow
 {
    Q_OBJECT

 public:
    MainWindow();
    ~MainWindow();

 private slots:
    void setUser();
    void addUser();
    void about();
	void setRider(User* user);
	void setRide(DataLog* data_log);
	void setLap(int lap_index);
	
 private:
    void createActions();
    void createMenus();

    QAction* _set_act;
    QAction* _add_act;
    QAction* _edit_act;
    QAction* _exit_act;
    QAction* _about_act;

    QMenu* _file_menu;
    QMenu* _help_menu;

	GoogleMap* _google_map;
	PlotWindow* _plot_window;
	DataStatisticsView* _stats_view;
	RideSelectionWindow* _ride_selector;

	User* _current_user;
 };

 #endif // MAINWINDOW_H