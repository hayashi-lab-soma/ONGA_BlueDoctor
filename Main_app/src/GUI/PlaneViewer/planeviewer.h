#ifndef PLANEVIEWER_H
#define PLANEVIEWER_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <QPointF>

#include "../../Data/data.h"
#include "../../3rdParty/qcustomplot/qcustomplot.h"

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
};

#endif // PLANEVIEWER_H
