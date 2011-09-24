#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <qtgui/qmainwindow>
#include <qtgui/qprinter>
#include <qtgui/qprintdialog>
#include <qtxml/qdomdocument>

 class QAction;
 class QLabel;
 class QMenu;
 class QScrollArea;
 class QScrollBar;

 class ImageViewer : public QMainWindow
 {
     Q_OBJECT

 public:
     ImageViewer();

 private slots:
     void open();
     void print();
     void zoomIn();
     void zoomOut();
     void normalSize();
     void fitToWindow();
     void about();
	 void plotSelection(const QPointF& point);

 private:
     void createActions();
     void createMenus();
     void updateActions();
     void scaleImage(double factor);
     void adjustScrollBar(QScrollBar* scroll_bar, double factor);

 #ifndef QT_NO_PRINTER
     QPrinter _printer;
 #endif
     QLabel* _image_label;
     QScrollArea* _scroll_area;
     double _scale_factor;

     QAction* _open_act;
     QAction* _print_act;
     QAction* _exit_act;
     QAction* _zoom_in_act;
     QAction* _zoom_out_act;
     QAction* _normal_size_act;
     QAction* _fit_to_window_act;
     QAction* _about_act;

     QMenu* _file_menu;
     QMenu* _view_menu;
     QMenu* _help_menu;

	 QDomDocument _dom_document;
 };

 #endif