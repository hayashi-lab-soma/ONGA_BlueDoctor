#include "parametercontroller.h"
#include "ui_parametercontroller.h"

ParameterController::ParameterController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ParameterController)
{
	ui->setupUi(this);
}

ParameterController::~ParameterController()
{
	delete ui;
}

void ParameterController::setName(QString name)
{
	ui->lblName->setText(name);
}

void ParameterController::setRange(int min, int max)
{
	ui->sldrValue->setRange(min, max);
	ui->spnbxValue->setRange(min, max);
}

void ParameterController::setValue(int value)
{
	ui->sldrValue->setValue(value);
	ui->spnbxValue->setValue(value);
}

void ParameterController::on_sldrValue_valueChanged(int value)
{
	ui->spnbxValue->setValue(value);
	emit changedParameter(value);
}

void ParameterController::on_spnbxValue_valueChanged(int value)
{
	ui->sldrValue->setValue(value);
	emit changedParameter(value);
}
