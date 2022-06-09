
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
	void Infinity_Judge();

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
	void finishedAnalyze();
	void window_close();
	void saveimage_r(QString);
	void btnStop_to_false();
	void search_INF(BDData_t &bd);
	void show_plnvwr();
    void calc_pre(BDData_t &bd);

private:
	Ui::MainController *ui;
	Data *data;

	QButtonGroup *bgChbxShows;


    //for interplation
	bool interpolation = true;
	bool cnt_switch = false;
	int _INF_count = 0;
    QPoint first_pix = {0,0};

	QPoint last_pix;
    QPointF first = {0.0,0.0};

	QPointF last;
    QPoint est_array_pix[1000];
    QPointF est_array[1000];

    int color_array[1000];

};


#endif // MAINCONTROLLER_H
