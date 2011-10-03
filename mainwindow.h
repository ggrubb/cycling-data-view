#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qtgui/qmainwindow>

 class QAction;
 class QLabel;
 class QMenu;
 class TcxParser;
 class GoogleMap;
 class PlotWindow;
 class DataStatisticsView;

 class MainWindow : public QMainWindow
 {
    Q_OBJECT

 public:
    MainWindow();
    ~MainWindow();

 private slots:
    void open();
    void about();

 private:
    void createActions();
    void createMenus();

    QAction* _open_act;
    QAction* _exit_act;
    QAction* _about_act;

    QMenu* _file_menu;
    QMenu* _help_menu;

	TcxParser* _parser;
	GoogleMap* _google_map;
	PlotWindow* _plot_window;
	DataStatisticsView* _stats_view;
 };

 #endif // MAINWINDOW_H