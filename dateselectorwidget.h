#ifndef DATESELECTORWIDGET_H
#define DATESELECTORWIDGET_H

#include <qtxml/qdomdocument>
#include <QWidget.h>
#include <QDateTime.h>

class QSlider;
class QLabel;
class QCheckBox;

class DateSelectorWidget : public QWidget
{
	Q_OBJECT
 public:
	DateSelectorWidget(QDate& start_date, QDate& end_date);
	~DateSelectorWidget();

	QDate minDate() const;
	QDate maxDate() const;
 
 signals:
	void datesChanged();

 private slots:
	void setSlidersDisabled(int disabled);
	void minSliderChanged(int day_number);
	void maxSliderChanged(int day_number);
	void userChangedDates();

 private:
	void updateLabels();

	QSlider* _min_date_slider;
	QSlider* _max_date_slider;

	QLabel* _min_date_label;
	QLabel* _max_date_label;

	QCheckBox* _all_dates_cb;

	QDate _start_date;
	QDate _end_date;
};

#endif // DATESELECTORWIDGET_H