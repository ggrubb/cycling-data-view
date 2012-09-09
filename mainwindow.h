#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow.h>

 class QAction;
 class QLabel;
 class QMenu;
 class GoogleMapWindow;
 class GoogleMapCollageWindow;
 class PlotWindow;
 class DataLog;
 class DataStatisticsWindow;
 class RideSelectionWindow;
 class User;
 class TotalsWindow;
 class RideIntervalFinderWindow;

 class MainWindow : public QMainWindow
 {
    Q_OBJECT

 public:
    MainWindow();
    ~MainWindow();

 protected:
	virtual void closeEvent(QCloseEvent* event);

 private slots:
    void promptForUser();
    void addUser();
    void editUser();
	void setSingleLogMode();
	void setDoubleLogMode();
	void retrieveLogs();
    void totals();
	void mapCollage();
	void rideIntervalFinder();
	void logFileEditor();
    void about();
    void help();
	void goToProjectPage();
	void setUser(User* user);
	void setRide(DataLog* data_log);
	void setLap(int lap_index);
	
 private:
	void checkForSaveUser();
    void createActions();
    void createMenus();

    QAction* _set_act;
    QAction* _add_act;
    QAction* _edit_act;
    QAction* _retrieve_logs_act;
	QAction* _single_log_mode_act;
	QAction* _double_log_mode_act;
    QAction* _totals_act;
    QAction* _map_collage_act;
    QAction* _ride_interval_finder_act;
    QAction* _log_file_editor_act;
    QAction* _exit_act;
    QAction* _about_act;
	QAction* _goto_help_act;
	QAction* _goto_project_page_act;

    QMenu* _file_menu;
    QMenu* _mode_menu;
    QMenu* _tools_menu;
    QMenu* _help_menu;

	GoogleMapWindow* _google_map;
	PlotWindow* _plot_window;
	DataStatisticsWindow* _stats_view;
	RideSelectionWindow* _ride_selector;
	TotalsWindow* _totals_window;
	GoogleMapCollageWindow* _ride_collage;
	RideIntervalFinderWindow* _rider_interval_finder;

	User* _current_user;
 };

 #endif // MAINWINDOW_H