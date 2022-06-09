#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define NOMINMAX

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QMdiSubWindow>
#include <QThread>
#include <QTimer>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFileDialog>

#include "./GUI/ImageViewer/imageviewer.h"
#include "./GUI/PlaneViewer/planeviewer.h"
#include "./GUI/MainController/maincontroller.h"

#include "./GUI/HSVRangeController/hsvrangecontroller.h"
#include "./GUI/CameraParameterController/cameraparametercontoller.h"
#include "./GUI/Spoit_ImageViewer/spoit_imageviewer.h"

#include "./Config/config.h"
#include "./Data/data.h"
#include "./Vision/vision.h"
//#include "./FSM/fsm.h"
#include "./BlueDoctorReceiver/bluedocterreceiver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setup();	//Create initial GUI
    QMdiSubWindow* addMdiSubWindow(QWidget *widget);
    void start();

private slots:
    //kinoshita
    void vision_update();

    void main();

    void on_actAlarm_ON_triggered();

//    void on_actOpenDepthCSV_triggered();
    void on_actSpoitBlue_triggered();
    void on_actSpoitGreen_triggered();
    void SetHSVRanges(cv::Range h, cv::Range s, cv::Range v);
    void openPopup();
    //    void on_actShowGrids_triggered();

    //auto open depth csv
    void openCSV();

signals:
    void updateTime();
    void updateData(Data *data);
    void actAlarm_num(int);

    void showdistance(double);

    void revision();
    void changeScale(double,double,double,double);
    void display_filename(QString);

private:
    Ui::MainWindow *ui;
    QThread *thMain;
    QTimer *timer;

    Config *cfg;
    Data *data;
    Vision *vision;
    //    FSM *fsm;
    BlueDocterReceiver *bdrcvr;

    ImageViewer *imgVwrRGB;
    ImageViewer *imgVwrBin;
    ImageViewer *imgVwrBD;
    PlaneViewer *plnVwr;

    Spoit_ImageViewer *spoit;
    HSVRangeController *hsvRngCont;
    HSVRangeController *bdRngCont;
    CameraParameterContoller *camParamCont;
    MainController *mainCont;

    QMdiSubWindow *swImgVwrRGB;
    QMdiSubWindow *swImgVwrBin;
    QMdiSubWindow *swImgVwrBD;
    QMdiSubWindow *swPlnVwr;
    QMdiSubWindow *swMainCont;

    int actAlarm = 1;

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
