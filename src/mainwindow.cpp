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

    //auto open depth csv
    openCSV();
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
    imgVwrBin = new ImageViewer("Green HSV", this);
//    imgVwrBD = new ImageViewer("BlueDoctor HSV", this);
    plnVwr = new PlaneViewer(this);

    spoit = new Spoit_ImageViewer("Spoit", this);
    hsvRngCont = new HSVRangeController(this);
//    bdRngCont = new HSVRangeController(this);
    camParamCont = new CameraParameterContoller(this);

    mainCont = new MainController(this);

    //initialize widgets

    spoit->initialize();
//    spoit->setWindowFlags(Qt::Window);
    spoit->setWindowFlags(Qt::Tool);
    spoit->close();

    imgVwrRGB->initialize(CV_8UC3, QImage::Format_RGB888);
    imgVwrBin->initialize(CV_8UC1, QImage::Format_Grayscale8);
//    imgVwrBD->initialize(CV_8UC1, QImage::Format_Grayscale8);
    plnVwr->initialize();

    int hsvRngType = 0;
    hsvRngCont->initialize(data, hsvRngType);
    hsvRngCont->setWindowFlags(Qt::Window);
    hsvRngCont->close();

//    int bdRngType = 1;
//    bdRngCont->initialize(data, bdRngType);
//    bdRngCont->setWindowFlags(Qt::Window);
//    bdRngCont->close();

    camParamCont->initialize(data);
    camParamCont->setWindowFlags(Qt::Window);
    camParamCont->close();

    mainCont->initialize(data);
    mainCont->setFirstWidget(bdrcvr);

    //Add MDI area as sub windows
    swImgVwrRGB = addMdiSubWindow(imgVwrRGB);
    swImgVwrBin = addMdiSubWindow(imgVwrBin);
//    swImgVwrBD = addMdiSubWindow(imgVwrBD);
    swPlnVwr = addMdiSubWindow(plnVwr);
    swMainCont = addMdiSubWindow(mainCont);

    //signal-slot Vision to GUI classes
    connect(vision, SIGNAL(updatedRGB(cv::Mat*)), imgVwrRGB, SLOT(setImage(cv::Mat*)));
    connect(vision, SIGNAL(updatedImgGreen(cv::Mat*)), imgVwrBin, SLOT(setImage(cv::Mat*)));
//    connect(vision, SIGNAL(updatedImgBD(cv::Mat*)), imgVwrBD, SLOT(setImage(cv::Mat*)));

    connect(vision, SIGNAL(sendImg(cv::Mat)), plnVwr , SLOT(setBack(cv::Mat)));
    connect(this,SIGNAL(revision()),vision,SLOT(getImg()));
    connect(this,SIGNAL(changeScale(double,double,double,double)),plnVwr,SLOT(scale(double,double,double,double)));

    /***
     * Action trigger slots
     ***/
//    connect(ui->actBDHSV, &QAction::triggered, this,
//            [=](){
//        bdRngCont->show();
//    });
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
        data->isShowBDRect = !checked;
    });
    connect(ui->actShowGrids, &QAction::triggered, this,
            [=](bool checked){
        data->isShowGrids = checked;
    });


    //signal-slot BlueDoctor to GUI classes
    connect(bdrcvr, SIGNAL(recievedHammeringData(BDData_t&)), plnVwr, SLOT(addHammeringData(BDData_t&)));
    connect(mainCont,SIGNAL(calc_pre(BDData_t&)),plnVwr,SLOT(addHammeringData(BDData_t&)));

    //signal-slot MainController to GUI classes
    connect(mainCont, SIGNAL(reqRefresh()), plnVwr, SLOT(refresh()));
    connect(mainCont, SIGNAL(reqRefresh()), bdrcvr, SLOT(refresh()));

    //kesitemita
    //    connect(this, SIGNAL(updateData(Data*)), plnVwr, SLOT(setData(Data*)));
    //kinoshita
    connect(bdrcvr,SIGNAL(main_call()),this,SLOT(vision_update()));

    //alarm
    connect(bdrcvr,SIGNAL(Popup()),this,SLOT(openPopup()));
    connect(vision,SIGNAL(lostError()),bdrcvr,SLOT(lostBD()));

    //save
    connect(bdrcvr,SIGNAL(savemap(QString)),plnVwr,SLOT(saveMap(QString)));
    connect(bdrcvr,SIGNAL(savemap(QString)),vision,SLOT(saveimage(QString)));
    connect(mainCont,SIGNAL(saveimage_r(QString)),vision,SLOT(saveimage_result(QString)));

    connect(mainCont,SIGNAL(window_close()),this,SLOT(close()));
    connect(bdrcvr,SIGNAL(area_clear()),vision,SLOT(area_Clear()));

    //changeBtnColor
    connect(bdrcvr,SIGNAL(AnalyzeColor()),mainCont,SLOT(AnalyzeColorBtn()));
    connect(bdrcvr,SIGNAL(RefreshColor()),mainCont,SLOT(RefrashColorBtn()));

    connect(bdrcvr,SIGNAL(start_lost()),vision,SLOT(start_Lost()));
    connect(this,SIGNAL(actAlarm_num(int)),bdrcvr,SLOT(ActAlarm_num(int)));

    connect(this,SIGNAL(display_filename(QString)),plnVwr,SLOT(Display_filename(QString)));



		connect(mainCont, &MainController::show_plnvwr, this,
						[=](){
			plnVwr->plot_select(data);
		});

    connect(this,SIGNAL(showdistance(double)),bdrcvr,SLOT(ShowDepth(double)));

    connect(this, &MainWindow::updateTime, this,
            [=](){
        QDateTime ct = QDateTime::currentDateTime();

        QString str = ct.toString("hh:mm:ss.zzz");
        ui->statusBar->showMessage(str);
    });

    setWindowState(Qt::WindowMaximized);
}

//kinoshita
void MainWindow::vision_update()
{
    vision->main(data);
}

void MainWindow::main()
{
    QMetaObject::invokeMethod(timer, "stop");

    TRACE("--- main process ---");
    TRACE(QDateTime::currentDateTime().toString("hh-mm-ss-zzz"));

    //Vision main process
    vision->main(data);

    //kinoshita
//    if(!data->isMeasure){
//        vision->main(data);
//    }

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
//            data->BDs << data->BD;//data increase bug
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
//    swImgVwrBin->resize(ui->mdi->width()*0.18, ui->mdi->height()*0.35);
    swImgVwrBin->move(0, swImgVwrRGB->height());
//    swImgVwrBD->resize(ui->mdi->width()*0.25, ui->mdi->height()*0.35);
//    swImgVwrBD->resize(ui->mdi->width()*0.18, ui->mdi->height()*0.35);
//    swImgVwrBD->move(0,swImgVwrRGB->height());
//    swImgVwrBD->move(ui->mdi->width()*0.18,swImgVwrRGB->height());

    swPlnVwr->resize(ui->mdi->width()*0.6, ui->mdi->height()*0.65);
    swPlnVwr->move(swImgVwrRGB->width(), 0);

    swMainCont->resize(ui->mdi->width()*0.75, ui->mdi->height()*0.35);
    swMainCont->move(ui->mdi->width()*0.25, ui->mdi->height()*0.65);
//    swMainCont->resize(ui->mdi->width()*0.64, ui->mdi->height()*0.35);
//    swMainCont->move(ui->mdi->width()*0.36, ui->mdi->height()*0.65);
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
    //delete subwindow icon
    wnd->setWindowIcon( QIcon(QPixmap(1,1)) );
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    wnd->show();

    return wnd;
}

void MainWindow::start()
{
    //start main thread
    thMain = new QThread();
    thMain->start();

    timer = new QTimer();
    timer->setInterval(cfg->getInteger("MAIN", "INTERVAL"));
    timer->moveToThread(thMain);

    //kawachi
//    switch(data->isMeasure) {
//    case false:
//        connect(timer, SIGNAL(timeout()),
//                this, SLOT(main()),
//                Qt::DirectConnection);
//        qDebug() << "1";
//        break;
//    case true:
//        connect(bdrcvr, SIGNAL(main_call()),
//                this, SLOT(main()),
//                Qt::DirectConnection);
//        qDebug() << "2";
//        break;
//    default:
//        break;
//    }
    connect(timer, SIGNAL(timeout()),
            this, SLOT(main()),
            Qt::DirectConnection);



    QMetaObject::invokeMethod(timer, "start");
    qInfo() << "--- Start Main Process ---";
}

//void MainWindow::on_actSpoitBlue_triggered()
//{
//    data->isSpoit = true;
//    //color_tag = 0(Blue)
//    spoit->color_tag = 0;

//    QDesktopWidget dw;
//    QRect mainScreen = dw.availableGeometry(dw.primaryScreen());
//    //        spoit->resize(mainScreen.width()*0.5,mainScreen.height());
//    //        spoit->move(mainScreen.width()*0.5,0);
//    spoit->resize(mainScreen.width()*0.6,mainScreen.height()*0.6);
//    spoit->move(mainScreen.width()*0.4,mainScreen.height()*0.25);
//    spoit->setImage((vision->imgResult));
//    spoit->show();

//    connect(spoit,SIGNAL(fuzzySelectedBlue(cv::Range,cv::Range,cv::Range)),this, SLOT(SetHSVRanges(cv::Range,cv::Range,cv::Range)));
//    disconnect();
//}

void MainWindow::on_actSpoitGreen_triggered()
{
    data->isSpoit = true;
    //color_tag = 1(Green)
    spoit->color_tag = 1;

    QDesktopWidget dw;
    QRect mainScreen = dw.availableGeometry(dw.primaryScreen());
    spoit->resize(mainScreen.width()*0.6,mainScreen.height()*0.6);
    spoit->move(mainScreen.width()*0.4,mainScreen.height()*0.25);
    spoit->setImage((vision->imgResult));
    spoit->show();

    connect(spoit,SIGNAL(fuzzySelectedGreen(cv::Range,cv::Range,cv::Range)),this, SLOT(SetHSVRanges(cv::Range,cv::Range,cv::Range)));
    disconnect();
}

void MainWindow::SetHSVRanges(cv::Range h,cv::Range s,cv::Range v){

    switch (spoit->color_tag) {
    case 0:
        data->hsvRngsBD.H = h;
        data->hsvRngsBD.S = s;
        data->hsvRngsBD.V = v;

        cfg->setInteger("VISION","H_MIN_BLUE",data->hsvRngsBD.H.start);
        cfg->setInteger("VISION","H_MAX_BLUE",data->hsvRngsBD.H.end);
        cfg->setInteger("VISION","S_MIN_BLUE",data->hsvRngsBD.S.start);
        cfg->setInteger("VISION","S_MAX_BLUE",data->hsvRngsBD.S.end);
        cfg->setInteger("VISION","V_MIN_BLUE",data->hsvRngsBD.V.start);
        cfg->setInteger("VISION","V_MAX_BLUE",data->hsvRngsBD.V.end);
        break;
    case 1:
        data->hsvRngsGreen.H = h;
        data->hsvRngsGreen.S = s;
        data->hsvRngsGreen.V = v;

        cfg->setInteger("VISION","H_MIN_GREEN",data->hsvRngsGreen.H.start);
        cfg->setInteger("VISION","H_MAX_GREEN",data->hsvRngsGreen.H.end);
        cfg->setInteger("VISION","S_MIN_GREEN",data->hsvRngsGreen.S.start);
        cfg->setInteger("VISION","S_MAX_GREEN",data->hsvRngsGreen.S.end);
        cfg->setInteger("VISION","V_MIN_GREEN",data->hsvRngsGreen.V.start);
        cfg->setInteger("VISION","V_MAX_GREEN",data->hsvRngsGreen.V.end);
        break;
    }

}

void MainWindow::openCSV()
{
    QString FileName = "./../../../../../Result/Depth_Log/DepthLog.csv";

    if(FileName.isEmpty()) return;//when cansel
    QFile file(FileName);
    if(!file.open(QIODevice::ReadOnly) ){
        QMessageBox::information(this, tr("Unable to open file as read mode"), file.errorString());
        return;
    }//if error to read file

    emit display_filename(FileName);

    QTextStream in(&file);
    QString str;
    str = in.readAll();//all strings in file enter to "str"

    std::string stdstr = str.toStdString();
    stdstr = regex_replace(stdstr,regex("\r\n"),",");//change newline strings(\r\n) to ","
    str=QString::fromStdString(stdstr);
    QStringList list = str.split(',');//sprit string by ","

    //read csv contents
    data->divN = list.at(1).toInt();
    for(int n = 5;n < list.size()-1;n=n+2){
        data->dis[(n-5)/2] = list.at(n).toDouble();
    }

    //edge detection-----------------------------------
    double CenterDep[12];//because "max divN = 12"
    double core[16]; //depth array for certain wall
    double disTop, disMiddle, disBottom, PredictTop, PredictBottom;
    double MaxSlope = -INFINITY;
    double MinSlope = INFINITY;
    //if image donot contain edge
    int topRow = 0;
    int bottomRow = data->divN-1;

    if((data->divN % 2) == 0){ //even(guusuu)
        int half = data->divN/2;//easy to read code

        //get center depth data
        for(int n=0; n < data->divN; n++){
            int CenterDepN_L = (half)-1 + (n * data->divN);//center depth number_left
            int CenterDepN_R = CenterDepN_L + 1; //right
            CenterDep[n] = (data->dis[CenterDepN_L] + data->dis[CenterDepN_R]) / 2; //average (left + right)
        }
        disMiddle = (CenterDep[half-1] + CenterDep[half]) / 2;
        //if image donot contain edge
        disTop = CenterDep[0];
        disBottom = CenterDep[data->divN-1];

        //get certain wall[4*4] depth data into core
        for(int i=-2;i<=1;i++){
            for(int j=-2;j<=1;j++){
                core[(j+2) + (i+2)*4] = data->dis[((data->divN * data->divN) / 2) + data->divN/2 + (i*data->divN) + j];
            }
        }

        //get max and min slope
        for(int n=0; n<=11; n++){
            double slope = core[n] - core[n+4];//compare depth than upper grid
            if(MaxSlope < slope) MaxSlope = slope;
            if(MinSlope > slope) MinSlope = slope;
        }
        MaxSlope = MaxSlope + 0.011;//error range
        MinSlope = MinSlope - 0.011;
//        if(abs(MaxSlope) < 0.011) MaxSlope = 0.011; //error range
//        if(abs(MinSlope) < 0.011) MinSlope = -0.011;

        //get edge
        for(int n=2; n < half-1; n++){//get top edge
            double edgeJudge = CenterDep[half-n-1]-CenterDep[half-n]; //compare depth than upper grid
            if(edgeJudge > MaxSlope || edgeJudge < MinSlope){ //"Min < edgeJudge < Max" : not edge
                disTop = CenterDep[half-n];
                topRow = half-n;
//                qInfo() << "find top edge";
                break;
            }
        }

        for(int n=1; n < half-1; n++){//get bottom edge
            double edgeJudge = CenterDep[half+n]-CenterDep[half+n+1];
            if(edgeJudge > MaxSlope || edgeJudge < MinSlope){
                disBottom = CenterDep[half+n];
                bottomRow = half + n;
//                qInfo() << "find bottom edge";
                break;
            }
        }

        double ratio = (double)half / ((double)half-(double)topRow); //ratio for top edge
        PredictTop = disMiddle + (disTop - disMiddle)*ratio; //cal distance to top
        ratio = (double)half / ((double)bottomRow + 1 - (double)half); //ratio for bottom edge
        PredictBottom = disMiddle + (disBottom - disMiddle)*ratio; //cal distance to bottom

    }else{//odd(kisuu)
        int half = (data->divN-1)/2;//easy to read code

        //get center depth data
        for(int n=0; n < data->divN; n++){
            int CenterDepN = half + (n * data->divN);
            CenterDep[n] = data->dis[CenterDepN];
        }
        disMiddle = CenterDep[half];
        //if image donot contain edge
        disTop = CenterDep[0];
        disBottom = CenterDep[data->divN-1];

        //get certain wall[3*3] depth data into core
        for(int i=-1;i<=1;i++){
            for(int j=-1;j<=1;j++){
                core[(j+1) + (i+1)*3] = data->dis[((data->divN * data->divN -1) / 2) + (data->divN * i) +j];
            }
        }

        //get max and min slope
        for(int n=0; n<=5; n++){
            double slope = core[n] - core[n+3];
            if(MaxSlope < slope) MaxSlope = slope;
            if(MinSlope > slope) MinSlope = slope;
        }
        MaxSlope = MaxSlope + 0.011;//error range
        MinSlope = MinSlope - 0.011;

        for(int n=1; n < half; n++){//get top edge
            double edgeJudge = CenterDep[half-n-1]-CenterDep[half-n];
            if(edgeJudge > MaxSlope || edgeJudge < MinSlope){
                disTop = CenterDep[half-n];
                topRow = half-n;
//                qInfo() << "find top edge";
                break;
            }
        }

        for(int n=1; n < half; n++){//get bottom edge
            double edgeJudge = CenterDep[half+n]-CenterDep[half+n+1];
            if(edgeJudge > MaxSlope || edgeJudge < MinSlope){
                disBottom = CenterDep[half+n];
                bottomRow = half + n;
//                qInfo() << "find bottom edge";
                break;
            }
        }

        double ratio = (double)half / ((double)half - (double)topRow); //ratio for top edge
        PredictTop = disMiddle + (disTop - disMiddle)*ratio; //cal distance to top
        ratio = (double)half / ((double)bottomRow - (double)half); //ratio for bottom edge
        PredictBottom = disMiddle + (disBottom - disMiddle)*ratio; //cal distance to bottom
    }


//    double disT,disM,disB;//top,middle,bottom
//    int T,T1,T2,M,M1,M2,M3,M4,B,B1,B2;

//    if((data->divN % 2) == 0){//even(guusuu)
//        T1 = (data->divN/2)-1; //top left
//        T2 = data->divN/2; //top right
//        M1 = ((pow(data->divN,2))/2)-(data->divN/2)-1;
//        M2 = ((pow(data->divN,2))/2)-(data->divN/2);
//        M3 = ((pow(data->divN,2))/2)+(data->divN/2)-1;
//        M4 = ((pow(data->divN,2))/2)+(data->divN/2);
//        B1 = ((pow(data->divN,2))-1)-(data->divN/2);
//        B2 = pow(data->divN,2)-(data->divN/2);
//        disT = (data->dis[T1]+data->dis[T2])/2; //cal distance to top
//        disM = (data->dis[M1]+data->dis[M2]+data->dis[M3]+data->dis[M4])/4; //middle
//        disB = (data->dis[B1]+data->dis[B2])/2; //bottom
//    }else{//odd(kisuu)
//        T = (data->divN-1)/2;
//        M = (pow(data->divN,2)-1)/2;
//        B = (pow(data->divN,2))-(((data->divN)+1)/2);
//        disT = data->dis[T]; //top
//        disM = data->dis[M]; //middle
//        disB = data->dis[B]; //bottom
//    }
//    data->disM = disM;

    //copy parameter to hosei plogram's variable-------
    data->disM = disMiddle;
    double disT = PredictTop;
    double disM = disMiddle;
    double disB = PredictBottom;
    //-------------------------------------------------

    double degree=M_PI/180;
    double top = 2*tan(45.25*degree)*disT; //width at top[m]
    double bottom = 2*tan(45.25*degree)*disB; //width at middle[m]
    double middle = 2*tan(45.25*degree)*disM; //width at bottom[m]
    int topL = data->ImgCols * top/middle; //width at top[pixel]
    int bottomL = data->ImgCols * bottom/middle; // width at bottom[pixel]

    data->topL = topL;
    data->bottomL = bottomL;
    double range_x = top/2;

    //cal angle
    double angle, angRatio, dif;
    double n = 0.0;
    double difMin = INFINITY;
    double disRatio = disT / disB;
    while(1){
        angRatio = (1/cos((29.75+n)*degree)) / (1/cos((29.75-n)*degree)); // if (disRatio = angRatio) -> n = angle  // 1->depth to wall
        dif = disRatio - angRatio; //difference between angRatio and disRatio
        //smallest difference -> n = angle
        if(abs(dif) < abs(difMin)){
            difMin = dif;
        }else{
            angle = n - 0.1;
            break;
        }
        n = n+0.1;
    }

    //cal vertical range
    double right_angle_disT = disT * tan(29.75*degree);
    double range_y1 = right_angle_disT / cos(angle*degree);
    double right_angle_disB = disB * tan(29*degree);
    double range_y2 = right_angle_disB / cos(angle*degree);

//    qDebug() << "y1:" <<range_y1;
//    qDebug() << "y2:" <<range_y2;

    if(angle > 1){ //when camera have angle
        if(range_y2 > range_y1) range_y2 = range_y1; //dis info tolerance
    }else{ //when "angle = near 0" (donot have angle)
        range_y1 = disM * tan(29.75*degree);
        range_y2 = range_y1;
    }

    //debug-------------------------------------
//    for(int n = 0;n<data->divN;n++){qDebug() << n << " centerDep:" << CenterDep[n];}
//    for(int n = 0;n<16;n++){qDebug() << n << " core:" << core[n];}
//    qDebug() << "topRow:" <<topRow;
//    qDebug() << "BottomRow:" <<bottomRow;
//    qDebug() << "Max slope:" <<MaxSlope;
//    qDebug() << "Min slope:" <<MinSlope;
//    qDebug() << "disTop:" <<disTop;
//    qDebug() << "disMiddle:" <<disMiddle;
//    qDebug() << "disBottom:" <<disBottom;
//    qDebug() << "preTop:" <<PredictTop;
//    qDebug() << "preBottom:" <<PredictBottom;
//    qDebug() << "angle:" <<angle;
    //------------------------------------------

    Sleep(100);//avoid error

    emit revision();//hosei
    emit changeScale(range_x,range_y1,range_y2,bottom);
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
