#ifndef HSVRANGECONTROLLER_H
#define HSVRANGECONTROLLER_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <QLayout>

#include "../../Config/config.h"
#include "../../Data/data.h"
#include "./RangeController/rangecontroller.h"

namespace Ui {
	class HSVRangeController;
}

class HSVRangeController : public QWidget
{
	Q_OBJECT

public:
	explicit HSVRangeController(QWidget *parent = 0);
	~HSVRangeController();

    void initialize(Data *data, int type);

private slots:
    void changedHRange(int min, int max);
    void changedSRange(int min, int max);
    void changedVRange(int min, int max);

private:
	Ui::HSVRangeController *ui;
	Config *cfg;
	Data *data;
    int type;

	RangeController *H;
	RangeController *S;
	RangeController *V;
};

#endif // HSVRANGECONTROLLER_H
