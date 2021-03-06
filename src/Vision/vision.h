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


    cv::Mat *imgResult;

        double disTop,disBottom,disMiddle;

private:
    void hsvFilter(cv::Mat *output, HSV_Ranges_t range);
    void detection(cv::Mat *input, QPoint *point, QPointF *point_m, Compornent_t *max,
                   Compornent_t *cmp, int lost_num);
    void detection2(cv::Mat *input,Compornent_t *max);
    int DetectionBlueDoctor(cv::Mat *img, HSV_Ranges_t hsvRngs, double wall_depth,
                            QPointF &pos_m, QPoint &pos_pix);
    int Spoit(Data *data);
    void cv_mouseCallBack(int event, int x, int y, int flags, void *userdata);
    void DrawResults(Data *data);
    void DrawPointSet(cv::Mat& imgInoutput, std::vector<cv::Point> vecPoint, int radius, const cv::Scalar& color, int thickness, int lineType);


signals:
    void updatedRGB(cv::Mat*);
    void updatedImgBD(cv::Mat *img);
    void updatedImgGreen(cv::Mat *img);
    void lostError();

    void sendImg(cv::Mat);

public slots:
    void saveimage(QString);
		void saveimage_result(QString);
    void setAutoColorExposure();
    //void area_Clear();

    void getImg();



private:
    R200 *r200;
    D400s *d455;
    Config *cfg;
    R200Data_t r200Data;
    RS2::Frames_t rs2_frames;

    Compornent_t max_area_cmp;
    Compornent_t max_area_cmp_BD;	//at "t"
    Compornent_t max_area_cmp_Green;
    Compornent_t cmpBD;					//buffer
    Compornent_t cmpGreen;

    cv::Mat *imgBinGreen;
    cv::Mat *imgBinBD;
    //    cv::Mat *imgResult;
    cv::Mat *imgpic;

		cv::Mat lastimg;

    bool isWndSpoit;

    bool isTest;
    int lostBD_num = 0;
    int lostGreen_num = 0;
    QString savepath_R;
    QString saveDis;

    QString disT,disM,disB;

//    double range_y;
    int topL,bottomL;
    int white;



    //for BD position prediction
    QPointF posBD_m_buf[2]; //t-1, t-2

    bool detect_inside = false;
    bool fpsDown = true;

    int new_x,new_y,new_width,new_height;
    //check fps
    bool calc_fps = false;
    QTime before,after;
    int64 ms_dif;
    float fps;



private slots:
    void area_Clear();
    //void start_Lost();
};

#endif // VISION_H
