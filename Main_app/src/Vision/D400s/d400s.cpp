#include "d400s.h"

D400s::D400s(bool isTest)
{
    this->isTest = isTest;
    rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);
    rsPipe = nullptr;
    rsCfg = nullptr;
}

int D400s::init()
{
    qInfo() << "Try to open RealSense device ...";
    if(isTest){
        qInfo() << "Launch test mode";
        return 0;
    }

    try {
        rsPipe = new rs2::pipeline();
        rsCfg = new rs2::config();

        //enable streams
        rsCfg->disable_all_streams();
        rsCfg->enable_stream(RS2_STREAM_COLOR,848, 480, rs2_format::RS2_FORMAT_RGB8, 30);
        rsCfg->enable_stream(RS2_STREAM_DEPTH,848,480, rs2_format::RS2_FORMAT_Z16, 30);

        //start device
        rs2::pipeline_profile profile = rsPipe->start(*rsCfg);

        align_to_depth = new rs2::align(RS2_STREAM_DEPTH);
        align_to_color = new rs2::align(RS2_STREAM_COLOR);

        _rs_frames = new rs2::frameset();
        rs_frames = new rs2::frameset();

        *_rs_frames = rsPipe->wait_for_frames();
        *rs_frames = align_to_color->process(*_rs_frames);

        video_frame = new rs2::video_frame(rs_frames->get_color_frame());
        depth_frame = new rs2::depth_frame(rs_frames->get_depth_frame());
        //        *video_frame = rs_frames->get_color_frame();
        //        *depth_frame = rs_frames->get_depth_frame();

        intr_depth = (profile.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>()).get_intrinsics();
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
    if(isTest){
        frames.imgRGB = cv::Mat(480,640,CV_8UC3,cv::Scalar(0,0,0));
        frames.imgDepth = cv::Mat(480,640,CV_8UC1,cv::Scalar(0));
        frames.imgAlignedRGB = cv::Mat(480,640,CV_8UC3,cv::Scalar(0,0,0));
        return 0;
    }

    try {
        *_rs_frames = rsPipe->wait_for_frames();
        *rs_frames = align_to_color->process(*_rs_frames);
        *video_frame = rs_frames->get_color_frame();
        *depth_frame = rs_frames->get_depth_frame();

        frames.scale = depth_frame->get_units();

        frames.imgRGB = cv::Mat(video_frame->get_height(),
                                video_frame->get_width(),
                                CV_8UC3,
                                (void*)video_frame->get_data()).clone();

        frames.imgDepth = cv::Mat(depth_frame->get_height(),
                                  depth_frame->get_width(),
                                  CV_16UC1,
                                  (void*)depth_frame->get_data()).clone();

        frames.imgAlignedRGB = cv::Mat(video_frame->get_height(),
                                       video_frame->get_width(),
                                       CV_8UC3,
                                       (void*)video_frame->get_data()).clone();
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

//kawachi
QRectF D400s::getFOV(double depth)
{
        QRectF fov;
        if(!isUse){
                fov = QRectF(-2.5, -2.0, 5.0, 4.0);
                return fov;
        }

        int height = video_frame->get_height();
        int width = video_frame->get_width();


        float xyz_bl[3] = {0.0};
        float uv_bl[2] = {0, (float)height};
        //rsIntrRGB.rs_intrinsics::model = (rs_distortion)rs::distortion::inverse_brown_conrady;

        rs2_deproject_pixel_to_point(xyz_bl, &intr_RGB, uv_bl, (float)depth);

        QPointF bl = QPointF(xyz_bl[0], xyz_bl[1]);


        float xyz_tr[3] = {0.0};
        float uv_tr[2] = {(float)width, 0};
        //rsIntrRGB.rs_intrinsics::model = (rs_distortion)rs::distortion::inverse_brown_conrady;

        rs2_deproject_pixel_to_point(xyz_tr, &intr_RGB, uv_tr, (float)depth);

        QPointF tr = QPointF(xyz_tr[0], xyz_tr[1]);


        fov.setBottomLeft(bl);
        fov.setTopRight(tr);
        return fov;
}

