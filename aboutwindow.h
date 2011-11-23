#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H
 
#include <QSplashScreen.h>
#include <QPainter.h>
 
class AboutWindow : public QSplashScreen
{
 
public:
	 AboutWindow(const QPixmap& pixmap);
	 ~AboutWindow();

	 virtual void drawContents(QPainter *painter);
	 void showVersionInfo(const QString& message, const QColor& color = Qt::black);
	 void setMessageRect(QRect rect, int alignment = Qt::AlignLeft);
 
private:
	 QString _message;
	 int _alignement;
	 QColor _color;
	 QRect _rect;
};
 
#endif // ABOUTWINDOW_H