#ifndef PLANEVIEWER_H
#define PLANEVIEWER_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <QPointF>

#include "../../Data/data.h"
#include "../../3rdParty/qcustomplot/qcustomplot.h"

//kinoshita
#include <opencv2/opencv.hpp>
#include "./Data/data.h"

namespace Ui {
class PlaneViewer;
}

class PlaneViewer : public QWidget
{
    Q_OBJECT

public:
    explicit PlaneViewer(QWidget *parent = 0);
    ~PlaneViewer();

    void initialize();

private slots:
    void addHammeringData(BDData_t &bd);
    void refresh();
    void setData(Data *data);
    void on_sldPntSz_valueChanged(int value);
    void saveMap(QString);

private:
    Ui::PlaneViewer *ui;

    QCPGraph *qcpgWhite;
    QCPGraph *qcpgGreen;
    QCPGraph *qcpgOrange;
    QCPGraph *qcpgYellow;
    QCPGraph *qcpgRed;

    QMap<int, QCPGraph*> qcpgsBD;

    //kinoshita_hoseihyouji
public slots:
//    void setBack(cv::Mat *img);
    void setBack(cv::Mat img);
    void scale(double x,double y);
};

#endif // PLANEVIEWER_H
