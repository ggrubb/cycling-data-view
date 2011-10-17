#ifndef QWTCUSTOMPLOTPICKER_H
#define QWTCUSTOMPLOTPICKER_H

#include <qwt_plot_picker.h>

class DataLog;

// A custom plot picker to highligh the value of curve closest to the pointer
class QwtCustomPlotPicker : public QwtPlotPicker
{
	Q_OBJECT

	enum AxisUnits {
		TimeAxis,
		DistAxis
	};

	public:
		QwtCustomPlotPicker(int x_axis, int y_axis, DataLog* data_log, QwtPlotCanvas* canvas);

		// Set the data log for this picker
		void setDataLog(
			DataLog* data_log,
			std::vector<double>* data_hr_filtered,
			std::vector<double>* data_speed_filtered,
			std::vector<double>* data_cadence_filtered,
			std::vector<double>* data_alt_filtered);

		// Surpress default tracker drawing
		void drawTracker(QPainter* painter) const;

		// Draw curve numerical values and highligh curve points
		void drawRubberBand(QPainter* painter) const;

	public slots:
		void xAxisUnitsChanged(int units);

	private:
		DataLog* _data_log;
		AxisUnits _x_axis_units;

		std::vector<double>* _data_hr_filtered;
		std::vector<double>* _data_speed_filtered;
		std::vector<double>* _data_cadence_filtered;
		std::vector<double>* _data_alt_filtered;
};


#endif // QWTCUSTOMPLOTPICKER_H