#ifndef D400S_H
#define D400S_H

#include <QObject>
#include <QDebug>
#include <QString>

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>

/*!
 * \brief The CamParam_t struct
 * Structure for camera parameters
 */
//#ifndef CAMPARAM_STRUCT
//#define CAMPARAM_STRUCT
//struct CamParams_t
//{
//	double saturation;
//	double whitebalance;
//	double gain;
//	double exposure;
//	double lr_gain;
//	double lr_exposure;
//};
//#endif


namespace RS2
{
struct Frames_t
{
    cv::Mat imgRGB;
    cv::Mat imgDepth;
    cv::Mat imgAlignedDepth;
    cv::Mat imgAlignedRGB;
    double scale;
};
}

class D400s
{
public:
    D400s();
    int init();
    int getFrames(RS2::Frames_t &frames);

private:
    rs2::pipeline *rsPipe;
    rs2::config *rsCfg;
    rs2::align *align_to_depth, *align_to_color;
};

#endif // D400S_H
