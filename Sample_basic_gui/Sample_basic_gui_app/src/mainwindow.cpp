#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	timer = new QTimer(this);
	timer->setInterval(33);
	connect(timer, SIGNAL(timeout()), this, SLOT(main()));
	timer->start();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::main()
{
	qInfo() << "timer call back";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	timer->stop();
	qInfo() << "Bye bye";
}

