#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

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
 class LogEditorWindow;

 class MainWindow : public QMainWindow
 {
    Q_OBJECT

 public:
    MainWindow();
    ~MainWindow();

	void refresh();

 protected:
	virtual void closeEvent(QCloseEvent* event);

 private slots:
    void promptForUser();
    void addUser();
    void editUser();
	void retrieveLogs();
    void totals();
	void mapCollage();
	void rideIntervalFinder();
	void logFileEditor();
    void about();
    void help();
	void goToProjectPage();
	void setUser(boost::shared_ptr<User> user);
	void setRide(boost::shared_ptr<DataLog> data_log);
	void setLap(int lap_index);
	
 private:
	void checkForSaveUser();
    void createActions();
    void createMenus();

    QAction* _set_act;
    QAction* _add_act;
    QAction* _edit_act;
    QAction* _retrieve_logs_act;
    QAction* _totals_act;
    QAction* _map_collage_act;
    QAction* _ride_interval_finder_act;
    QAction* _log_file_editor_act;
    QAction* _exit_act;
    QAction* _about_act;
	QAction* _goto_help_act;
	QAction* _goto_project_page_act;

    QMenu* _file_menu;
    QMenu* _tools_menu;
    QMenu* _help_menu;

	boost::shared_ptr<GoogleMapWindow> _google_map;
	boost::shared_ptr<PlotWindow> _plot_window;
	boost::shared_ptr<DataStatisticsWindow> _stats_view;
	boost::shared_ptr<RideSelectionWindow> _ride_selector;

	boost::scoped_ptr<TotalsWindow> _totals_window;
	boost::scoped_ptr<GoogleMapCollageWindow> _ride_collage;
	boost::scoped_ptr<RideIntervalFinderWindow> _rider_interval_finder;
	boost::scoped_ptr<LogEditorWindow> _log_file_editor;

	boost::shared_ptr<User> _current_user;
 };

 #endif // MAINWINDOW_H