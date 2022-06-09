#include "alarm.h"

Alarm::Alarm(QObject *parent) : QObject(parent)
{
	//MP3ファイルを使う場合
	player = new QMediaPlayer();

	alarm_Th = new QThread();
	timer = new QTimer();
	Error = new QMessageBox();
	volume = 100;
}

int Alarm::initialize(){

	//MP3ファイルを使う場合//////////////////////////////
	//		QDir alarmfile = QDir::current();
	//		alarmfile.cdUp();
	//		player->setMedia(QUrl::fromLocalFile(alarmfile.path() + "/Ring01.wav"));
	//		//qDebug()<<QDir::currentPath();
	//    player->setVolume(volume);
	//    //MP3ファイルを使う場合//////////////////////////////

	//    timer->setInterval(1);//タイマー周期1msec
	//    timer->moveToThread(alarm_Th);//タイマーを別スレッドに移動
	//    connect(timer,SIGNAL(timeout()),this,SLOT(play()),Qt::DirectConnection);//play()をタイマーが切れるごとに呼び出し
	//    alarm_Th->start();//スレッドの起動

	player->setMedia(QUrl("qrc:/SOUND/Ring01.wav"));
	player->setVolume(50);
	//	player->play();

	timer->setInterval(1);//タイマー周期1msec
		timer->moveToThread(alarm_Th);//タイマーを別スレッドに移動
	connect(timer,SIGNAL(timeout()),this,SLOT(play()),Qt::DirectConnection);//play()をタイマーが切れるごとに呼び出し
		alarm_Th->start();//スレッドの起動
	//timer->start();
	return 0;
}

void Alarm::play(){
	//	QMetaObject::invokeMethod(timer,"stop");//タイマーの停止
	timer->stop();

	//MP3ファイルを使う場合
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
	//エラー―発生に呼び出し
	/*
	//popup
	Error->setWindowTitle("Warning");
	Error->setText(errormsg);
	Error->setIcon(QMessageBox::Warning);
	Error->setStandardButtons(QMessageBox::Yes);
	Error->setButtonText(QMessageBox::Yes,tr("Confimation"));
	Error->setBaseSize(600,600);

	QMetaObject::invokeMethod(timer,"start");//タイマー起動//アラームのループを開始

	int res = Error->exec();
	if(res == QMessageBox::Yes){
		timer->stop();
	*/

	QMetaObject::invokeMethod(timer,"start");//タイマー起動//アラームのループを開始

	//writeErrorLog(errormsg);//ログ記録の関数
	return;
}

void Alarm::stopAlarm(QString errormsg){
	QMetaObject::invokeMethod(timer,"stop");//タイマー停止//アラームのループを終了
	//writeRecoverLog(errormsg);//ログ記録の関数
	return;
}

void Alarm::writeErrorLog(QString errormsg){
	QDir currentDir = QDir::current();
	QFile LogFile(currentDir.path() + "/AlarmLog.txt");//AlarmLog.txtを作成・ある場合は追加書き込み

	QDateTime current = QDateTime::currentDateTime();//現在の時間取得

	LogFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);//ファイルを書き込みモードでオープン
	QTextStream out(&LogFile);
	out<< current.toString("yyyy.MM.dd_hh:mm");//現在時間の書き込み
	out<< "   ERROR_ALARM"<<endl;
	out << "  Message : " + errormsg << endl;//エラー内容の書き込み

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

