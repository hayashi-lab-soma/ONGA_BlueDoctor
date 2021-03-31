#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QDebug>
#include <QString>
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

private slots:
	void main();

private:
	Ui::MainWindow *ui;

	QTimer *timer;
	QThread *thread;

protected:
	void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
