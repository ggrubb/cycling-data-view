#ifndef DATESELECTORWIDGET_H
#define DATESELECTORWIDGET_H

#include <qtxml/qdomdocument>
#include <QGroupBox.h>
#include <QDateTime.h>

class QSlider;
class QLabel;
class QCheckBox;

class DateSelectorWidget : public QGroupBox
{
	Q_OBJECT
 public:
	DateSelectorWidget();
	~DateSelectorWidget();

	QDate minDate() const;
	QDate maxDate() const;
 
	void setRangeDates(
		const QDate& start_date, 
		const QDate& end_date);

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