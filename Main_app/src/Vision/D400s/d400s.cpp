#include "d400s.h"

D400s::D400s()
{
    rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);

    rsPipe = nullptr;
    rsCfg = nullptr;
}

int D400s::init()
{
    qInfo() << "Try to open RealSense device ...";
    try {
        rsPipe = new rs2::pipeline();
        rsCfg = new rs2::config();

        //enable streams
        rsCfg->disable_all_streams();
        rsCfg->enable_stream(RS2_STREAM_COLOR, rs2_format::RS2_FORMAT_RGB8, 30);
        rsCfg->enable_stream(RS2_STREAM_DEPTH, rs2_format::RS2_FORMAT_Z16, 30);

        //start device
        rs2::pipeline_profile profile = rsPipe->start(*rsCfg);

        align_to_depth = new rs2::align(RS2_STREAM_DEPTH);
        align_to_color = new rs2::align(RS2_STREAM_COLOR);
    }
    catch (const rs2::error &e) {
        qCritical() << "[RS Error]:" << e.get_failed_function().c_str();
        qCritical() << "[RS Error]:" << e.get_failed_args().c_str();
        qCritical() << "[RS Error]:" << e.what();
        return -1;
    }

    qInfo() << "Opened RGBD camera";
    return 0;
}

int D400s::getFrames(RS2::Frames_t &frames)
{
    try {
        rs2::frameset _rs_frames = rsPipe->wait_for_frames();

        rs2::frameset rs_frames = align_to_color->process(_rs_frames);
        rs2::video_frame video = rs_frames.get_color_frame();
        rs2::depth_frame depth = rs_frames.get_depth_frame();

        frames.scale = depth.get_units();

        frames.imgRGB = cv::Mat(video.get_height(),
                                video.get_width(),
                                CV_8UC3,
                                (void*)video.get_data()).clone();

        frames.imgDepth = cv::Mat(depth.get_height(),
                                  depth.get_width(),
                                  CV_16UC1,
                                  (void*)depth.get_data()).clone();

        frames.imgAlignedRGB = cv::Mat(video.get_height(),
                                       video.get_width(),
                                       CV_8UC3,
                                       (void*)video.get_data()).clone();
    }
    catch (const rs2::error &e) {
        qCritical() << "[RS Error]:" << e.get_failed_function().c_str();
        qCritical() << "[RS Error]:" << e.get_failed_args().c_str();
        qCritical() << "[RS Error]:" << e.what();
        return -1;
    }

    return 0;
}

/*!
 * \brief D400s::deproject
 * \param u
 * \param v
 * \param point
 * \return
 */
int D400s::deproject(int u, int v, float point[])
{
    try{
        float pixel[2] = {(float)u, (float)v};
        float d = depth_frame->get_distance(u, v);
        rs2_deproject_pixel_to_point(point,&intr_depth,pixel,d);
    }
    catch(rs2::error &e){
        qWarning() << e.what();
    }
    return 0;
}

