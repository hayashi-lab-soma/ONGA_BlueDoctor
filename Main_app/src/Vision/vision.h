#ifndef VISION_H
#define VISION_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QRectF>
//#include <QRandomGenerator>

#include "../Config/config.h"
#include "../Data/data.h"
#include "R200/r200.h"
#include "D400s/d400s.h"

struct Compornent_t
{
    QPoint tl;
    QSize size;
    double area;
    QPoint centroid;
};

class Vision : public QObject
{
    Q_OBJECT
public:
    explicit Vision(QObject *parent = nullptr);

    int initialize();
    void main(Data *data);
    void finalize();
    //void saveimage_result();

    void setCameraParam(Camera_Params_t camParams);

private:
    void hsvFilter(cv::Mat *output, HSV_Ranges_t range);
    int DetectionBlueDoctor(cv::Mat *img, HSV_Ranges_t hsvRngs, double wall_depth,
                            QPointF &pos_m, QPoint &pos_pix);
    int Spoit(Data *data);
    void cv_mouseCallBack(int event, int x, int y, int flags, void *userdata);
    void DrawResults(Data *data);

signals:
    void updatedRGB(cv::Mat*);
    void updatedImgBD(cv::Mat *img);
    void updatedImgGreen(cv::Mat *img);
    void lostError();

public slots:
    void saveimage(QString);
    void saveimage_result();
    void setAutoColorExposure();
    //void area_Clear();

private:
    R200 *r200;
    D400s *d455;
    Config *cfg;
    R200Data_t r200Data;
    RS2::Frames_t rs2_frames;

    Compornent_t max_area_cmp;	//at "t"
    Compornent_t cmpBD;					//buffer

    cv::Mat *imgBinGreen;
    cv::Mat *imgBinBD;
    cv::Mat *imgResult;
    cv::Mat *imgpic;

    bool isWndSpoit;

    bool isTest;
    int lostBD_num = 0;
    QString savepath_R;
    QString saveDis;

    QString disT,disM,disB;

private slots:
    void area_Clear();
    //void start_Lost();
};

#endif // VISION_H
