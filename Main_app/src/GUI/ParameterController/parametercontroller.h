#ifndef PARAMETERCONTROLLER_H
#define PARAMETERCONTROLLER_H

#include <QWidget>

namespace Ui {
	class ParameterController;
}

class ParameterController : public QWidget
{
	Q_OBJECT

public:
	explicit ParameterController(QWidget *parent = 0);
	~ParameterController();

	void setName(QString name);
	void setRange(int min, int max);
	void setValue(int value);

signals:
	void changedParameter(int value);

private slots:
	void on_sldrValue_valueChanged(int value);
	void on_spnbxValue_valueChanged(int value);

private:
	Ui::ParameterController *ui;
};

#endif // PARAMETERCONTROLLER_H
