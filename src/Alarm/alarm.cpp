#include "alarm.h"

Alarm::Alarm(QObject *parent) : QObject(parent)
{
	//MP3�t�@�C�����g���ꍇ
	player = new QMediaPlayer();

	alarm_Th = new QThread();
	timer = new QTimer();
	Error = new QMessageBox();
	volume = 100;
}

int Alarm::initialize(){

	//MP3�t�@�C�����g���ꍇ//////////////////////////////
	//		QDir alarmfile = QDir::current();
	//		alarmfile.cdUp();
	//		player->setMedia(QUrl::fromLocalFile(alarmfile.path() + "/Ring01.wav"));
	//		//qDebug()<<QDir::currentPath();
	//    player->setVolume(volume);
	//    //MP3�t�@�C�����g���ꍇ//////////////////////////////

	//    timer->setInterval(1);//�^�C�}�[����1msec
	//    timer->moveToThread(alarm_Th);//�^�C�}�[��ʃX���b�h�Ɉړ�
	//    connect(timer,SIGNAL(timeout()),this,SLOT(play()),Qt::DirectConnection);//play()���^�C�}�[���؂�邲�ƂɌĂяo��
	//    alarm_Th->start();//�X���b�h�̋N��

	player->setMedia(QUrl("qrc:/SOUND/Ring01.wav"));
	player->setVolume(50);
	//	player->play();

	timer->setInterval(1);//�^�C�}�[����1msec
		timer->moveToThread(alarm_Th);//�^�C�}�[��ʃX���b�h�Ɉړ�
	connect(timer,SIGNAL(timeout()),this,SLOT(play()),Qt::DirectConnection);//play()���^�C�}�[���؂�邲�ƂɌĂяo��
		alarm_Th->start();//�X���b�h�̋N��
	//timer->start();
	return 0;
}

void Alarm::play(){
	//	QMetaObject::invokeMethod(timer,"stop");//�^�C�}�[�̒�~
	timer->stop();

	//MP3�t�@�C�����g���ꍇ
	player->play();

/*
	int res = Error->exec();
	if(res == QMessageBox::Yes){
		timer->stop();
		return;
	}
*/

	timer->start();
}

void Alarm::playAlarm(QString errormsg){
	//�G���[�\�����ɌĂяo��
	/*
	//popup
	Error->setWindowTitle("Warning");
	Error->setText(errormsg);
	Error->setIcon(QMessageBox::Warning);
	Error->setStandardButtons(QMessageBox::Yes);
	Error->setButtonText(QMessageBox::Yes,tr("Confimation"));
	Error->setBaseSize(600,600);

	QMetaObject::invokeMethod(timer,"start");//�^�C�}�[�N��//�A���[���̃��[�v���J�n

	int res = Error->exec();
	if(res == QMessageBox::Yes){
		timer->stop();
	*/

	QMetaObject::invokeMethod(timer,"start");//�^�C�}�[�N��//�A���[���̃��[�v���J�n

	//writeErrorLog(errormsg);//���O�L�^�̊֐�
	return;
}

void Alarm::stopAlarm(QString errormsg){
	QMetaObject::invokeMethod(timer,"stop");//�^�C�}�[��~//�A���[���̃��[�v���I��
	//writeRecoverLog(errormsg);//���O�L�^�̊֐�
	return;
}

void Alarm::writeErrorLog(QString errormsg){
	QDir currentDir = QDir::current();
	QFile LogFile(currentDir.path() + "/AlarmLog.txt");//AlarmLog.txt���쐬�E����ꍇ�͒ǉ���������

	QDateTime current = QDateTime::currentDateTime();//���݂̎��Ԏ擾

	LogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);//�t�@�C�����������݃��[�h�ŃI�[�v��
	QTextStream out(&LogFile);
	out<< current.toString("yyyy.MM.dd_hh:mm");//���ݎ��Ԃ̏�������
	out<< "   ERROR_ALARM"<<endl;
	out << "  Message : " + errormsg << endl;//�G���[���e�̏�������

	LogFile.close();
}

void Alarm::writeRecoverLog(QString errormsg){
	QDir currentDir = QDir::current();
	QFile LogFile(currentDir.path() + "/AlarmLog.txt");

	QDateTime current = QDateTime::currentDateTime();

	LogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	QTextStream out(&LogFile);
	out<< current.toString("yyyy.MM.dd_hh:mm");
	out<< "   ERROR_Recovery"<<endl;
	out << "  Message : " + errormsg << endl;

	LogFile.close();
}

void Alarm::volumeUP(){
	volume += 10;
	player->setVolume(volume);
}

void Alarm::volumeDWON(){
	volume -= 10;
	player->setVolume(volume);
}

void Alarm::setAlarmMp3(QString alarmfile){
	player->setMedia(QUrl::fromLocalFile(alarmfile));
}

void Alarm::show()
{
}

