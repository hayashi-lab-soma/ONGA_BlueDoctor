#ifndef PLANEVIEWER_H
#define PLANEVIEWER_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <QPointF>

#include "../../Data/data.h"
#include "../../3rdParty/qcustomplot/qcustomplot.h"

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

    double x,y1,y2,x2;

public slots:
		//void addHammeringData(BDData_t &bd);
        void refresh();
		void plot_select(Data *data);
		void replot();
		void setData(Data *data);
		void on_sldPntSz_valueChanged(int value);
		void saveMap(QString);
        void Display_filename(QString file);


private:
    Ui::PlaneViewer *ui;

    QCPGraph *qcpgWhite;
    QCPGraph *qcpgGreen;
    QCPGraph *qcpgOrange;
    QCPGraph *qcpgYellow;
    QCPGraph *qcpgRed;
    QCPGraph *qcpgPredict;

    QMap<int, QCPGraph*> qcpgsBD;

public slots:
//    void setBack(cv::Mat *img);
    void setBack(cv::Mat img);
    void scale(double x,double y1, double y2, double x2);
		//2.3.3
		void addHammeringData(BDData_t &bd);
};

#endif // PLANEVIEWER_H
