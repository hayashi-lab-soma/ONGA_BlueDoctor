
#include "planeviewer.h"
#include "ui_planeviewer.h"

#include <qdebug.h>
#include <regex>

using namespace std;

PlaneViewer::PlaneViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlaneViewer)
{
    ui->setupUi(this);

    //
    ui->qcp->xAxis->grid()->setPen(QPen(QColor(Qt::black)));
    ui->qcp->yAxis->grid()->setPen(QPen(QColor(Qt::black)));
    ui->qcp->xAxis->setLabel("x[m] 20cm/grid");
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

    for(int i = 0; i < NUM_BD_STATE_COLOR ; i++){
        qcpgsBD[i] = ui->qcp->addGraph();
    }

    qcpgsBD[BD_STATE_COLOR::White]->setLineStyle(QCPGraph::lsNone);
    qcpgsBD[BD_STATE_COLOR::Green]->setLineStyle(QCPGraph::lsNone);
    qcpgsBD[BD_STATE_COLOR::Yellow]->setLineStyle(QCPGraph::lsNone);
    qcpgsBD[BD_STATE_COLOR::Orange]->setLineStyle(QCPGraph::lsNone);
    qcpgsBD[BD_STATE_COLOR::Red]->setLineStyle(QCPGraph::lsNone);
    qcpgsBD[BD_STATE_COLOR::Predict]->setLineStyle(QCPGraph::lsNone);

    double size = 10;
    qcpgsBD[BD_STATE_COLOR::White]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(QColor(Qt::black)), QBrush(QColor(Qt::white)), size));
    qcpgsBD[BD_STATE_COLOR::Green]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(Qt::green), size));
    qcpgsBD[BD_STATE_COLOR::Yellow]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(QColor(Qt::black)), QBrush(QColor(Qt::yellow)), size));
    qcpgsBD[BD_STATE_COLOR::Orange]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(255, 140, 0), size));
    qcpgsBD[BD_STATE_COLOR::Red]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(Qt::red), size));
    qcpgsBD[BD_STATE_COLOR::Predict]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(0, 150, 255), size));

    ui->sldPntSz->setRange(1, 20);
//    ui->spnbxPntSz->setRange(1,20);
    ui->sldPntSz->setValue(10);
//    ui->spnbxPntSz->setValue(10);
}

PlaneViewer::~PlaneViewer()
{
    delete ui;
}

void PlaneViewer::initialize()
{
    return;
}

//add data and plot planeviewer
void PlaneViewer::addHammeringData(BDData_t &bd)
{
    double key = bd.XY.x();
    double value = bd.XY.y();
    qcpgsBD[bd.State]->addData(key, value);

    ui->qcp->replot();
//    if(bd.State == 5){
//        qDebug() << "plot points to plane";
//    }
}


//when push clear button
void PlaneViewer::refresh()
{
    Q_FOREACH(QCPGraph *g, qcpgsBD){
        g->data().data()->clear();//clear BD data
    }

    ui->qcp->replot();
}

void PlaneViewer::plot_select(Data *data)
{
    //choose plot type by "show level"
    if(!data->BDs.isEmpty()
            && (!data->isAnalyzing && data->isAnalyzed)){

        if(data->isShowLevel_P){
            qcpgsBD[BD_STATE_COLOR::White]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Yellow]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Orange]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Red]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Green]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Predict]->setVisible(true);
            this->replot();
        }
        if(data->isShowLevel_Y){
            qcpgsBD[BD_STATE_COLOR::White]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Yellow]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Orange]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Red]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Green]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Predict]->setVisible(false);
            this->replot();
        }
        if(data->isShowLevel_R){
            qcpgsBD[BD_STATE_COLOR::White]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Yellow]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Orange]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Red]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Green]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Predict]->setVisible(false);
            this->replot();
        }
        if(data->isShowAnalyzed){
            qcpgsBD[BD_STATE_COLOR::White]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Yellow]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Orange]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Red]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Green]->setVisible(false);
            qcpgsBD[BD_STATE_COLOR::Predict]->setVisible(false);
            this->replot();
        }
        if(data->isShowMeasured){
            qcpgsBD[BD_STATE_COLOR::White]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Yellow]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Orange]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Red]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Green]->setVisible(true);
            qcpgsBD[BD_STATE_COLOR::Predict]->setVisible(false);
            this->replot();
        }
    }
}

void PlaneViewer::replot()
{
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
    qcpgsBD[BD_STATE_COLOR::Predict]->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,QColor(Qt::blue),value));

}

void PlaneViewer::saveMap(QString savepath)
{
    //get dis info and to string
    double Y = y1 + y2;
    QString qstrX = QString::number(x,'f',3);
    QString qstrY = QString::number(Y,'f',3);
    qstrX.replace(1,1,"-");
    qstrY.replace(1,1,"-");
    QString disInfo = qstrX + "_" + qstrY;

    ui->qcp->saveJpg(savepath + "/map.jpg",1920,1080,1.0,-1); //save jpg

    //save map for paste
    ui->qcp->xAxis->grid()->setVisible(false);
    ui->qcp->yAxis->grid()->setVisible(false);
    ui->qcp->xAxis->setLabel("x[m] 50cm/grid");
    ui->qcp->saveJpg(savepath + "/noGridMap_" + disInfo + "_.jpg",1920,1080,1.0,-1); //save jpg (for paste)

    //return to original
    ui->qcp->xAxis->grid()->setVisible(true);
    ui->qcp->yAxis->grid()->setVisible(true);
    ui->qcp->xAxis->setLabel("x[m] 20cm/grid");

    ui->qcp->replot();
}

//set hosei image at back of qcp
void PlaneViewer::setBack(cv::Mat img)
{
    cv::Mat mat = img;
    QPixmap qpixmap = QPixmap::fromImage(QImage((unsigned char*) mat.data, mat.cols, mat.rows,
                                                static_cast<int>(mat.step), QImage::Format_RGB888));
    ui->qcp->axisRect()->setBackground(qpixmap);//set img at qcp back
    ui->qcp->axisRect()->setBackgroundScaledMode(Qt::IgnoreAspectRatio); //size -> biggest

    ui->qcp->replot();
}

void PlaneViewer::scale(double x,double y1, double y2, double x2)
{
    ui->qcp->xAxis->setRange(-x, x); //change qcp range(x)
    ui->qcp->yAxis->setRange(-y2, y1); //change qcp range(y)

    //copy distance info
    this->x = x;
    this->y1 = y1;
    this->y2 = y2;
    this->x2 = x2;

    QSharedPointer<QCPAxisTickerFixed> fixedTicker_x(new QCPAxisTickerFixed);
    ui->qcp->xAxis->setTicker(fixedTicker_x);
    fixedTicker_x->setTickStep(1);// tick -> 1[m] (x)

    QSharedPointer<QCPAxisTickerFixed> fixedTicker_y(new QCPAxisTickerFixed);
    ui->qcp->yAxis->setTicker(fixedTicker_y);
    fixedTicker_y->setTickStep(1);// tick -> 1[m] (y)

    ui->qcp->replot();
}

void PlaneViewer::Display_filename(QString file)
{
    QStringList _csv = file.split(QLatin1Char('/'));
    QString filename = _csv.last();
    QString Disp_ = "file: " + filename;
    ui->filename_label->setText(Disp_);
}
