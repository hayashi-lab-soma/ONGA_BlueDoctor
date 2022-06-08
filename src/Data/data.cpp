#include "data.h"


Data::Data()
{

    cfg = new Config();
    topLogsDirName = QString("");
    fps = 0.0;

    fov = QRectF();
    camParams.color_exposure = cfg->getInteger("VISION", "COLOR_EXPOSURE");
    camParams.ir_exposure = cfg->getInteger("VISION", "IR_EXPOSURE");

    hsvRngsGreen.H.start = cfg->getInteger("VISION", "H_MIN_GREEN");
    hsvRngsGreen.H.end = cfg->getInteger("VISION", "H_MAX_GREEN");
    hsvRngsGreen.S.start = cfg->getInteger("VISION", "S_MIN_GREEN");
    hsvRngsGreen.S.end = cfg->getInteger("VISION", "S_MAX_GREEN");
    hsvRngsGreen.V.start = cfg->getInteger("VISION", "V_MIN_GREEN");
    hsvRngsGreen.V.end = cfg->getInteger("VISION", "V_MAX_GREEN");

    hsvRngsBD.H.start = cfg->getInteger("VISION", "H_MIN_GREEN");
    hsvRngsBD.H.end = cfg->getInteger("VISION", "H_MAX_GREEN");
    hsvRngsBD.S.start = cfg->getInteger("VISION", "S_MIN_GREEN");
    hsvRngsBD.S.end = cfg->getInteger("VISION", "S_MAX_GREEN");
    hsvRngsBD.V.start = cfg->getInteger("VISION", "V_MIN_GREEN");
    hsvRngsBD.V.end = cfg->getInteger("VISION", "V_MAX_GREEN");

    posBD_m = QPointF(0.0, 0.0);
    posBD_pix = QPoint(0, 0);

    wall_depth = cfg->getDouble("MAIN", "WALL_DEPTH");
    //    wall_depth = 3.0;
    grid_b_m = 0.1;	//[m]
    grid_b_pix = 0;

    BD.State = BD_STATE_COLOR::White;
    BDs.clear();

    mapBDState2cvScalar[BD_STATE_COLOR::White] = new cv::Scalar(BD_STATE_COLOR::CV_SCALAR::White);
    mapBDState2cvScalar[BD_STATE_COLOR::Green] = new cv::Scalar(BD_STATE_COLOR::CV_SCALAR::Green);
    mapBDState2cvScalar[BD_STATE_COLOR::Yellow] = new cv::Scalar(BD_STATE_COLOR::CV_SCALAR::Yellow);
    mapBDState2cvScalar[BD_STATE_COLOR::Orange] = new cv::Scalar(BD_STATE_COLOR::CV_SCALAR::Orange);
    mapBDState2cvScalar[BD_STATE_COLOR::Red] = new cv::Scalar(BD_STATE_COLOR::CV_SCALAR::Red);
    mapBDState2cvScalar[BD_STATE_COLOR::Predict] = new cv::Scalar(BD_STATE_COLOR::CV_SCALAR::Predict);


    state = State::Stanby;

    isCamParamChange = false;

    isSpoit = false;

    isMeasure = false;
    isRefresh = false;

    isAnalyzing = false;
    isAnalyzed = false;

    isShowBDRect = false;
    isShowGrids = false;

		isShowLevel_Y = isShowLevel_R = isShowLevel_P = isShowAnalyzed = isShowMeasured = false;


    //Make logs directory
    QDir dir;
    dir.mkdir("./../../../../../Result/BD_Log");

    QDateTime today = QDateTime::currentDateTime();
    QString dirName = QString("./../../../../../Result/BD_Log/");
    dirName += today.toString("yyyy_MM_dd");
    dir.mkdir(dirName);
    topLogsDirName = dirName + QString("/");

}

void Data::makeMeasurementLogDir()
{
    measurementLogDirName = topLogsDirName;

    QDateTime ct = QDateTime::currentDateTime();
    measurementLogDirName += ct.toString("/hh_mm_ss");

    QDir dir;
    dir.mkdir(measurementLogDirName);

}

void Data::restoreConfig()
{
    //Restore Camera Parameters
    cfg->setInteger("VISION", "COLOR_EXPOSURE", camParams.color_exposure);
    cfg->setInteger("VISION", "IR_EXPOSURE", camParams.ir_exposure);

    //Restore HSV Ranges
    //Green
    cfg->setInteger("VISION", "H_MIN_GREEN", hsvRngsGreen.H.start);
    cfg->setInteger("VISION", "H_MAX_GREEN", hsvRngsGreen.H.end);
    cfg->setInteger("VISION", "S_MIN_GREEN", hsvRngsGreen.S.start);
    cfg->setInteger("VISION", "S_MAX_GREEN", hsvRngsGreen.S.end);
    cfg->setInteger("VISION", "V_MIN_GREEN", hsvRngsGreen.V.start);
    cfg->setInteger("VISION", "V_MAX_GREEN", hsvRngsGreen.V.end);

    cfg->setInteger("VISION", "H_MIN_BLUE", hsvRngsBD.H.start);
    cfg->setInteger("VISION", "H_MAX_BLUE", hsvRngsBD.H.end);
    cfg->setInteger("VISION", "S_MIN_BLUE", hsvRngsBD.S.start);
    cfg->setInteger("VISION", "S_MAX_BLUE", hsvRngsBD.S.end);
    cfg->setInteger("VISION", "V_MIN_BLUE", hsvRngsBD.V.start);
    cfg->setInteger("VISION", "V_MAX_BLUE", hsvRngsBD.V.end);
}
