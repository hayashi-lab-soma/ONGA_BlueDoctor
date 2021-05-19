#ifndef ALARM_H
#define ALARM_H

#include <QObject>
#include <QMultimedia>
#include <QMediaPlayer>
#include <QDir>
#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <windows.h>

class Alarm : public QObject
{
    Q_OBJECT
public:
	 explicit Alarm(QObject *parent = nullptr);
   int initialize();//初期化

private:
	 void writeErrorLog(QString);		//アラーム発生のログを書き込み
	 void writeRecoverLog(QString);	//アラームが停止したログの書き込み
signals:

public slots:
    void playAlarm(QString errormsg);//アラームのループ開始
    void stopAlarm(QString erromsg);//アラームのループ停止
    void play();//アラームを鳴らす
    void volumeUP();//アラームの大きさを大きく（MP3の時のみ有効）
    void volumeDWON();//アラームの大きさを小さく（MP3の時のみ有効）
		void setAlarmMp3(QString alarmfile);//アラームを変更したいとき
    //playAlarmとstopAlarmをconnectして呼び出す
		//volumeUPとvolumeDOWNをメニューバーで呼び出す

		void show();

private:
    QMediaPlayer *player;
    QThread *alarm_Th;
    QTimer *timer;
		QMessageBox *Error;
    int volume;
};

#endif // ALARM_H
