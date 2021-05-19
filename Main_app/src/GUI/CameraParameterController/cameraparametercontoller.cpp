#include "cameraparametercontoller.h"
#include "ui_cameraparametercontoller.h"

CameraParameterContoller::CameraParameterContoller(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CameraParameterContoller)
{
	ui->setupUi(this);

	cfg = new Config();

	color_Exposure = new ParameterController(this);
	ir_Exposure = new ParameterController(this);

	color_Exposure->setName("color exposure");
	ir_Exposure->setName("ir exporsure");

	color_Exposure->setRange(-30, -1);
	ir_Exposure->setRange(1, 30);

	color_Exposure->setValue(cfg->getInteger("VISION", "COLOR_EXPOSURE"));
	ir_Exposure->setValue(cfg->getInteger("VISION", "IR_EXPOSURE"));

	ui->vl->addWidget(color_Exposure);
	ui->vl->addWidget(ir_Exposure);

	connect(color_Exposure, SIGNAL(changedParameter(int)), this, SLOT(changedColorExposure(int)));
	connect(ir_Exposure, SIGNAL(changedParameter(int)), this, SLOT(changedIrExposure(int)));
}

CameraParameterContoller::~CameraParameterContoller()
{
	delete ui;
}

void CameraParameterContoller::initialize(Data *data)
{
	this->data = data;
}

void CameraParameterContoller::changedColorExposure(int value)
{
	data->camParams.color_exposure = value;
	data->isCamParamChange = true;
}

void CameraParameterContoller::changedIrExposure(int value)
{
	data->camParams.ir_exposure = value;
	data->isCamParamChange = true; //flag on
}
