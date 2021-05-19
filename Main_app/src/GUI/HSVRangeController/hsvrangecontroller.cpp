#include "hsvrangecontroller.h"
#include "ui_hsvrangecontroller.h"

HSVRangeController::HSVRangeController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::HSVRangeController)
{
	ui->setupUi(this);
	cfg = new Config();

	H = new RangeController(this);
	S = new RangeController(this);
	V = new RangeController(this);

	H->setLabel("H");
	S->setLabel("S");
	V->setLabel("V");
	//	H->setLabel("Y");
	//	S->setLabel("U");
	//	V->setLabel("V");

	H->setRange(0, 180);
	//	H->setRange(0, 255);
	S->setRange(0, 255);
	V->setRange(0, 255);

	H->setValues(cfg->getInteger("VISION", "H_MIN_GREEN"), cfg->getInteger("VISION", "H_MAX_GREEN"));
	S->setValues(cfg->getInteger("VISION", "S_MIN_GREEN"), cfg->getInteger("VISION", "S_MAX_GREEN"));
	V->setValues(cfg->getInteger("VISION", "V_MIN_GREEN"), cfg->getInteger("VISION", "V_MAX_GREEN"));

	ui->verticalLayout->addWidget(H);
	ui->verticalLayout->addWidget(S);
	ui->verticalLayout->addWidget(V);

	connect(H, SIGNAL(changedValue(int,int)), this, SLOT(changedHRange(int,int)));
	connect(S, SIGNAL(changedValue(int,int)), this, SLOT(changedSRange(int,int)));
	connect(V, SIGNAL(changedValue(int,int)), this, SLOT(changedVRange(int,int)));
}

HSVRangeController::~HSVRangeController()
{
	delete ui;
}

void HSVRangeController::initialize(Data *data)
{
	this->data = data;
	data->hsvRngsGreen.H = cv::Range(cfg->getInteger("VISION", "H_MIN_GREEN"), cfg->getInteger("VISION", "H_MAX_GREEN"));
	data->hsvRngsGreen.S = cv::Range(cfg->getInteger("VISION", "S_MIN_GREEN"), cfg->getInteger("VISION", "S_MAX_GREEN"));
	data->hsvRngsGreen.V = cv::Range(cfg->getInteger("VISION", "V_MIN_GREEN"), cfg->getInteger("VISION", "V_MAX_GREEN"));
}

void HSVRangeController::changedHRange(int min, int max)
{
	data->hsvRngsGreen.H.start = min;
	data->hsvRngsGreen.H.end = max;
}

void HSVRangeController::changedSRange(int min, int max)
{
	data->hsvRngsGreen.S.start = min;
	data->hsvRngsGreen.S.end = max;
}

void HSVRangeController::changedVRange(int min, int max)
{
	data->hsvRngsGreen.V.start = min;
	data->hsvRngsGreen.V.end = max;
}