#ifndef DATA_H
#define DATA_H

#include <QDebug>
#include <QString>
#include <QSize>
#include <QPoint>
#include <QRectF>
#include <QDateTime>
#include <QMap>
#include <QDir>

#include <opencv2/opencv.hpp>
#include "../Config/config.h"

struct Camera_Params_t
{
    int color_exposure;
    int ir_exposure;
};

struct HSV_Ranges_t
{
    cv::Range H;
    cv::Range S;
    cv::Range V;
};

#define NUM_BD_STATE_COLOR (int)6
namespace BD_STATE_COLOR
{
const int White = 0;
const int Green = 1;
const int Yellow = 2;
const int Orange = 3;
const int Red = 4;
const int Predict = 5;

const float VISION_DETECTION_ERROR = -INFINITY;

  namespace CV_SCALAR
  {
  const cv::Scalar White = cv::Scalar::all(255);
  const cv::Scalar Green = cv::Scalar(0, 255, 0);
  const cv::Scalar Yellow = cv::Scalar(255, 255, 0);
  const cv::Scalar Orange = cv::Scalar(255, 140, 0);
  const cv::Scalar Red = cv::Scalar(255, 0, 0);
  const cv::Scalar Predict = cv::Scalar(0, 150, 255);
  }
}

struct BDData_t
{
    QDateTime Timestamp;
    int State;	//BD_STATE_COLOR::White, BD_STATE_COLOR::Green, ...
    QPointF XY;	//[m, m]
    double Z;		//[m]
    QPoint UV;	//[pixel, pixel]
};

class Data
{
public:
    Data();

    void makeMeasurementLogDir();
    void restoreConfig();

public:
    Config *cfg;
    QString topLogsDirName;
    QString measurementLogDirName;
    double fps;

    //For vision class
    QRectF fov;
    Camera_Params_t camParams;
    HSV_Ranges_t hsvRngsGreen;
    HSV_Ranges_t hsvRngsBD;
    QPointF posBD_m;
    QPoint posBD_pix;
    QPointF posGreen_m;
    QPoint posGreen_pix;
    double wall_depth;
    double grid_b_m;
    int grid_b_pix;

    //For Blue Doctor data
    BDData_t BD;					//current Blue Doctor data
    QList<BDData_t> BDs;	//Blue Doctor tracking data
    QMap<int, cv::Scalar*> mapBDState2cvScalar;


    //For analyze
    QList<BDData_t> BDs_White;
    QList<BDData_t> BDs_Green;
    QList<BDData_t> BDs_Yellow;
    QList<BDData_t> BDs_Orange;
    QList<BDData_t> BDs_Red;
    QList<BDData_t> BDs_Predict;

    //For Finite State Machine
    int state;

    //distance
    int divN = NULL;
    double dis[150];
    int topL,bottomL;
//    double range_x,range_y1,range_y2;
    double disM;
//    int ImgCols = 848;
//    int ImgRows = 640;
    int ImgCols = 640;
    int ImgRows = 360;

    //flags
    bool isCamParamChange;
    bool isSpoit;
    bool isMeasure, isRefresh;
    bool isAnalyzing, isAnalyzed;

    bool isShowBDRect;
    bool isShowGrids;
		bool isShowLevel_Y,isShowLevel_R, isShowLevel_P, isShowAnalyzed, isShowMeasured;

    QTime alpha,beta;
};

namespace State
{
const int Init = 0;
const int Stanby = 1;
const int Measurement	= 2;
const int Refresh = 3;
const int Analyze = 4;
}

#endif // DATA_H
