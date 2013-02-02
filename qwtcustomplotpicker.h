#ifndef QWTCUSTOMPLOTPICKER_H
#define QWTCUSTOMPLOTPICKER_H

#include <qwt_plot_picker.h>

#include <boost/shared_ptr.hpp>

class DataLog;
class QCheckBox;

// A custom plot picker to highligh the value of curve closest to the pointer
class QwtCustomPlotPicker : public QwtPlotPicker
{
	Q_OBJECT

	enum AxisUnits {
		TimeAxis,
		DistAxis
	};

	public:
		QwtCustomPlotPicker(
			int x_axis, int y_axis, 
			boost::shared_ptr<DataLog> data_log, 
			QwtPlotCanvas* canvas, 
			boost::shared_ptr<QCheckBox> hr_cb,
			boost::shared_ptr<QCheckBox> speed_cb,
			boost::shared_ptr<QCheckBox> alt_cb,
			boost::shared_ptr<QCheckBox> cadence_cb,
			boost::shared_ptr<QCheckBox> power_cb,
			boost::shared_ptr<QCheckBox> temp_cb);

		// Set the data log for this picker
		void setDataLog(boost::shared_ptr<DataLog> data_log);

	protected:
		// Surpress default tracker drawing
		void drawTracker(QPainter* painter) const;

		// Draw curve numerical values and highligh curve points
		void drawRubberBand(QPainter* painter) const;

	public slots:
		void xAxisUnitsChanged(int units);

	private:
		boost::shared_ptr<DataLog> _data_log;
		AxisUnits _x_axis_units;

		boost::shared_ptr<QCheckBox> _hr_cb;
		boost::shared_ptr<QCheckBox> _speed_cb;
		boost::shared_ptr<QCheckBox> _alt_cb;
		boost::shared_ptr<QCheckBox> _cadence_cb;
		boost::shared_ptr<QCheckBox> _power_cb;
		boost::shared_ptr<QCheckBox> _temp_cb;
};


#endif // QWTCUSTOMPLOTPICKER_H