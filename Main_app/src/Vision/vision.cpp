#include "vision.h"

#define IS_TRACE 0
#if IS_TRACE==1
#define TRACE(msg) qDebug()<<msg
#else
#define TRACE(msg)
#endif

#define SELECT_DEVICE 1 //0:R200,1:D4**

Vision::Vision(QObject *parent) : QObject(parent)
{
    cfg = new Config();
    isTest = cfg->getBool("OPTION", "TEST");

#if SELECT_DEVICE == 0
    r200 = new R200();
#elif SELECT_DEVICE == 1
    d455 = new D400s(isTest);
#endif

    max_area_cmp.area = -INFINITY;
    max_area_cmp.centroid = QPoint(0,0);
    max_area_cmp.size = QSize(0,0);
    max_area_cmp.tl = QPoint(0,0);

    cmpBD.area = 0.0;
    cmpBD.centroid = QPoint(0, 0);
    cmpBD.size = QSize(0, 0);
    cmpBD.tl = QPoint(0, 0);

    max_area_cmp_BD.centroid = QPoint(0,0);
    max_area_cmp_BD.size = QSize(0,0);
    max_area_cmp_BD.tl = QPoint(0,0);
    max_area_cmp_BD.area = -INFINITY;

    max_area_cmp_Green.centroid = QPoint(0,0);
    max_area_cmp_Green.size = QSize(0,0);
    max_area_cmp_Green.tl = QPoint(0,0);
}

int Vision::initialize()
{
#if SELECT_DEVICE == 0
    r200->initialize(!isTest);
    r200->concreate(r200Data);

    imgResult = new cv::Mat(r200Data.imgRGB->rows,
                            r200Data.imgRGB->cols,
                            r200Data.imgRGB->type(),
                            cv::Scalar::all(0));

    imgBinBD = new cv::Mat(r200Data.imgRGB->rows,
                           r200Data.imgRGB->cols,
                           CV_8UC1,
                           cv::Scalar::all(0));
    r200->startDevice();

#elif SELECT_DEVICE == 1
    if(d455->init() == -1) return -1;
    imgResult = new cv::Mat();
    imgBinBD = new cv::Mat();
    imgBinGreen = new cv::Mat();

#endif

    QThread::msleep(200);
    return 0;
}

#if SELECT_DEVICE == 0
void Vision::main(Data *data)
{
    //    //get Field-of-View of XY plane at wall's depth
    //    data->fov = r200->getFOV(data->wall_depth);
    //    //    //get grid size in pixel coordinate at wall's depth
    //    data->grid_b_pix = r200->getPixelWidth(data->grid_b_m, data->wall_depth);

    TRACE("--- Get current frame ---");

    //    r200->waitFrame();
    //    r200->getImage(R200_IMG_TYPES::Color, r200Data.imgRGB);
    //    r200Data.imgRGB->copyTo(*imgResult);

    //    //Spoiting
    //    //    if(data->isSpoit){
    //    //		Spoit(data);
    //    //    }

    //    //get blue doctor position ([m], [m]) centroid of labeling result
    //    DetectionBlueDoctor(r200Data.imgRGB,			//input image (Color)
    //                        data->hsvRngsGreen,		//input HSV range for Blue doctor LED color
    //                        data->wall_depth,			//input depth to wall [m]
    //                        data->posBD_m,				//ouput Blue doctor position (meter, 2D)
    //                        data->posBD_pix);			//output Blue doctor position (pix, 2D)


    //    //Results drawing on RGB image
    //    DrawResults(data);


    //signals
    emit updatedRGB(&rs2_frames.imgAlignedRGB);
    //    emit updatedRGB(imgResult);
    //    //    emit updatedRGB(r200Data.imgRGB);
    //    emit updatedImgMask(imgBinBD);

    //    //    //camera parameter change request??
    //    //    qInfo() << "4";
    //    if(data->isCamParamChange){
    //        r200->setColorExposure(data->camParams.color_exposure);
    //        r200->setIrExposure(data->camParams.ir_exposure);
    //        data->isCamParamChange = false; //flag off
    //    }
}
#elif SELECT_DEVICE == 1
void Vision::main(Data *data)
{
    TRACE("do main");
    //get current frame
    d455->getFrames(rs2_frames);
    //copy RGB image -> imgResult
    *imgResult = rs2_frames.imgAlignedRGB.clone();
    //copy division parameters
    this->disT = data->disT;
    this->disM = data->disM;
    this->disB = data->disB;

    hsvFilter(imgBinGreen, data->hsvRngsGreen);
    hsvFilter(imgBinBD, data->hsvRngsBD);

    detection(imgBinGreen,
              &data->posGreen_pix,
              &data->posGreen_m,
              &max_area_cmp_Green,
              &cmpGreen,
              lostGreen_num);
    detection(imgBinBD,
              &data->posBD_pix,
              &data->posBD_m,
              &max_area_cmp_BD,
              &cmpBD,
              lostBD_num);


//NoWhite:

    //Results drawing on RGB image
    DrawResults(data);

    //signals
    emit updatedRGB(imgResult);
    emit updatedImgGreen(imgBinGreen);
    emit updatedImgBD(imgBinBD);

    return;
}
#endif

void Vision::finalize()
{
}

void Vision::setCameraParam(Camera_Params_t camParams)
{
    r200->setColorExposure(camParams.color_exposure);
    r200->setIrExposure(camParams.ir_exposure);
}

void Vision::setAutoColorExposure()
{
    r200->setAutoColorExposure();
}

void Vision::hsvFilter(cv::Mat *output, HSV_Ranges_t range){
    cv::Mat tmp;
    cv::Mat inR_output;
    cv::cvtColor(rs2_frames.imgAlignedRGB, tmp, cv::COLOR_RGB2HSV);
    //Threashold by HSV range
    cv::Scalar lower = cv::Scalar(range.H.start, range.S.start, range.V.start);
    cv::Scalar upper = cv::Scalar(range.H.end, range.S.end, range.V.end);
    cv::inRange(tmp, lower, upper, inR_output);
    cv::morphologyEx(inR_output, *output, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 2); //removing noise(erosion followed by dilation)
}

void Vision::detection(cv::Mat *input,
                       QPoint *point,
                       QPointF *point_m,
                       Compornent_t *max,
                       Compornent_t *cmp,
                       int lost_num) {
    //labeling process on binary image
    cv::Mat matLabels = cv::Mat();
    cv::Mat stats = cv::Mat();
    cv::Mat centroids = cv::Mat();
    int numComponents = cv::connectedComponentsWithStats(*input,
                                                         matLabels,
                                                         stats,
                                                         centroids,
                                                         8,
                                                         CV_32S);
    TRACE(QString("Num labels : %1").arg(numComponents));

    //if number of compornents is one,
    //it means that the component is background of the image.
    //So, you should something to avoid error of the detection process
    if(numComponents == 1){
        max->area = -1.0;
        max->centroid = QPoint(-1, -1);
        //		cmp.area = -1.0;
        //		cmp.centroid = QPoint(-1, -1);

        lost_num++;

        if(lost_num == 15){
            emit lostError();
            lost_num = 0;
        }
    }else{
        lost_num = 0;
    }


    //if number of compornent is two or more,
    //try to find maximum area compornent and the maximum compornent is set as the object
    int max_area = -INFINITY;
    int idx_max_area = -1;
    for(int i = 1; i < numComponents; i++){
        int *stat = stats.ptr<int>(i); //get stat of the (i)th compornent
        int A = stat[cv::ConnectedComponentsTypes::CC_STAT_AREA]; //get area [pixel]

        if(A > max_area){
            max_area = A;
            idx_max_area = i;
        }
    }
    TRACE(QString("Max area compornent : %1 (%2)").arg(idx_max_area).arg(max_area));

    int *stat = stats.ptr<int>(idx_max_area);
    int A = stat[cv::ConnectedComponentsTypes::CC_STAT_AREA];
    max->area = A;
    int t = stat[cv::ConnectedComponentsTypes::CC_STAT_LEFT]; //top coordinate
    int l = stat[cv::ConnectedComponentsTypes::CC_STAT_TOP];  //left coordinate
    max->tl = QPoint(t, l);
    int w = stat[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
    int h = stat[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
    max->size = QSize(w, h);
    int cx = (int)((centroids.ptr<double>(idx_max_area))[0]);
    int cy = (int)((centroids.ptr<double>(idx_max_area))[1]);
    max->centroid = QPoint(cx, cy);


    //return Blue Doctor or Green position (x[m], y[m])
    if(max->area >= cmp->area){
        //Store result
        point->setX(max->centroid.x()); //[pixel]
        point->setY(max->centroid.y()); //[pixel]
        //convert to meter
        int u = max->centroid.x();
        int v = max->centroid.y();
        float point[3] = {0.0};
        d455->deproject(u,v,point);
        //finally input meter
        point_m->setX(point[0]); //meter
        //data->posBD_m.setY(point[1]); //meter
        point_m->setY(point[1]*-1);
        cmp = max;
    }
}

int Vision::Spoit(Data *data)
{
    if(!isWndSpoit){
        cv::namedWindow("Spoit");
        cv::namedWindow("Binaly");
        //mouse click call back
        isWndSpoit = true;
        return 0;
    }

    cv::Mat _rgb = r200Data.imgRGB->clone();
    cv::Mat rgb;
    cv::resize(_rgb, rgb, cv::Size(), 0.5, 0.5);

    cv::Mat _bin = imgBinBD->clone();
    cv::Mat bin;
    cv::resize(_bin, bin, cv::Size(), 0.5, 0.5);

    cv::imshow("Spoit", rgb);
    cv::imshow("Binaly", bin);
    cv::waitKey(1);

    return 0;
}

void Vision::cv_mouseCallBack(int event, int x, int y, int flags, void *userdata)
{
    return;
}

/*!
 * \brief Vision::DrawResults
 * \param wall_depth
 * \param isShowBDRect
 * \param isShowGrids
 */
void Vision::DrawResults(Data *data)
{
    TRACE("draw results");

    //--------------------------------------------------
    //Draw green compornent's rectangle
    if(data->isShowBDRect){
        if(max_area_cmp_BD.centroid != QPoint(-1, -1)){	//if found BD
            cv::rectangle(*imgResult,
                          cv::Rect(max_area_cmp_BD.tl.x(),
                                   max_area_cmp_BD.tl.y(),
                                   max_area_cmp_BD.size.width(),
                                   max_area_cmp_BD.size.height()),
                          cv::Scalar(0, 255, 0),
                          5);
        }
        if(max_area_cmp_Green.centroid != QPoint(-1, -1)){	//if found Green
            cv::rectangle(*imgResult,
                          cv::Rect(max_area_cmp_Green.tl.x(),
                                   max_area_cmp_Green.tl.y(),
                                   max_area_cmp_Green.size.width(),
                                   max_area_cmp_Green.size.height()),
                          cv::Scalar(255, 0, 0),
                          5);
        }

        cv::RotatedRect rectBD(cv::Point(max_area_cmp_BD.centroid.x(),max_area_cmp_BD.centroid.y()),
                               cv::Size(max_area_cmp_BD.size.width(),max_area_cmp_BD.size.height()), 0);
        cv::RotatedRect rectlight(cv::Point(max_area_cmp_Green.centroid.x(),max_area_cmp_Green.centroid.y()),
                                  cv::Size(max_area_cmp_Green.size.width(),max_area_cmp_Green.size.height()),0);

        std::vector<cv::Point2f> vertices;
        int status = cv::rotatedRectangleIntersection(rectBD,rectlight,vertices);
        if(status>0){
            std::cout << "x" << max_area_cmp_BD.centroid.x() << std::endl;
            DrawPointSet(*imgResult, std::vector<cv::Point>(vertices.begin(), vertices.end()), 5, cv::Scalar(0, 0, 255),-1,cv::LINE_AA);
            cv::circle(*imgResult, cv::Point(max_area_cmp_BD.centroid.x(),max_area_cmp_BD.centroid.y()), 4, cv::Scalar(255,0,0), 2, 4);

        }
    }

    //--------------------------------------------------
    //Draw grid lines
    if(data->isShowGrids){

        for(int i = 1; i < data->divN; i++){
            //            cv::line(*imgResult,//vertical middle
            //                     cv::Point(0, imgResult->rows/2),
            //                     cv::Point(imgResult->cols, imgResult->rows/2),
            //                     cv::Scalar(255, 0, 0),
            //                     5);

            //            cv::line(*imgResult,//horizontal middle
            //                     cv::Point(imgResult->cols/2, 0),
            //                     cv::Point(imgResult->cols/2, imgResult->rows),
            //                     cv::Scalar(255, 0, 0),
            //                     5);

            cv::line(*imgResult,//horizontal
                     cv::Point(imgResult->cols/data->divN*i, 0),
                     cv::Point(imgResult->cols/data->divN*i, imgResult->rows),
                     cv::Scalar(255, 0, 0),
                     5);
            cv::line(*imgResult,//vertical
                     cv::Point(0, imgResult->rows/data->divN*i),
                     cv::Point(imgResult->cols, imgResult->rows/data->divN*i),
                     cv::Scalar(255, 0, 0),
                     5);

        }


        //        for(int u = imgResult->cols/2; u < imgResult->cols; u += data->grid_b_pix){
        //            cv::line(*imgResult,
        //                     cv::Point(u, 0),
        //                     cv::Point(u, imgResult->rows),
        //                     cv::Scalar(255, 0, 0),
        //                     2);
        //        }

        //        for(int u = imgResult->cols/2; u >= 0; u -= data->grid_b_pix){
        //            cv::line(*imgResult,
        //                     cv::Point(u, 0),
        //                     cv::Point(u, imgResult->rows),
        //                     cv::Scalar(255, 0, 0),
        //                     2);
        //        }

        //        for(int v = imgResult->rows/2; v < imgResult->rows; v += data->grid_b_pix){
        //            cv::line(*imgResult,
        //                     cv::Point(0, v),
        //                     cv::Point(imgResult->cols, v),
        //                     cv::Scalar(255, 0, 0),
        //                     2);
        //        }

        //        for(int v = imgResult->rows/2; v >= 0; v -= data->grid_b_pix){
        //            cv::line(*imgResult,
        //                     cv::Point(0, v),
        //                     cv::Point(imgResult->cols, v),
        //                     cv::Scalar(255, 0, 0),
        //                     2);
        //        }
    }
    //--------------------------------------------------

    //--------------------------------------------------
    //After analyzed
    if(!data->BDs.isEmpty()
            && (!data->isAnalyzing && data->isAnalyzed)){

        if(data->isShowLevel_Y){
            for(int i = 0; i < data->BDs_Yellow.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Yellow[i].UV.x(), data->BDs_Yellow[i].UV.y()),15,cv::Scalar(255,255,0),-1,
                           cv::LINE_AA);
            }
        }

        if(data->isShowLevel_R){
            for(int i = 0; i < data->BDs_Red.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Red[i].UV.x(), data->BDs_Red[i].UV.y()),15,cv::Scalar(255,0,0),-1,
                           cv::LINE_AA);
            }
        }

        //��͌��ʂ̗��U���f�[�^���摜��ɕ`��
        if(data->isShowAnalyzed){
            for(int i = 0; i < data->BDs_Yellow.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Yellow[i].UV.x(), data->BDs_Yellow[i].UV.y()),15,cv::Scalar(255,255,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Orange.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Orange[i].UV.x(), data->BDs_Orange[i].UV.y()),15,cv::Scalar(255,165,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Red.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Red[i].UV.x(), data->BDs_Red[i].UV.y()),15,cv::Scalar(255,0,0),-1,cv::LINE_AA);
            }
        }

        //�v�����ʂ̐��f�[�^���摜��ɕ`��
        if(data->isShowMeasured){
            for(int i = 0; i < data->BDs_White.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_White[i].UV.x(), data->BDs_White[i].UV.y()),15,cv::Scalar(255,255,255),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Green.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Green[i].UV.x(), data->BDs_Green[i].UV.y()),15,cv::Scalar(0,128,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Yellow.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Yellow[i].UV.x(), data->BDs_Yellow[i].UV.y()),15,cv::Scalar(255,255,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Orange.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Orange[i].UV.x(), data->BDs_Orange[i].UV.y()),15,cv::Scalar(255,165,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Red.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Red[i].UV.x(), data->BDs_Red[i].UV.y()),15,cv::Scalar(255,0,0),-1,cv::LINE_AA);
            }

            setAutoColorExposure();
            //saveimage_result();
        }
    }
    //--------------------------------------------------

    return;
}

void Vision::DrawPointSet(cv::Mat& imgInoutput, std::vector<cv::Point> vecPoint, int radius, const cv::Scalar& color, int thickness, int lineType)
{
    for (size_t i = 0; i < vecPoint.size(); i++)
        cv::circle(imgInoutput, vecPoint[i], radius, color, thickness, lineType);
    *imgResult = imgInoutput.clone();
}

void Vision::saveimage(QString savepath){
    TRACE("call save image slot");
    TRACE(savepath);
    QString file_path(savepath + "/image_" + disT +"_"+ disM +"_"+ disB +".jpg");
    TRACE(file_path);
    cv::Mat tmp = imgResult->clone();
    cv::cvtColor(tmp,tmp,cv::COLOR_BGR2RGB);
    cv::imwrite(file_path.toStdString(), tmp);
}

void Vision::saveimage_result(){
    //    QString file_path(savepath + "/image_" + disT +"_"+ disM +"_"+ disB +".jpg");
    cv::imwrite((savepath_R + "/image_result" + disT +"_"+ disM +"_"+ disB +".jpg").toStdString(), *imgResult);
    cv::Mat tmp = imgResult->clone();
    cv::cvtColor(tmp,tmp,cv::COLOR_BGR2RGB);
    //    cv::imwrite(file_path.toStdString(), tmp);
}

void Vision::area_Clear(){
    cmpBD.area = -1.0;
    cmpBD.centroid = QPoint(-1,-1);
}
