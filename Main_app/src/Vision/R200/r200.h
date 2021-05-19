#ifndef R200_H
#define R200_H

#include <QObject>
#include <QDebug>
#include <QString>
#include <QSize>
#include <QRectF>
#include <QThread>
#include <QImage>

#include<librealsense/rs.hpp>	//R200を扱うためのライブラリ
#include <opencv2/opencv.hpp> //OpenCVライブラリ

//#include <pcl/point_cloud.h>
//#include <pcl/point_types.h>
//#include <pcl/io/ply_io.h>
//#include <pcl/common/transforms.h>

#define USE_INFRARED 0

//typedef pcl::PointXYZRGB PointT;
//typedef pcl::PointCloud<PointT> PointCloudT;

struct R200Data_t
{
    cv::Mat *imgRGB;
    cv::Mat *imgDepth;
    cv::Mat *imgInfrared;
    //	PointCloudT::Ptr pntCldPtr;
};

class R200 : public QObject
{
    Q_OBJECT
public:
    explicit R200(QObject *parent = nullptr);	//コンストラクタ

    int initialize(bool isUse = true);
    void concreate(R200Data_t &data);

    int startDevice();
    void finishDevice();

    int waitFrame();
    int getData(R200Data_t &r200Data);
    int getImage(QString qsStream, cv::Mat *img);

    //(u, v)[pixel, pixel] -> (x, y)[m, m]
    QPointF deprojectRGB(QPoint p, double);
    QPoint projectRGB(QPointF p, double depth);
    QRectF getFOV(double depth);
    double getPixelWidth(double meter, double depth);

    QPointF getCenter(double depth);

    //camera parameter change method
    void setAutoColorExposure();
    void setColorExposure(int value);
    void setIrExposure(int value);

private:
    int initDevice();										//デバイス初期化, rs::deviceの作成まで
    int initStreams();									//各ストリーム初期化
    int initIntrinsicsAndExtrinsics();	//

    //	int getPointCloud(PointCloudT::Ptr pointCloud);	//ポイントクラウド作成, 格納

private:
    bool isUse;

    static rs::context *rsCntxt;	//コンテキストオブジェクト
    rs::device *rsDev;						//デバイスオブジェクト
    QString qsSerial;							//デバイスのシリアルナンバー設定

    rs::intrinsics rsIntrRGB;						//組み込み関数
    rs::intrinsics rsIntrDepth;					//
    rs::intrinsics rsIntrInfrared;			//
    rs::extrinsics rsExtrDepthToColor;	//マッピング用組み込み関数

    float scale; //depth値の変換係数 16bit深度->[m]

public:
    QSize sizeImgRGB;
    QSize sizeImgDepth;
    QSize sizeImgInfrared;
};

namespace R200_IMG_TYPES
{
const QString Color= "Color";
const QString Depth = "Depth";
const QString IR = "IR";
}

#endif // R200_H
