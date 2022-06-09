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
   int initialize();//������

private:
	 void writeErrorLog(QString);		//�A���[�������̃��O����������
	 void writeRecoverLog(QString);	//�A���[������~�������O�̏�������
signals:

public slots:
    void playAlarm(QString errormsg);//�A���[���̃��[�v�J�n
    void stopAlarm(QString erromsg);//�A���[���̃��[�v��~
    void play();//�A���[����炷
    void volumeUP();//�A���[���̑傫����傫���iMP3�̎��̂ݗL���j
    void volumeDWON();//�A���[���̑傫�����������iMP3�̎��̂ݗL���j
		void setAlarmMp3(QString alarmfile);//�A���[����ύX�������Ƃ�
    //playAlarm��stopAlarm��connect���ČĂяo��
		//volumeUP��volumeDOWN�����j���[�o�[�ŌĂяo��

		void show();

private:
    QMediaPlayer *player;
    QThread *alarm_Th;
    QTimer *timer;
		QMessageBox *Error;
    int volume;
};

#endif // ALARM_H
