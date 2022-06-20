#include "bluedocterreceiver.h"
#include "ui_bluedocterreceiver.h"
#define _USE_MATH_DEFINES
#include<math.h>
#include<QDebug>

/*!
 * \brief BlueDocterReceiver::BlueDocterReceiver
 * \param parent
 */
BlueDocterReceiver::BlueDocterReceiver(QWidget *parent):
    QWidget(parent),
    ui(new Ui::BlueDocterReceiver)
{
    ui->setupUi(this);
    cfg = new Config(this);
    isTest = cfg->getBool("OPTION", "TEST");

    initGUI();

    comBD = new QSerialPort(this);
    comBD->setBaudRate(QSerialPort::Baud38400);
    comBD->setDataBits(QSerialPort::Data8);
    comBD->setParity(QSerialPort::NoParity);
    comBD->setStopBits(QSerialPort::OneStop);

    alarm = new Alarm(this);
    alarm->initialize();

    timer = new QTimer(this);
    timer->setInterval(10000);
    connect(timer,SIGNAL(timeout()),this,SLOT(BDsig_error()));


    //timer->setInterval(250);
    //connect(timer, SIGNAL(timeout()), this, SLOT(TEST()));

    //
    // Make commands as byte array
    //
    commResponse.append(NOTIF_CODE::TYPE::ADDR);
    commResponse.append(0x31);
    commResponse.append(NOTIF_CODE::ADDR::RESPONCE);
    commResponse.append(5, NOTIF_CODE::FREE);
    commResponse.append(NOTIF_CODE::TERMINATE);

    commStart.append(NOTIF_CODE::TYPE::START_STOP);
    commStart.append(0x31);
    commStart.append(NOTIF_CODE::START::START);
    commStart.append(5, NOTIF_CODE::FREE);
    commStart.append(NOTIF_CODE::TERMINATE);

    commStop.append(NOTIF_CODE::TYPE::START_STOP);
    commStop.append(0x31);
    commStop.append(NOTIF_CODE::START::STOP);
    commStop.append(5, NOTIF_CODE::FREE);
    commStop.append(NOTIF_CODE::TERMINATE);


    t_last = QDateTime::currentDateTime();

    ui->btnOpenClose->setStyleSheet("background-color: red; font-size: 2em; font-weight: bold;");
    ui->btnStart->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    ui->btnStop->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
}

BlueDocterReceiver::~BlueDocterReceiver()
{
    delete ui;
}

/*!
 * \brief BlueDocterReceiver::initGUI
 */
void BlueDocterReceiver::initGUI()
{
    Q_FOREACH(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        //		if(info.manufacturer() == "Vyacheslav Frolov")	continue;
        //		if(info.manufacturer() == "Microsoft")					continue;
        //			QString strCOM = info.portName() + QString("/")
        //					+ info.manufacturer() + QString("/")
        //					+ info.description();

        //ui->cmbbxCom->addItem(info.portName()); //ポ�?ト追��?


        if(!info.serialNumber().isEmpty()) {
            if(info.manufacturer() == QString("FTDI")){
                ui->cmbbxCom->addItem(info.portName()
                                      + " (ZigBee)");
            }
            else{
                ui->cmbbxCom->addItem(info.portName()
                                      + " (" + info.manufacturer() + ")");
            }
        }

    }

    ui->tbl->setHorizontalHeaderItem(0, new QTableWidgetItem("No."));
    ui->tbl->setHorizontalHeaderItem(1, new QTableWidgetItem("State"));
    ui->tbl->setHorizontalHeaderItem(2, new QTableWidgetItem("Sensor data"));
    ui->tbl->setHorizontalHeaderItem(3, new QTableWidgetItem("XY"));
    ui->tbl->adjustSize();

    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(false);

    ui->ConnectErrorBtn->setEnabled(false);
    ui->LostErrorBtn->setEnabled(false);
    ui->SigErrorBtn->setEnabled(false);
    ui->SpeedErrorBtn->setEnabled(false);
    ui->HammeringErrorBtn->setEnabled(false);
    ui->ConnectErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    ui->LostErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    ui->SigErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    ui->SpeedErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    ui->HammeringErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");

}

/*!
 * \brief BlueDocterReceiver::initialize
 * \param data
 * \return
 */
int BlueDocterReceiver::initialize(Data *data)
{
    this->data = data;

    data->measurementLogDirName = data->topLogsDirName;

    QDateTime ct = QDateTime::currentDateTime();
    data->measurementLogDirName += ct.toString("/hh_mm_ss");

    QDir dir; //make directory(result/date,time...)
    dir.mkdir(data->measurementLogDirName);

    QString path = data->measurementLogDirName + QString("/Alarm.txt");
    log = new QFile(path);
    bool ret = log->open(QFile::Text | QFile::Append);
    if(!ret){
        qCritical() << log->errorString();
        exit(1);
    }
    ostrError = new QTextStream(log);

    return 0;
}

void BlueDocterReceiver::finalize()
{
    if(comBD->isOpen()){
        comBD->close();
    }

    if(log->isOpen()){
        log->close();
    }
}

/*!
 * \brief BlueDocterReceiver::read
 */
void BlueDocterReceiver::read()
{
    if(comBD->bytesAvailable() < PACKET_SIZE) return;

    timer->start();

    QByteArray recv = comBD->readAll();
    QList<QByteArray> packs = recv.split(NOTIF_CODE::TERMINATE);

    if(packs.isEmpty()) return;

    Q_FOREACH(QByteArray pack, packs){
        if(pack.size() < (PACKET_SIZE)-1) continue;

        switch(pack.at(POS_NOTIF_TYPE)){
        case NOTIF_CODE::TYPE::HAMMERING:
            timer->start();
            ReadHammeringCommand(pack);
            return;

        case NOTIF_CODE::TYPE::ALERM:
            timer->start();
            ReadAlermCommand(pack);
            return;

        default:
            break;
        }
    } //Q_FOREACH(packs)
}

/*!
 * \brief BlueDocterReceiver::on_btnOpenClose_clicked
 */
//接続ボタン
void BlueDocterReceiver::on_btnOpenClose_clicked()
{
    qDebug() << "click";
    ui->btnOpenClose->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    ui->btnStart->setStyleSheet("background-color: red; font-size: 2em; font-weight: bold;");

    //QString commName = ui->cmbbxCom->currentText();
    QString commName = ui->cmbbxCom->currentText().remove(" (ZigBee)");

    if(commName == QString("")){
        QMessageBox::warning(this, QString("Warning"),
                             QString("Please select com port"));
        return;
    }

    comBD->setPortName(commName);

    //qDebug() << "comBD1" << comBD->open(QIODevice::ReadWrite);
    //qDebug() << "comBD2" << comBD->open(QIODevice::ReadWrite);
    //qDebug() << "comBD3" << comBD->open(QIODevice::ReadWrite);
    //2020/10/26
    //comBD->QIODevice::setOpenMode(QIODevice::ReadWrite);
    if(!comBD->open(QIODevice::ReadWrite)){
        QMessageBox::warning(this, QString("Warning"),
                             QString("Com port open error"));
        return;
    }

    //		Restart or Startup?
    ui->txtbrwsrMsg->append("Please startup Blue Doctor ...");
    ui->txtbrwsrMsg->repaint();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //int ret = RestartSeaquence();
    //if(ret == -1)	StartupSeaquence();

    StartupSeaquence();

    QApplication::restoreOverrideCursor();
    return;
}

/*!
 * \brief BlueDocterReceiver::RestartSeaquence
 * \return
 */
int BlueDocterReceiver::RestartSeaquence()
{
    //if(!comBD->isOpen()) return -1;

    //send start onshot
    WriteStartCommand();

    QByteArray recv;
    ReadPacket(1000, recv);

    if(recv.at(POS_NOTIF_TYPE) != NOTIF_CODE::TYPE::HAMMERING){
        return -1;
    }
    else{
        ui->txtbrwsrMsg->append("Already");
        WriteStopCommand();

        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(false);
    }

    return 0;
}

/*!
 * \brief BlueDocterReceiver::StartupSeaquence
 */
void BlueDocterReceiver::StartupSeaquence()
{

    QDateTime timestamp = QDateTime::currentDateTime();

    if(!comBD->isOpen()) return;

    QByteArray recv;
    //WriteStartCommand();
    //qDebug() << "readpacket :: " << ReadPacket(10000,recv);
    //qDebug() << "Alarm_num :: " << Alarm_num;
    if(ReadPacket(10000, recv) == -1 && Alarm_num == 1){

        /*
        ui->txtbrwsrMsg->append("Timeout error");
        comBD->close();
        QApplication::restoreOverrideCursor();
        emit Popup();
    */
        //play alerm sound
        alarm->playAlarm("Timeout error");
        ui->ConnectErrorBtn->setEnabled(true);
        ui->ConnectErrorBtn->setStyleSheet("background-color: lime; font-size: 2em; font-weight: bold;");

        //Log text output
        (*ostrError) << timestamp.toString("yyyy.MM.dd hh:mm::ss");//現在時間の書き込み
        (*ostrError) << "   ERROR_ALARM";
        (*ostrError) << "   Message : Timeout error" << endl;//エラー��?��の書き込み

        return;
    }

    if(recv.at(POS_NOTIF_TYPE) == NOTIF_CODE::TYPE::ADDR
            && recv.at(POS_NOTIF_DATA == NOTIF_CODE::ADDR::NOTICE)){
        ui->txtbrwsrMsg->append("Recieved address notice command");
    }
    else{
        ui->txtbrwsrMsg->append("Error");
    }
    //WriteStopCommand();
    //Send response command
    WriteResponsCommand();
    //ui->txtbrwsrMsg->append("Already");

    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
}

int BlueDocterReceiver::ReadPacket(int timeout, QByteArray &recv)
{
    int cnt = 0;
    int dt = 100;

    QByteArray _recv;

    while(1){

        if(cnt >= (timeout/dt)){
            return -1;
        }

        if(!comBD->waitForReadyRead(dt)){
            cnt++;
            continue;
        }

        if(comBD->bytesAvailable() < PACKET_SIZE){
            continue;
        }

        _recv.append(comBD->readAll());

        QList<QByteArray> packs = _recv.split(NOTIF_CODE::TERMINATE);

        if(packs.count() == 0) continue;
        else{
            Q_FOREACH(QByteArray pack, packs){
                if(pack.size() == (PACKET_SIZE-1)){
                    recv.append(pack);
                    return 0;
                }
            }
        }
    }

    return -1;
}

//
// ハンマリング通知が来た時に呼び出される関数
//
void BlueDocterReceiver::ReadHammeringCommand(QByteArray pack)
{

    //kawachi
//    data->alpha = QTime::currentTime();
//    Sleep(100);
//    emit main_call();
//    int sa = int(data->alpha.msecsTo(data->beta));
//    qDebug() << "sa:" << sa;
    //if(int LostAlarm = 0){
    //	LostAlarm =1;
    //	emit start_lost();
    //}

    QDateTime t = QDateTime::currentDateTime();

    QString str = QString("(%1) Recv:>").arg(QString::number(t_last.msecsTo(t)));
    str += QString(pack);
    ui->txtbrwsrMsg->append(str);

    //Logging Blue Doctor data
    data->BD.Timestamp = t;


    switch(pack.at(POS_NOTIF_DATA)){
    case NOTIF_CODE::STATE::NO_BLINK:
        data->BD.State = BD_STATE_COLOR::White;
        // Condition white
        // Counter and play alerm???
        break;

    case NOTIF_CODE::STATE::GREEN:
        data->BD.State = BD_STATE_COLOR::Green;
        break;

    case NOTIF_CODE::STATE::YELLOW:
        data->BD.State = BD_STATE_COLOR::Yellow;
        break;

    case NOTIF_CODE::STATE::ORANGE:
        data->BD.State = BD_STATE_COLOR::Orange;
        break;

    case NOTIF_CODE::STATE::RED:
        data->BD.State = BD_STATE_COLOR::Red;
        break;

    default:
        break;
    }

    //store BD Position from vision detection
    data->BD.XY = data->posBD_m;	//x,y [m]
    data->BD.Z = data->wall_depth;  //z(depth)[m]
    data->BD.UV = data->posBD_pix;	//u,v [pixel]

    data->BDs << data->BD; //copy BD data to BDs

    //アラーム
    if(Alarm_num == 1){
        for(int i = 4; i >=1; i--){

            SData_x[i] = SData_x[i-1];
            SData_y[i] = SData_y[i-1];
        }
        SData_x[0] = data->BD.XY.x();
        SData_y[0] = data->BD.XY.y();
        if((SData_x[0] != BD_STATE_COLOR::VISION_DETECTION_ERROR) && (SData_x[4] != BD_STATE_COLOR::VISION_DETECTION_ERROR)){

        if(SData_x[0] != 0.0 && SData_x[1] != 0.0 && SData_x[2] != 0.0 && SData_x[3] != 0.0 && SData_x[4] != 0.0
                && SData_y[0] != 0.0 && SData_y[1] != 0.0 && SData_y[2] != 0.0 && SData_y[3] != 0.0 && SData_y[4] != 0.0)
        {
            double Speed = sqrt((SData_x[0] - SData_x[4]) * (SData_x[0] - SData_x[4])
                    + (SData_y[0] - SData_y[4]) * (SData_y[0] - SData_y[4]));

            //qDebug() << "speed" << Speed;

            if(Speed >= 0.8){
                //qDebug() << "ERRORERROR";
                QDateTime timestamp = QDateTime::currentDateTime();

                //play alerm sound
                alarm->playAlarm("BDspeedError");
                ui->SpeedErrorBtn->setEnabled(true);
                ui->SpeedErrorBtn->setStyleSheet("background-color: lime; font-size: 2em; font-weight: bold;");

                if(logCount == 0){
                    //Log text output
                    (*ostrError) << timestamp.toString("yyyy.MM.dd hh:mm::ss");//現在時間の書き込み
                    (*ostrError) << "   ERROR_ALARM";
                    (*ostrError) << "   Message : BDspeedError" << endl;//エラー��?��の書き込み
                    logCount = 1;
                }
            }
        }
      }
      for (int i=0; i<5; i++){
          SData_x[i] = 0.0;
          SData_y[i] = 0.0;
      }
    }


    emit recievedHammeringData(data->BD);

    //update GUI
    int rowCnt = ui->tbl->rowCount();
    ui->tbl->insertRow(rowCnt);

    //qDebug()<<rowCnt<<data->BDs.size();

    QTableWidgetItem *twiNo = new QTableWidgetItem(QString("%1").arg(pack.at(POS_NOTIF_NO)));
    QTableWidgetItem *twiData = new QTableWidgetItem(QString("%1").arg(pack.at(POS_NOTIF_DATA)));
    QTableWidgetItem *twiSense = new QTableWidgetItem(QString(pack.mid(POS_NOTIF_SENSE, LEN_SENSE)));
    QTableWidgetItem *twiXY = new QTableWidgetItem(QString("%1,%2").arg(QString::number(data->BD.XY.x(),'f',3)).arg(QString::number(data->BD.XY.y(),'f',3)));
    twiNo->setTextAlignment(Qt::AlignHCenter);
    twiData->setTextAlignment(Qt::AlignHCenter);
    twiSense->setTextAlignment(Qt::AlignHCenter);
    twiXY->setTextAlignment(Qt::AlignHCenter);

    ui->tbl->setItem(rowCnt, 0, twiNo);
    ui->tbl->setItem(rowCnt, 1, twiData);
    ui->tbl->setItem(rowCnt, 2, twiSense);
    ui->tbl->setItem(rowCnt, 3, twiXY);

    ui->tbl->scrollToBottom();

    t_last = t;

    emit area_clear();
}

/*!
 * \brief BlueDocterReceiver::ReadAlermCommand
 * \param pack
 */
void BlueDocterReceiver::ReadAlermCommand(QByteArray pack)
{
    QDateTime t = QDateTime::currentDateTime();
    QString str = QString("(%1) Recv:>").arg(QString::number(t_last.msecsTo(t)));
    str += QString(pack);
    ui->txtbrwsrMsg->append(str);

    //----------------------------------------------
    // Performance for each error
    //----------------------------------------------
    QDateTime timestamp = QDateTime::currentDateTime();

    if(Alarm_num == 1){
        switch(pack.at(POS_NOTIF_DATA)){
        case NOTIF_CODE::ALERM::PUSH_ERROR:
            //play alerm sound
            alarm->playAlarm("pushError");

            //Log text output
            (*ostrError) << timestamp.toString("yyyy.MM.dd hh:mm::ss");//現在時間の書き込み
            (*ostrError) << "   ERROR_ALARM";
            (*ostrError) << "   Message : pushError" << endl;//エラー��?��の書き込み
            break;

        case NOTIF_CODE::ALERM::HAMMER_ERROR:
            //play alerm sound
            alarm->playAlarm("hammerError");
            ui->HammeringErrorBtn->setEnabled(true);
            ui->HammeringErrorBtn->setStyleSheet("background-color: lime; font-size: 2em; font-weight: bold;");

            //Log text output
            (*ostrError) << timestamp.toString("yyyy.MM.dd hh:mm::ss");//現在時間の書き込み
            (*ostrError) << "   ERROR_ALARM";
            (*ostrError) << "   Message : hanmmerError" << endl;//エラー��?��の書き込み
            break;

        default:
            break;
        }


        /*
         * アラーム通知の状態確認
         */
        switch(pack.at(POS_NOTIF_STATE)){
        default:
            break;
        }
    }

    t_last = t;
}

int BlueDocterReceiver::WriteResponsCommand()
{
    if(!comBD->isOpen()) return -1;
    comBD->write(commResponse);
    qDebug() << "response";

    if(!comBD->waitForBytesWritten(1000)){
        return -1;
    }
    return 0;
}

int BlueDocterReceiver::WriteStartCommand()
{
    if(!comBD->isOpen()) return -1;
    comBD->write(commStart);
    qDebug() << "start";

    if(!comBD->waitForBytesWritten(1000)){
        return -1;
    }
    return 0;
}

int BlueDocterReceiver::WriteStopCommand()
{
    if(!comBD->isOpen()) return -1;
    comBD->write(commStop);

    if(!comBD->waitForBytesWritten(1000)){
        return -1;
    }
    return 0;
}

void BlueDocterReceiver::on_btnStart_clicked()
{
    if(data->divN == NULL){//if donot open csv file
        QMessageBox::information(this,
                                 "Caution",
                                 "please open CSV file of depth");
        return;
    }

    emit btnRefresh_to_false();
    if(isTest){
        data->isMeasure = true;
        timer->start();
    }

    if(WriteStartCommand() == 0){
        data->isMeasure = true;
        ui->btnStart->setEnabled(false);
        ui->btnStop->setEnabled(true);
        ui->btnOpenClose->setEnabled(false);
//        ui->depth->setEnabled(false);
//        ui->angle->setEnabled(false);


        ui->btnStart->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
        ui->btnStop->setStyleSheet("background-color: red; font-size: 2em; font-weight: bold;");

        //
        //Make log directory
        //
        //data->measurementLogDirName = data->topLogsDirName;
        //QDateTime t = QDateTime::currentDateTime();
        //data->measurementLogDirName += t.toString("/hh_mm_ss");
        //
        //QDir dir;
        //dir.mkdir(data->measurementLogDirName);
        //
        //QString path = data->measurementLogDirName + QString("/Alarm.txt");
        //log = new QFile(path);
        //bool ret = log->open(QFile::Text | QFile::Append);
        //if(!ret){
        //	qCritical() << log->errorString();
        //	exit(1);
        //}
        //ostrError = new QTextStream(log);


        connect(comBD, SIGNAL(readyRead()), this, SLOT(read()));
    }
}

/*!
 * \brief BlueDocterReceiver::on_btnStop_clicked
 */
void BlueDocterReceiver::on_btnStop_clicked()
{
    if(WriteStopCommand() == 0){

        data->isMeasure = false;
        //Call Measurement state exit function

        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(false);
        ui->btnStart->setStyleSheet("background-color: red; font-size: 2em; font-weight: bold;");
        ui->btnStop->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");

        emit AnalyzeColor();
        emit RefreshColor();

        disconnect(comBD, SIGNAL(readyRead()), this, SLOT(read()));
        emit savemap(data->measurementLogDirName);

        //csv save
        std::string num; std::string x; std::string y; std::string z; std::string state; std::string u; std::string v;

        resultFile.setFileName(data->measurementLogDirName + QString("/result.csv"));
        if(resultFile.open(QIODevice::WriteOnly)){
            QTextStream out(&resultFile);
            for(int i = 0; i < data->BDs.count(); i++){
                if(i == 0){
                    out << "No." << "," << "x" << "," << "y" << "," << "z" << "," << "state" << "," << "u" << "," << "v" <<endl;
                }else{
                    num = std::to_string(i);
                    x = QString::number(data->BDs[i].XY.x()).toStdString();
                    y = QString::number(data->BDs[i].XY.y()).toStdString();
                    z = QString::number(data->BDs[i].Z).toStdString();
                    state = QString::number(data->BDs[i].State).toStdString();
                    double px_x = data->BDs[i].UV.x();//pixel u (848px or 640px)
                    double px_y = data->BDs[i].UV.y();
                    double px1920_x = px_x *(1920/data->ImgCols);//pixel u (1920px)
                    double px1080_y = px_y *(1080/data->ImgRows);
                    int px_u = px1920_x;//convert to int
                    int px_v = px1080_y;
                    u = QString::number(px_u).toStdString();
                    v = QString::number(px_v).toStdString();
                    if(px_u == 10000*(1920/data->ImgCols)){//when can't detect BD[px]
                        u = "-inf";
                        v = "-inf";
                    }
                    out << num.c_str() << "," << x.c_str() << "," << y.c_str() << "," << z.c_str() << "," << state.c_str() << "," << u.c_str() << "," << v.c_str() <<endl;
                }
            }
        }
        resultFile.close();


    }
}

void BlueDocterReceiver::refresh()
{
    ui->tbl->setRowCount(0);
    ui->tbl->repaint();
//    ui->depth->setEnabled(true);
//    ui->angle->setEnabled(true);
    ui->btnStart->setEnabled(true);
    ui->btnStart->setStyleSheet("background-color: red");
}

/*!
 * \brief BlueDocterReceiver::TEST
 */
void BlueDocterReceiver::TEST()
{
    //	int s = QRandomGenerator::global()->generate()%(NUM_BD_STATE_COLOR);
    //	s += 0x30;

    //	//Make test packet data
    //	QByteArray pack;
    //	pack.append(NOTIF_CODE::TYPE::HAMMERING);
    //	pack.append(0x31);
    //	pack.append(s);
    //	pack.append(5, NOTIF_CODE::FREE);

    //	ReadHammeringCommand(pack);
}

void BlueDocterReceiver::lostBD()
{
    if(data->isMeasure == true && Alarm_num == 1){
        QDateTime timestamp = QDateTime::currentDateTime();
        //play alerm sound
        alarm->playAlarm("lostError");
        ui->LostErrorBtn->setEnabled(true);
        ui->LostErrorBtn->setStyleSheet("background-color: lime; font-size: 2em; font-weight: bold;");

        if(logCount == 0){
            //Log text output
            (*ostrError) << timestamp.toString("yyyy.MM.dd hh:mm::ss");//現在時間の書き込み
            (*ostrError) << "   ERROR_ALARM";
            (*ostrError) << "   Message : LostError" << endl;//エラー��?��の書き込み
            logCount = 1;
        }
    }
}

void BlueDocterReceiver::BDsig_error()
{
    if(data->isMeasure == true && Alarm_num == 1){
        QDateTime timestamp = QDateTime::currentDateTime();
        //play alerm sound
        alarm->playAlarm("BDsigError");
        ui->SigErrorBtn->setEnabled(true);
        ui->SigErrorBtn->setStyleSheet("background-color: lime; font-size: 2em; font-weight: bold;");

        if(logCount == 0){
            //Log text output
            (*ostrError) << timestamp.toString("yyyy.MM.dd hh:mm::ss");//現在時間の書き込み
            (*ostrError) << "   ERROR_ALARM";
            (*ostrError) << "   Message : BDsig_Error" << endl;//エラー��?��の書き込み
            logCount = 1;
        }

    }
}

void BlueDocterReceiver::on_ConnectErrorBtn_clicked()
{
    alarm->stopAlarm("Timeout error");
    ui->ConnectErrorBtn->setEnabled(false);
    ui->ConnectErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    comBD->close();
}

void BlueDocterReceiver::on_LostErrorBtn_clicked()
{
    alarm->stopAlarm("Timeout error");
    ui->LostErrorBtn->setEnabled(false);
    ui->LostErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    logCount = 0;
}

void BlueDocterReceiver::on_SigErrorBtn_clicked()
{
    alarm->stopAlarm("Timeout error");
    ui->SigErrorBtn->setEnabled(false);
    ui->SigErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    logCount = 0;
}

void BlueDocterReceiver::on_SpeedErrorBtn_clicked()
{
    alarm->stopAlarm("Timeout error");
    ui->SpeedErrorBtn->setEnabled(false);
    ui->SpeedErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    logCount = 0;
}


void BlueDocterReceiver::on_HammeringErrorBtn_clicked()
{
    alarm->stopAlarm("Timeout error");
    ui->HammeringErrorBtn->setEnabled(false);
    ui->HammeringErrorBtn->setStyleSheet("background-color: gray; font-size: 2em; font-weight: bold;");
    logCount = 0;
}


void BlueDocterReceiver::ActAlarm_num(int alarm_num){
    Alarm_num = alarm_num;
}

void BlueDocterReceiver::btn_Stop_Controll()
{
    ui->btnStop->setEnabled(false);
    ui->btnStop->setStyleSheet("background-color: gray; font-size: 2em; font-wheight: bold;");
}



void BlueDocterReceiver::ShowDepth(double Dist){

//     ui->depth_label->setText(QString::number(Dist));

}
