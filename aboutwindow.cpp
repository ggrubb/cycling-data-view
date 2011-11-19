#include "aboutwindow.h"
 
/******************************************************/
AboutWindow::AboutWindow(const QPixmap& pixmap)
{
	QSplashScreen::setPixmap(pixmap);
}
 
/******************************************************/
AboutWindow::~AboutWindow()
{
}
 
/******************************************************/
void AboutWindow::drawContents(QPainter* painter)
{ 
	QPixmap textPix = QSplashScreen::pixmap();
	painter->setPen(_color);
	painter->drawText(_rect, _alignement, _message);
}
 
/******************************************************/
void AboutWindow::showVersionInfo(const QString& message, const QColor& color)
{
	_message = message;
	_color = color;
	showMessage(_message, _alignement, _color);
}
 
/******************************************************/
void AboutWindow::setMessageRect(QRect rect, int alignement)
{
	_rect = rect;
	_alignement = alignement;
}