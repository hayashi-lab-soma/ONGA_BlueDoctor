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
	bgChbxShows->addButton(ui->chbxShowLevel_P);
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
    //change bools
	emit btnStop_to_false();
	data->isRefresh = true;
	data->isAnalyzing = false;
	data->isAnalyzed = false;

    //clear data
    data->BDs.clear();
    data->BDs_Green.clear();
    data->BDs_Orange.clear();
    data->BDs_Predict.clear();
    data->BDs_Red.clear();
    data->BDs_White.clear();
    data->BDs_Yellow.clear();

	ui->btnAnalyze->setEnabled(false);
	ui->btnAnalyze->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
	ui->saveBtn->setEnabled(false);
	ui->saveBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");

	ui->btnRefresh->setEnabled(false);

	emit reqRefresh();
}

/*!
 * \brief MainController::on_btnAnalyze_clicked
 */
//解析ボタン
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

		if(data->BDs[i].State == BD_STATE_COLOR::White){
			data->BDs_White.push_back(data->BDs[i]);
		}
		else if(data->BDs[i].State == BD_STATE_COLOR::Green){
			data->BDs_Green.push_back(data->BDs[i]);
		}
		else if(data->BDs[i].State == BD_STATE_COLOR::Yellow){
			data->BDs_Yellow.push_back(data->BDs[i]);
		}
		else if(data->BDs[i].State  == BD_STATE_COLOR::Orange){
			data->BDs_Orange.push_back(data->BDs[i]);
		}
		else if(data->BDs[i].State == BD_STATE_COLOR::Red){
			data->BDs_Red.push_back(data->BDs[i]);
        }
//        else if(data->BDs[i].State == BD_STATE_COLOR::Predict){
//            data->BDs_Predict.push_back(data->BDs[i]);
//        }
	}

    Infinity_Judge();

	qDebug() << "Finished Analyze";
	emit finishedAnalyze();

	data->isAnalyzed = true;
	data->isAnalyzing = false;

	data->isShowMeasured = true;
	ui->chbxShowMeasured->setChecked(true);

	QApplication::restoreOverrideCursor();
}

//表示するレベル選択
void MainController::CheckBox_Triggered(int idx, bool checked)
{
	data->isShowLevel_Y = ui->chbxShowLevel_Y->checkState();
	data->isShowLevel_R = ui->chbxShowLevel_R->checkState();
	data->isShowAnalyzed = ui->chbxShowAnalized->checkState();
	data->isShowMeasured = ui->chbxShowMeasured->checkState();
	data->isShowLevel_P = ui->chbxShowLevel_P->checkState();
	qDebug() << data->isShowLevel_P << data->isShowLevel_Y << data->isShowLevel_R << data->isShowAnalyzed << data->isShowMeasured;
	emit show_plnvwr();
}
//終了ボタン
void MainController::on_pushButton_6_clicked()
{
	int res = QMessageBox::question(this,tr(""),tr("close?"));
	if(res == QMessageBox::Yes){
		emit window_close();
	}else{
		return;
	}
}
//保存ボタン
void MainController::on_saveBtn_clicked()
{
	emit saveimage_r(data->measurementLogDirName);
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

//Image Viewer とPlane Viewerの補間
void MainController::Infinity_Judge()
{

	for(int i = 0; i < data->BDs.count(); i++){

		//ここで補間するかどうかの判断
		if(data->BDs[i].XY.x() == BD_STATE_COLOR::VISION_DETECTION_ERROR){
            if(cnt_switch == true){
				color_array[_INF_count] = data->BDs[i].State;
				_INF_count = _INF_count + 1;
				interpolation = false;

            }else{
                continue;//最初にBDの検出ができていないとき
            }
		}else{

			//正しく記録されている
			//-inf　抜けたら補間
			if(interpolation == false){
                //終点
                last_pix = data->BDs[i].UV;//[px]2
                last = data->BDs[i].XY;//[m]

                float x_interval_pix = (last_pix.x() - first_pix.x())/(_INF_count+1);//[px]
				float y_interval_pix = (last_pix.y() - first_pix.y())/(_INF_count+1);
                float x_interval = (last.x() - first.x())/(_INF_count+1);//[m]
				float y_interval = (last.y() - first.y())/(_INF_count+1);

                for(int num = 0; num < _INF_count; num++){ //補間の位置を一つずつ計算
                    int x_pos_pix = first_pix.x() + x_interval_pix*(num+1);//[px]
					int y_pos_pix = first_pix.y() + y_interval_pix*(num+1);
                    float x_pos = first.x() + x_interval*(num+1);//[m]
					float y_pos = first.y() + y_interval*(num+1);

                    //配列に保存
                    est_array_pix[num].setX(x_pos_pix);
					est_array_pix[num].setY(y_pos_pix);
					est_array[num].setX(x_pos);
					est_array[num].setY(y_pos);


				}
				for(int j = 1; j <= _INF_count; j++){
                    //BDのデータに埋め込む
					data->BD.UV = est_array_pix[j-1];
					data->BD.XY = est_array[j-1];
                    data->BD.State = BD_STATE_COLOR::Predict;
                    //to draw plane viewer(plot blue points)
                    emit calc_pre(data->BD);
                    //to drew image viewer(plot predict points by colors)
                    data->BD.State = color_array[j-1];
                    data->BDs_Predict.push_back(data->BD);
//                    data->BDs[i-_INF_count+j-1] = data->BD;


				}
				interpolation = true;
				_INF_count = 0;
			}
            //始点
            first_pix = data->BDs[i].UV;
            first = data->BDs[i].XY;

			cnt_switch =true;
		}
	}
	cnt_switch = false;

}
