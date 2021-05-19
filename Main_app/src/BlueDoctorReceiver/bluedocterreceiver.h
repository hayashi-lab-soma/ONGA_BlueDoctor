#ifndef BLUEDOCTERRECEIVER_H
#define BLUEDOCTERRECEIVER_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QThread>
#include <QByteArray>
#include <QChar>
#include <QTableWidget>
#include <QTimer>
#include <QTime>
//#include <QRandomGenerator>

#include "../Config/config.h"
#include "../Data/data.h"
#include "../Alarm/alarm.h"

#define PACKET_SIZE			(int)9

#define POS_NOTIF_TYPE	(int)0
#define POS_NOTIF_NO		(int)1
#define POS_NOTIF_DATA	(int)2
#define POS_NOTIF_STATE (int)3
#define POS_NOTIF_SENSE	(int)5
#define POS_TERMINATE		(int)8

#define LEN_SENSE (int)3

namespace NOTIF_CODE
{
namespace TYPE
{
const char ADDR				= 0x30;
const char START_STOP	= 0x31;
const char HAMMERING	= 0x32;
const char ALERM			= 0x33;
}

namespace ADDR
{
const char NOTICE			= 0x31;
const char RESPONCE		= 0x32;
}

namespace START
{
const char START			= 0x31;
const char STOP				= 0x32;
}

namespace STATE
{
const char NO_BLINK = 0x30;
const char GREEN		= 0x31;
const char YELLOW		= 0x32;
const char ORANGE		= 0x33;
const char RED			= 0x34;
}

namespace ALERM
{
const char PUSH_ERROR			= 0x31;
const char HAMMER_ERROR	= 0x32;
const char ERROR_OCCURED	= 0x31;
const char ERROR_RECOVERY	= 0x32;
}

const char FREE				= 0x30;
const char TERMINATE	= 0x0d;
}

namespace Ui {
class BlueDocterReceiver;
}

class BlueDocterReceiver : public QWidget
{
    Q_OBJECT

public:
    explicit BlueDocterReceiver(QWidget *parent = 0);
    ~BlueDocterReceiver();

    void initGUI();
    int initialize(Data *data);
    void finalize();

    QFile resultFile;
    QFile resultFile2;

private slots:
    void read();
    void on_btnOpenClose_clicked();
    void on_btnStart_clicked();
    void on_btnStop_clicked();
    void refresh();

    void TEST();
    void lostBD();
    void BDsig_error();

    //void Hammering_error();

    void on_depth_valueChanged(double arg1);

    void on_ConnectErrorBtn_clicked();
    void on_LostErrorBtn_clicked();
    void on_SigErrorBtn_clicked();
    void on_SpeedErrorBtn_clicked();

    void ActAlarm_num(int);

    void on_angle_valueChanged(double arg1);

    void btn_Stop_Controll();




    void on_HammeringErrorBtn_clicked();

signals:
    void recievedHammeringData(BDData_t &bd);
    void Popup();
    void savemap(QString);
    void area_clear();

    void AnalyzeColor();
    void RefreshColor();
    //void start_lost();
    void btnRefresh_to_false();

private:
    int RestartSeaquence();		//-1:Failure, 0:Success
    void StartupSeaquence();	//

    int ReadPacket(int timeout, QByteArray &recv);
    void ReadHammeringCommand(QByteArray pack);
    void ReadAlermCommand(QByteArray pack);
    bool BD_Point_Jump_Blocker(QPointF data);

    int WriteResponsCommand();
    int WriteStartCommand();
    int WriteStopCommand();

private:
    Ui::BlueDocterReceiver *ui;
    Config *cfg;
    Data *data;
    QFile *log;
    QTextStream *ostrError;
    Alarm *alarm;

    QSerialPort *comBD;

    QByteArray commResponse;
    QByteArray commStart, commStop;
    QDateTime t_last;

    bool isTest;
    QTimer *timer;
    double SData_x[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
    double SData_y[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
    int logCount = 0;
    int Alarm_num = 1;

    int Point_Jump_1st_Check = 0;
    QPointF New_Data = {0.0, 0.0};
    QPointF Old_Data = {0.0, 0.0};


protected:
    //void QIODevice::setOpenMode(QIODevice::OpenMode);
};

#endif // BLUEDOCTERRECEIVER_H
