#ifndef CAMERAPARAMETERCONTOLLER_H
#define CAMERAPARAMETERCONTOLLER_H

#include <QWidget>
#include <QString>
#include <QDebug>

#include "../../Config/config.h"
#include "../../Data/data.h"
#include "../ParameterController/parametercontroller.h"

namespace Ui {
	class CameraParameterContoller;
}

class CameraParameterContoller : public QWidget
{
	Q_OBJECT

public:
	explicit CameraParameterContoller(QWidget *parent = 0);
	~CameraParameterContoller();

	void initialize(Data *data);

private slots:
	void changedColorExposure(int value);
	void changedIrExposure(int value);

private:
	Ui::CameraParameterContoller *ui;
	Config *cfg;
	Data *data;

	ParameterController *ir_Exposure;
	ParameterController *color_Exposure;
};

#endif // CAMERAPARAMETERCONTOLLER_H
