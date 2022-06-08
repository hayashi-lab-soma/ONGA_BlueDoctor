#include "rangecontroller.h"
#include "ui_rangecontroller.h"

RangeController::RangeController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RangeController)
{
	ui->setupUi(this);
}

RangeController::~RangeController()
{
	delete ui;
}

void RangeController::setLabel(QString label)
{
	ui->lblName->setText(label);
}

void RangeController::setRange(int min, int max)
{
	ui->sldrMin->setRange(min, max);
	ui->sldrMax->setRange(min, max);
}

void RangeController::setValues(int min, int max)
{
	ui->sldrMin->setValue(min);
	ui->sldrMax->setValue(max);
}


void RangeController::on_sldrMin_valueChanged(int value)
{
	QString str;
	str += "(";
	str += QString::number(value);
	str += ", ";
	str += QString::number(ui->sldrMax->value());
	str += ")";
	ui->lblRange->setText(str);

	emit changedValue(ui->sldrMin->value(), ui->sldrMax->value());
}

void RangeController::on_sldrMax_valueChanged(int value)
{
	QString str;
	str += "(";
	str += QString::number(ui->sldrMin->value());
	str += ", ";
	str += QString::number(value);
	str += ")";
	ui->lblRange->setText(str);

	emit changedValue(ui->sldrMin->value(), ui->sldrMax->value());
}
