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

//	H->setRange(0, 180);
    H->setRange(0, 255);
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

void HSVRangeController::initialize(Data *data, int type)
{
    this->type = type;
	this->data = data;

    if(this->type==0){
        data->hsvRngsGreen.H = cv::Range(cfg->getInteger("VISION", "H_MIN_GREEN"), cfg->getInteger("VISION", "H_MAX_GREEN"));
        data->hsvRngsGreen.S = cv::Range(cfg->getInteger("VISION", "S_MIN_GREEN"), cfg->getInteger("VISION", "S_MAX_GREEN"));
        data->hsvRngsGreen.V = cv::Range(cfg->getInteger("VISION", "V_MIN_GREEN"), cfg->getInteger("VISION", "V_MAX_GREEN"));
    }

    if(this->type==1){
        data->hsvRngsBD.H = cv::Range(cfg->getInteger("VISION", "H_MIN_GREEN"), cfg->getInteger("VISION", "H_MAX_GREEN"));
        data->hsvRngsBD.S = cv::Range(cfg->getInteger("VISION", "S_MIN_GREEN"), cfg->getInteger("VISION", "S_MAX_GREEN"));
        data->hsvRngsBD.V = cv::Range(cfg->getInteger("VISION", "V_MIN_GREEN"), cfg->getInteger("VISION", "V_MAX_GREEN"));
    }
}

void HSVRangeController::changedHRange(int min, int max)
{
    if(this->type==0){
        data->hsvRngsGreen.H.start = min;
        data->hsvRngsGreen.H.end = max;
    }
    if(this->type==1){
        data->hsvRngsBD.H.start = min;
        data->hsvRngsBD.H.end = max;
    }
}

void HSVRangeController::changedSRange(int min, int max)
{
    if(this->type==0){
        data->hsvRngsGreen.S.start = min;
        data->hsvRngsGreen.S.end = max;
    }
    if(this->type==1){
        data->hsvRngsBD.S.start = min;
        data->hsvRngsBD.S.end = max;
    }
}

void HSVRangeController::changedVRange(int min, int max)
{
    if(this->type==0){
        data->hsvRngsGreen.V.start = min;
        data->hsvRngsGreen.V.end = max;
    }
    if(this->type==1){
        data->hsvRngsBD.V.start = min;
        data->hsvRngsBD.V.end = max;
    }
}
