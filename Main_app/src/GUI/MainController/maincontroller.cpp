#include "maincontroller.h"
#include "ui_maincontroller.h"

MainController::MainController(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainController)
{
	ui->setupUi(this);
	setWindowTitle("Main Controller");

	bgChbxShows = new QButtonGroup(this);
	bgChbxShows->addButton(ui->chbxShowLevel_Y);
	bgChbxShows->addButton(ui->chbxShowLevel_R);		//1
	bgChbxShows->addButton(ui->chbxShowAnalized);	//2
	bgChbxShows->addButton(ui->chbxShowMeasured);	//3
	bgChbxShows->setExclusive(true);

	connect(bgChbxShows, SIGNAL(buttonToggled(int,bool)), this, SLOT(CheckBox_Triggered(int,bool)));

	ui->btnAnalyze->setEnabled(false);
	ui->btnAnalyze->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
	ui->saveBtn->setEnabled(false);
	ui->saveBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
	ui->btnRefresh->setEnabled(false);
	ui->btnRefresh->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
}

MainController::~MainController()
{
	delete ui;
}

int MainController::initialize(Data *data)
{
	this->data = data;
	return 0;
}

void MainController::setFirstWidget(QWidget *widget)
{
	ui->vlFirst->addWidget(widget);
	return;
}

void MainController::on_btnRefresh_clicked()
{
	emit btnStop_to_false();
	data->isRefresh = true;
	data->isAnalyzing = false;
	data->isAnalyzed = false;

	ui->btnAnalyze->setEnabled(false);
	ui->btnAnalyze->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
	ui->saveBtn->setEnabled(false);
	ui->saveBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
	//kawachi 2020/10/19
	ui->btnRefresh->setEnabled(false);

	emit reqRefresh();
}

/*!
 * \brief MainController::on_btnAnalyze_clicked
 */
void MainController::on_btnAnalyze_clicked()
{
	//Let's analyze!!!
	//you can access data->BDs
	if(data->BDs.isEmpty()) return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	emit startAnalyze();
	data->isAnalyzed = false;
	data->isAnalyzing = true;

	ui->btnAnalyze->setEnabled(false);
	ui->btnAnalyze->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
	ui->saveBtn->setEnabled(true);
	ui->saveBtn->setStyleSheet("background-color: red; font-size: 2em; font-weight: bold;");

	//�O���b�h�i�q���̑��\�l���v�Z���܂�
	//for(grid y ...)
	//for(grid x ...)

	for(int i = 0; i < data->BDs.count(); i++){
		if(data->BDs[i].State == 0){
			data->BDs_White.push_back(data->BDs[i]);
		}
		else if(data->BDs[i].State == 1){
			data->BDs_Green.push_back(data->BDs[i]);
		}
		else if(data->BDs[i].State == 2){
			data->BDs_Yellow.push_back(data->BDs[i]);
		}
		else if(data->BDs[i].State  == 3){
			data->BDs_Orange.push_back(data->BDs[i]);
		}
		else if(data->BDs[i].State == 4){
			data->BDs_Red.push_back(data->BDs[i]);
		}
	}

	data->isAnalyzed = true;
	data->isAnalyzing = false;

	data->isShowMeasured = true;
	ui->chbxShowMeasured->setChecked(true);

	emit endAnalyze();
	QApplication::restoreOverrideCursor();
}

void MainController::CheckBox_Triggered(int idx, bool checked)
{
	data->isShowLevel_Y = ui->chbxShowLevel_Y->checkState();
	data->isShowLevel_R = ui->chbxShowLevel_R->checkState();
	data->isShowAnalyzed = ui->chbxShowAnalized->checkState();
	data->isShowMeasured = ui->chbxShowMeasured->checkState();
}

void MainController::on_pushButton_6_clicked()
{
	int res = QMessageBox::question(this,tr(""),tr("close?"));
	if(res == QMessageBox::Yes){
		emit window_close();
	}else{
		return;
	}
}

void MainController::on_saveBtn_clicked()
{
	emit saveimage_r();
	ui->saveBtn->setEnabled(false);
	ui->saveBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
}

void MainController::AnalyzeColorBtn()
{
	ui->btnAnalyze->setEnabled(true);
	ui->btnAnalyze->setStyleSheet("background-color: red; font-size: 2em; font-weight: bold;");
}

void MainController::RefrashColorBtn()
{
	ui->btnRefresh->setEnabled(true);
	//ui->btnRefresh->setStyleSheet("background-color: red; font-size: 2em; font-weight: bold;");
}
void MainController::btn_Refresh_Controll()
{
	ui->btnRefresh->setEnabled(false);
}
