
#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QWidget>
#include <QDebug>
#include <QString>
#include <QButtonGroup>
#include <QMessageBox>

#include "../../Data/data.h"

namespace Ui {
	class MainController;
}

class MainController : public QWidget
{
	Q_OBJECT

public:
	explicit MainController(QWidget *parent = 0);
	~MainController();

	int initialize(Data *data);
	void setFirstWidget(QWidget *widget);

private slots:
	void on_btnRefresh_clicked();
	void on_btnAnalyze_clicked();
	void CheckBox_Triggered(int idx, bool checked);

	void on_pushButton_6_clicked();

	void on_saveBtn_clicked();
	void AnalyzeColorBtn();
	void RefrashColorBtn();

	void btn_Refresh_Controll();

signals:
	void reqRefresh();
	void startAnalyze();
	void endAnalyze();
	void window_close();
	void saveimage_r();
	void btnStop_to_false();

private:
	Ui::MainController *ui;
	Data *data;

	QButtonGroup *bgChbxShows;
};

#endif // MAINCONTROLLER_H
