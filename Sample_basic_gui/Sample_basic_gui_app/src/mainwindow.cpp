#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define IS_TRACE 1
#if IS_TRACE==1
#define TRACE(msg) qDebug()<<msg
#elif IS_TRACE==0
#define TRACE(msg)
#endif

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	setup();
	start();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setup()
{
	settings = new QSettings("./config.ini",QSettings::IniFormat,this);
	TRACE("------------------------------");
	TRACE("Configurations");
	TRACE(settings->allKeys());
	settings->beginGroup("MAIN");
	int interval = settings->value("INTERVAL").toInt();
	settings->endGroup();
	TRACE("------------------------------");

	timer = new QTimer();
	timer->setInterval(interval);
	thread = new QThread();
}

void MainWindow::start()
{
	timer->moveToThread(thread);
	connect(timer, SIGNAL(timeout()), this, SLOT(main()),Qt::DirectConnection);

	isThread = true;
	thread->start();
	QMetaObject::invokeMethod(timer,"start");
}

void MainWindow::main()
{
	TRACE("------------------------------");
	QString strThID = QString("%1").arg((int)QThread::currentThreadId(),0,16);
	TRACE(QString("Timer call back: 0x" + strThID));
	TRACE("------------------------------");

	if(!isThread){
		timer->stop();
		thread->exit(0);
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QString strThID = QString("%1").arg((int)QThread::currentThreadId(),0,16);
	TRACE(QString("GUI thread: 0x" + strThID));

	isThread = false;
	thread->wait(1000);
	qInfo() << "Bye bye";
}

