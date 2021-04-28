
#include "planeviewer.h"
#include "ui_planeviewer.h"

PlaneViewer::PlaneViewer(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PlaneViewer)
{
	ui->setupUi(this);

	//
	ui->qcp->xAxis->grid()->setPen(QPen(QColor(Qt::black)));
	ui->qcp->yAxis->grid()->setPen(QPen(QColor(Qt::black)));
	ui->qcp->xAxis->setLabel("x[m]");
	ui->qcp->yAxis->setLabel("y[m]");
	ui->qcp->xAxis->setLabelFont(QFont("Consolas", 14, QFont::Bold));
	ui->qcp->yAxis->setLabelFont(QFont("Consolas", 14, QFont::Bold));
	ui->qcp->xAxis->setTickLabelFont(QFont("Consolas", 16, QFont::Bold));
	ui->qcp->yAxis->setTickLabelFont(QFont("Consolas", 16, QFont::Bold));
	ui->qcp->xAxis->grid()->setZeroLinePen(QPen(QColor(Qt::black)));
	ui->qcp->yAxis->grid()->setZeroLinePen(QPen(QColor(Qt::black)));

	//add
	ui->qcp->xAxis->grid()->setSubGridPen(QPen(QColor(Qt::black)));
	ui->qcp->yAxis->grid()->setSubGridPen(QPen(QColor(Qt::black)));
	ui->qcp->xAxis->grid()->setSubGridVisible(true);
	ui->qcp->yAxis->grid()->setSubGridVisible(true);

	ui->qcp->xAxis->setRange(-2.5, 2.5);	//default
	ui->qcp->yAxis->setRange(-2.0, 2.0);	//default

	ui->qcp->xAxis2->setVisible(true);
	ui->qcp->yAxis2->setVisible(true);
	ui->qcp->xAxis2->setTickLabels(false);
	ui->qcp->yAxis2->setTickLabels(false);

	for(int i = 0; i < NUM_BD_STATE_COLOR; i++){
		qcpgsBD[i] = ui->qcp->addGraph();
	}

	qcpgsBD[BD_STATE_COLOR::White]->setLineStyle(QCPGraph::lsNone);
	qcpgsBD[BD_STATE_COLOR::Green]->setLineStyle(QCPGraph::lsNone);
	qcpgsBD[BD_STATE_COLOR::Yellow]->setLineStyle(QCPGraph::lsNone);
	qcpgsBD[BD_STATE_COLOR::Orange]->setLineStyle(QCPGraph::lsNone);
	qcpgsBD[BD_STATE_COLOR::Red]->setLineStyle(QCPGraph::lsNone);


	double size = 10;
	qcpgsBD[BD_STATE_COLOR::White]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(QColor(Qt::black)), QBrush(QColor(Qt::white)), size));
	qcpgsBD[BD_STATE_COLOR::Green]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(Qt::green), size));
	qcpgsBD[BD_STATE_COLOR::Yellow]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(QColor(Qt::black)), QBrush(QColor(Qt::yellow)), size));
	qcpgsBD[BD_STATE_COLOR::Orange]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(255, 140, 0), size));
	qcpgsBD[BD_STATE_COLOR::Red]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(Qt::red), size));


	ui->sldPntSz->setRange(1, 20);
	ui->spnbxPntSz->setRange(1,20);
	ui->sldPntSz->setValue(10);
	ui->spnbxPntSz->setValue(10);
}

PlaneViewer::~PlaneViewer()
{
	delete ui;
}

void PlaneViewer::initialize()
{
	return;
}

void PlaneViewer::addHammeringData(BDData_t &bd)
{
	double key = bd.XY.x();
	double value = bd.XY.y();
	qcpgsBD[bd.State]->addData(key, value);
	ui->qcp->replot();
}

void PlaneViewer::refresh()
{
	Q_FOREACH(QCPGraph *g, qcpgsBD){
		g->data().data()->clear();
	}
	ui->qcp->replot();
}

void PlaneViewer::setData(Data *data)
{
	//Adjust x,y range by fov(Field-of-View)
	//	qDebug() << data->fov;
	ui->qcp->xAxis->setRange(data->fov.x(), data->fov.x() + data->fov.width());
	ui->qcp->yAxis->setRange(data->fov.y(), data->fov.y() + data->fov.height());
	ui->qcp->xAxis->ticker()->setTickCount(data->fov.width()/0.5);
	ui->qcp->yAxis->ticker()->setTickCount(data->fov.height()/0.5);
	ui->qcp->replot();

	//	if(data->ana_state == FSM_Values::Stanby){
	//	}

	//	if(data->ana_state == FSM_Values::Refresh){
	//	}

	return;
}

void PlaneViewer::on_sldPntSz_valueChanged(int value)
{
	qcpgsBD[BD_STATE_COLOR::White]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(QColor(Qt::black)), QBrush(QColor(Qt::white)), value));
	qcpgsBD[BD_STATE_COLOR::Green]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(Qt::green), value));
	qcpgsBD[BD_STATE_COLOR::Yellow]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(QColor(Qt::black)), QBrush(QColor(Qt::yellow)), value));
	qcpgsBD[BD_STATE_COLOR::Orange]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(255, 140, 0), value));
	qcpgsBD[BD_STATE_COLOR::Red]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(Qt::red), value));
}

void PlaneViewer::saveMap(QString savepath)
{
	ui->qcp->saveJpg(savepath + "/map" + ".jpg",1920,1080,1.0,-1);
}
