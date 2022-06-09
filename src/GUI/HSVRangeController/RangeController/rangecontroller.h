#ifndef RANGECONTROLLER_H
#define RANGECONTROLLER_H

#include <QWidget>
#include <QString>
#include <QDebug>

namespace Ui {
	class RangeController;
}

class RangeController : public QWidget
{
	Q_OBJECT

public:
	explicit RangeController(QWidget *parent = 0);
	~RangeController();

	void setLabel(QString label);
	void setRange(int min, int max);
	void setValues(int min, int max);

private slots:
	void on_sldrMin_valueChanged(int value);
	void on_sldrMax_valueChanged(int value);

signals:
	void changedValue(int, int);

private:
	Ui::RangeController *ui;
};

#endif // RANGECONTROLLER_H
