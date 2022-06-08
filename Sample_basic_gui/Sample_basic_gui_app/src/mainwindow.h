#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QDebug>
#include <QString>
// for configuration
#include <QSettings>
// for multi thread
#include <QTimer>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	void setup();
	void start();

private slots:
	void main();

private:
	Ui::MainWindow *ui;

	QSettings *settings;

	QTimer *timer;
	QThread *thread;
	bool isThread;

protected:
	void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
