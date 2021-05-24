#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <string>
#include <regex>
using namespace std;

#define IS_TRACE 0
#if IS_TRACE==1
#define TRACE(msg) qDebug()<<msg
#else
#define TRACE(msg)
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Load config
    cfg = new Config();

    //Create instance
    data = new Data();
    vision = new Vision();
    //    fsm = new FSM();
    bdrcvr = new BlueDocterReceiver(this);

    if(vision->initialize() == -1) exit(255);
    //    fsm->initialize();
    bdrcvr->initialize(data);

    //Setup GUIs
    setup();

    //Start main process
    start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * \brief MainWindow::initGUI
 */
void MainWindow::setup()
{
    //Create GUI instalce
    imgVwrRGB = new ImageViewer("RGB", this);
    imgVwrBin = new ImageViewer("Binary", this);
    plnVwr = new PlaneViewer(this);

    //    spoit = new Spoit_ImageViewer("Spoit", this);
    hsvRngCont = new HSVRangeController(this);
    camParamCont = new CameraParameterContoller(this);

    mainCont = new MainController(this);

    //initialize widgets

    //    spoit->initialize();
    //    spoit->setWindowFlags(Qt::Window);
    //    spoit->close();

    imgVwrRGB->initialize(CV_8UC3, QImage::Format_RGB888);
    imgVwrBin->initialize(CV_8UC1, QImage::Format_Grayscale8);
    plnVwr->initialize();

    hsvRngCont->initialize(data);
    hsvRngCont->setWindowFlags(Qt::Window);
    hsvRngCont->close();

    camParamCont->initialize(data);
    camParamCont->setWindowFlags(Qt::Window);
    camParamCont->close();

    mainCont->initialize(data);
    mainCont->setFirstWidget(bdrcvr);

    //Add MDI area as sub windows
    swImgVwrRGB = addMdiSubWindow(imgVwrRGB);
    swImgVwrBin = addMdiSubWindow(imgVwrBin);
    swPlnVwr = addMdiSubWindow(plnVwr);
    swMainCont = addMdiSubWindow(mainCont);

    //signal-slot Vision to GUI classes
    connect(vision, SIGNAL(updatedRGB(cv::Mat*)), imgVwrRGB, SLOT(setImage(cv::Mat*)));
    connect(vision, SIGNAL(updatedImgMask(cv::Mat*)), imgVwrBin, SLOT(setImage(cv::Mat*)));

    /***
     * Action trigger slots
     ***/
    connect(ui->actGreenHSV, &QAction::triggered, this,
            [=](){
        hsvRngCont->show();
    });
    connect(ui->actCamParam, &QAction::triggered, this,
            [=](){
        camParamCont->show();
    });
    connect(ui->actShowGreenTarget, &QAction::triggered, this,
            [=](bool checked){
        data->isShowBDRect = checked;
    });
    connect(ui->actShowGrids, &QAction::triggered, this,
            [=](bool checked){
        data->isShowGrids = checked;
    });


    //signal-slot BlueDoctor to GUI classes
    connect(bdrcvr, SIGNAL(recievedHammeringData(BDData_t&)), plnVwr, SLOT(addHammeringData(BDData_t&)));

    //signal-slot MainController to GUI classes
    connect(mainCont, SIGNAL(reqRefresh()), plnVwr, SLOT(refresh()));
    connect(mainCont, SIGNAL(reqRefresh()), bdrcvr, SLOT(refresh()));

    connect(this, SIGNAL(updateData(Data*)), plnVwr, SLOT(setData(Data*)));

    //alarm
    connect(bdrcvr,SIGNAL(Popup()),this,SLOT(openPopup()));
    connect(vision,SIGNAL(lostError()),bdrcvr,SLOT(lostBD()));

    //save
    connect(bdrcvr,SIGNAL(savemap(QString)),plnVwr,SLOT(saveMap(QString)));
    connect(bdrcvr,SIGNAL(savemap(QString)),vision,SLOT(saveimage(QString)));
    connect(mainCont,SIGNAL(saveimage_r()),vision,SLOT(saveimage_result()));

    connect(mainCont,SIGNAL(window_close()),this,SLOT(close()));
    connect(bdrcvr,SIGNAL(area_clear()),vision,SLOT(area_Clear()));

    //changeBtnColor
    connect(bdrcvr,SIGNAL(AnalyzeColor()),mainCont,SLOT(AnalyzeColorBtn()));
    connect(bdrcvr,SIGNAL(RefreshColor()),mainCont,SLOT(RefrashColorBtn()));

    connect(bdrcvr,SIGNAL(start_lost()),vision,SLOT(start_Lost()));
    connect(this,SIGNAL(actAlarm_num(int)),bdrcvr,SLOT(ActAlarm_num(int)));

    //button_process kawachi2020/10/21
    //    connect(mainCont,SIGNAL(btnStop_to_false()),bdRcvr,SLOT(btn_Stop_Controll()));
    //    connect(bdRcvr,SIGNAL(btnRefresh_to_false()),mainCont,SLOT(btn_Refresh_Controll()));

    connect(this, &MainWindow::updateTime, this,
            [=](){
        QDateTime ct = QDateTime::currentDateTime();

        QString str = ct.toString("hh:mm:ss.zzz");
        ui->statusBar->showMessage(str);
    });

    setWindowState(Qt::WindowMaximized);
}

void MainWindow::main()
{
    QMetaObject::invokeMethod(timer, "stop");

    TRACE("--- main process ---");
    TRACE(QDateTime::currentDateTime().toString("hh-mm-ss-zzz"));

    //Vision main process
    vision->main(data);

    //main process
    TRACE(QString("State:%1").arg(data->state));

    switch(data->state){
    case State::Stanby:
        //Transition?
        if(data->isMeasure) data->state = State::Measurement;
        break;
    case State::Measurement:
        //Transition?
        if(!data->isMeasure) data->state = State::Stanby;
        else{
            //do measurement process
            //push back BD data to list
            data->BDs << data->BD;
            TRACE(QString("BD position : %1, %2").arg(data->BD.XY.x()).arg(data->BD.XY.y()));
            TRACE(QString("BD data count : %1").arg(data->BDs.count()));
        }
        break;
    case State::Analyze:
        break;
    case State::Refresh:
        break;
    }

    //call PlaneViewer::setData()
    emit updateData(data);
    emit updateTime();

    QMetaObject::invokeMethod(timer, "start");

    return;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //Adjust size and position
    swImgVwrRGB->resize(ui->mdi->width()*0.4, ui->mdi->height()*0.65);
    swImgVwrRGB->move(0,0);
    swImgVwrBin->resize(ui->mdi->width()*0.25, ui->mdi->height()*0.35);
    swImgVwrBin->move(0, swImgVwrRGB->height());

    swPlnVwr->resize(ui->mdi->width()*0.6, ui->mdi->height()*0.65);
    swPlnVwr->move(swImgVwrRGB->width(), 0);

    swMainCont->resize(ui->mdi->width()*0.75, ui->mdi->height()*0.35);
    swMainCont->move(ui->mdi->width()*0.25, ui->mdi->height()*0.65);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //	timer->deleteLater();
    //	thMain->deleteLater();

    vision->finalize();
    bdrcvr->finalize();
    data->restoreConfig();
}



QMdiSubWindow* MainWindow::addMdiSubWindow(QWidget *widget)
{
    Q_FOREACH(const auto window, ui->mdi->subWindowList()){
        if(window->windowTitle() == widget->windowTitle()) return nullptr;
    }

    QMdiSubWindow *wnd = ui->mdi->addSubWindow(widget);
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    wnd->show();

    return wnd;
}

void MainWindow::start()
{
    thMain = new QThread();
    thMain->start();

    timer = new QTimer();
    timer->setInterval(cfg->getInteger("MAIN", "INTERVAL"));
    timer->moveToThread(thMain);

    connect(timer, SIGNAL(timeout()),
            this, SLOT(main()),
            Qt::DirectConnection);

    QMetaObject::invokeMethod(timer, "start");
    qInfo() << "--- Start Main Process ---";
}


void MainWindow::on_actSpoit_triggered()
{
    data->isSpoit = true;

    //spoit���ʂ̃T�C�Y�w��
    QDesktopWidget dw;
    QRect mainScreen = dw.availableGeometry(dw.primaryScreen());
    //    spoit->resize(mainScreen.width()*0.5,mainScreen.height());
    //    spoit->move(mainScreen.width()*0.5,0);
    //    spoit->show();

    //    connect(vision, SIGNAL(updatedRGB(cv::Mat*)), spoit, SLOT(setImage(cv::Mat*)));
    //    connect(spoit, SIGNAL(fuzzySelected(cv::Range,cv::Range,cv::Range)), this, SLOT(setHSVRanges(cv::Range,cv::Range,cv::Range)));
}

void MainWindow::setHSVRanges(cv::Range h, cv::Range s, cv::Range v)
{
    data->hsvRngsGreen.H = h;
    data->hsvRngsGreen.S = s;
    data->hsvRngsGreen.V = v;
}

void MainWindow::on_actOpenDepthCSV_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(this,
                                                    QString("Open Depth CSV File"),
                                                    QDir::rootPath(),
                                                    QString("CSV (*.csv);;"));

    if(FileName.isEmpty()) return;//when cansel
    QFile file(FileName);
    if(!file.open(QIODevice::ReadOnly) ){
        QMessageBox::information(this, tr("Unable to open file as read mode"), file.errorString());
        return;
    }//if error to read file

    QTextStream in(&file);
    QString str;
    str = in.readAll();//all strings in file enter to "str"

    std::string stdstr = str.toStdString();
    stdstr = regex_replace(stdstr,regex("\r\n"),",");//change newline strings(\r\n) to ","
    str=QString::fromStdString(stdstr);
    QStringList list = str.split(',');//sprit string by ","

    data->divN = list.at(1).toInt();
    data->countMax = list.at(3).toInt();
    for(int n = 5;n < list.size()-1;n=n+2){
        data->dis[(n-5)/2] = list.at(n).toDouble();
    }
    //    qDebug() << "FileName" << csvName;
    //    qInfo() << "result";

    double disT,disM,disB;
    int T,T1,T2,M,M1,M2,M3,M4,B,B1,B2;

    if((data->divN % 2) == 0){//even(guusuu)
        T1 = (data->divN/2)-1;
        T2 = data->divN/2;
        M1 = ((pow(data->divN,2))/2)-(data->divN/2)-1;
        M2 = ((pow(data->divN,2))/2)-(data->divN/2);
        M3 = ((pow(data->divN,2))/2)+(data->divN/2)-1;
        M4 = ((pow(data->divN,2))/2)+(data->divN/2);
        B1 = ((pow(data->divN,2))-1)-(data->divN/2);
        B2 = pow(data->divN,2)-(data->divN/2);
        disT = (data->dis[T1]+data->dis[T2])/2;
        disM = (data->dis[M1]+data->dis[M2]+data->dis[M3]+data->dis[M4])/4;
        disB = (data->dis[B1]+data->dis[B2])/2;
    }else{//odd(kisuu)
        T = (data->divN-1)/2;
        M = (pow(data->divN,2)-1)/2;
        B = (pow(data->divN,2))-(((data->divN)+1)/2);
        disT = data->dis[T];
        disM = data->dis[M];
        disB = data->dis[B];
    }

    data->disT = QString::number(disT);
    data->disM = QString::number(disM);
    data->disB = QString::number(disB);

    QMessageBox::information(this,
                             "CSV file open success",
                             file.fileName());
}


void MainWindow::openPopup()
{
    QString mess = "1234";
    //	alarm->playAlarm(mess);
}

void MainWindow::on_actAlarm_ON_triggered()
{
    if(actAlarm == 1){
        actAlarm = 0;
        emit actAlarm_num(0);
    }
    else{
        actAlarm = 1;
        emit actAlarm_num(1);
    }
}

//void MainWindow::on_actShowGrids_triggered()
//{
//    data->isShowGrids = !data->isShowGrids;
//}
