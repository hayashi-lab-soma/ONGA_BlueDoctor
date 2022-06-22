#include "vision.h"
#include <qdebug.h>

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

	if(data->isSpoit){
		Spoit(data);
	}

    //copy division parameters
    this->disMiddle = data->disM;
	this->topL = data->topL;
	this->bottomL = data->bottomL;

    white = 1;//reset internal variable

    //binarization
	hsvFilter(imgBinGreen, data->hsvRngsGreen);
    hsvFilter(imgBinBD, data->hsvRngsBD);

    //eroding(delating)--------------
	cv::Mat imgBinG = *imgBinGreen;
	cv::dilate(imgBinG, *imgBinGreen, cv::noArray(), cv::Point(-1,-1),5);
	cv::Mat imgBinB = *imgBinBD;
	cv::dilate(imgBinB, *imgBinBD, cv::noArray(), cv::Point(-1,-1),5);
	//-------------------------------

    detection(imgBinGreen, //detect Green light
              &data->posGreen_pix,
              &data->posGreen_m,
              &max_area_cmp_Green,
              &cmpGreen,
              lostGreen_num);

//    detection2(imgBinBD,&max_area_cmp_Green); //detect BD blue around Green

	//Results drawing on RGB image
	DrawResults(data);

    //count fps
//    if(calc_fps == true){
//        after = QTime::currentTime();
//        ms_dif = int(before.msecsTo(after));

//        before = after;
//        fps = (1.0/ms_dif)*1000;

//        QString _fps = QString::number(fps,'f',2);
//        cv::putText(*imgResult,
//                    "fps:"+_fps.toStdString(),
//                    cv::Point(5,30),
//                    cv::FONT_HERSHEY_SIMPLEX,
//                    0.8,
//                    cv::Scalar(0, 122, 255),
//                    2);
//    }else{
//        before = QTime::currentTime();
//        calc_fps = true;
//    }


	//signals
    emit updatedRGB(imgResult);//show img at main


    //Bin_BD fps to be half
    if(fpsDown) {
        emit updatedImgBD(imgBinBD);//show Bin Blue
        emit updatedImgGreen(imgBinGreen);//show Bin Green light
    }
    fpsDown = !fpsDown;

    data->beta = QTime::currentTime();
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

void Vision::getImg()
{
    cv::Mat originalImg = *imgResult;

    //remove white (cal average rgb in Img)
    int r = 0;
    int g = 0;
    int b = 0;
    for(int i=0;i<imgResult->cols;i++){
        for(int j=0;j<imgResult->rows;j++){
            cv::Vec3b rgb = originalImg.at<cv::Vec3b>(j,i);
            r = r + rgb[0];
            g = g + rgb[1];
            b = b + rgb[2];
        }
    }
    int avg_r = r/(imgResult->cols*imgResult->rows);
    int avg_g = g/(imgResult->cols*imgResult->rows);
    int avg_b = b/(imgResult->cols*imgResult->rows);

    //hosei
    cv::Point2f src_pt[] = {cv::Point2f(0,0), cv::Point2f(imgResult->cols,0),
                            cv::Point2f(0,imgResult->rows), cv::Point2f(imgResult->cols,imgResult->rows)};//original size
    cv::Point2f revision_pt[] = {cv::Point2f(0,0), cv::Point2f(topL,0),
                                 cv::Point2f(topL/2-bottomL/2,imgResult->rows), cv::Point2f(topL/2+bottomL/2,imgResult->rows)};//after hosei size
    const cv::Mat homography_matrix = cv::getPerspectiveTransform(src_pt,revision_pt);
    cv::Mat revisionImg = cv::Mat(cv::Size(topL,imgResult->rows),CV_8UC3);
    cv::warpPerspective(originalImg,
                        revisionImg,
                        homography_matrix,
                        revisionImg.size(),
//                        cv::INTER_LINEAR,CV_HAL_BORDER_CONSTANT,cv::Scalar(255,255,255));//hosei -> white
                        cv::INTER_LINEAR,CV_HAL_BORDER_CONSTANT,cv::Scalar(avg_r,avg_g,avg_b));//hosei -> average rgb

    emit sendImg(revisionImg);//show img at qcp
    //        qDebug() << " : " << ;
}

void Vision::hsvFilter(cv::Mat *output, HSV_Ranges_t range){
	cv::Mat tmp;
	cv::Mat inR_output;
    cv::cvtColor(rs2_frames.imgAlignedRGB, tmp, cv::COLOR_RGB2HSV); //rgb to hsv
	//Threashold by HSV range
	cv::Scalar lower = cv::Scalar(range.H.start, range.S.start, range.V.start);
	cv::Scalar upper = cv::Scalar(range.H.end, range.S.end, range.V.end);
	cv::inRange(tmp, lower, upper, inR_output);
    //cv::morphologyEx(inR_output, *output, cv::MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), 2);//removing noise(erosion followed by dilation) arg(cv::Mat()) = kernel = 3~3

    //erosion counts decrease, cross karnel
    cv::morphologyEx(inR_output, *output, cv::MORPH_CLOSE,//open
									 cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(3,3),cv::Point(-1,-1)),
									 cv::Point(-1, -1), 1);
}

void Vision::detection(cv::Mat *input,
                       QPoint *point,
                       QPointF *point_m,
                       Compornent_t *max,
                       Compornent_t *cmp,
                       int lost_num)
{
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

    //when no component (when cannot detect)
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
        white = 0;
        //avoid error(can not detect any components)
        goto NoWhite;
    }else{
        lost_num = 0;
    }

    //if number of compornent is two or more,
    //try to find maximum area compornent and the maximum compornent is set as the object
    int max_area = -INFINITY;
    int idx_max_area = -1;
    for(int i = 1; i < numComponents; i++){
        int *stat = stats.ptr<int>(i); //get stat of the (i)th component
        int A = stat[cv::ConnectedComponentsTypes::CC_STAT_AREA]; //get area [pixel]

        if(A > max_area){
            max_area = A;
            idx_max_area = i;
        }
    }
    TRACE(QString("Max area compornent : %1 (%2)").arg(idx_max_area).arg(max_area));
    //infoamation of component
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

NoWhite:
    if(white == 0){}

}

//To detect color2 around color1
void Vision::detection2(cv::Mat *input,Compornent_t *max)
{
    //To put Rectangle in FoV
    new_width= 30*(3/this->disMiddle);
    new_height= 30*(3/this->disMiddle);
    new_x = max->centroid.x()-(new_width/2);
    new_y = max->centroid.y()-(new_height/2);
//    int new_x = max->tl.x() - 5;
//    int new_y = max->tl.y() - 5;
//    int new_width = max->size.width() + 10;
//    int new_height = max->size.height() + 10;
    cv::Mat origin = *input;

    if(new_x < 0){//out of range
        new_x =0;
    }
    if(new_x + new_width > origin.cols){
        new_width = origin.cols - new_x;
    }
    if(new_y < 0){
        new_y = 0;
    }
    if(new_y+new_height > origin.rows){
        new_height = origin.rows - new_y;
    }

    //Get range around color1

    cv::Rect roi_pos(cv::Point(new_x,new_y),
                     cv::Size(new_width,new_height));

    cv::Mat narrow =origin(roi_pos);
    //Calculate ratio of white
//    int ALLwhite = cv::countNonZero(origin);
    int whitePixels = cv::countNonZero(narrow);//count white pixels in roi
    float white_ratio = (float(whitePixels)/(float(new_width) * float(new_height)))*100;
    //if ratio is over const, return true.
    if(white_ratio >=3.0){
        detect_inside = true;

    }else{
        detect_inside = false;
//        qDebug() << white_ratio << "%";
//        qDebug() << "rect_size: " << new_width << new_height;
    }
}

int Vision::Spoit(Data *data)
{
	return 0;
}

void Vision::cv_mouseCallBack(int event, int x, int y, int flags, void *userdata)
{
	return;
}

void Vision::DrawResults(Data *data)
{
    TRACE("draw results");

    //--------------------------------------------------
    //Draw green compornent's rectangle
    if(!data->isShowBDRect && white != 0){

        if(max_area_cmp_Green.centroid != QPoint(-1, -1)){	//if found Green
            cv::rectangle(*imgResult,//rectangle Green light
                          cv::Rect(max_area_cmp_Green.tl.x(),
                                   max_area_cmp_Green.tl.y(),
                                   max_area_cmp_Green.size.width(),
                                   max_area_cmp_Green.size.height()),
                          cv::Scalar(0, 255, 0),
                          2);
//        }

//        if(white != 0){// avoid no component error
//            if(detect_inside == true){// when blue (around green) can detect
//                //put point at centroid
//                cv::rectangle(*imgResult,//rectangle around light
//                              cv::Rect(new_x,
//                                       new_y,
//                                       new_width,
//                                       new_height),
//                              cv::Scalar(0, 0, 255),
//                              2);
            cv::circle(*imgResult, cv::Point(max_area_cmp_Green.centroid.x(),max_area_cmp_Green.centroid.y()), 4, cv::Scalar(255,0,0), 2, 4);

            //return Blue Doctor position (x[m], y[m])
            data->posBD_pix.setX(max_area_cmp_Green.centroid.x()); //[pixel]
            data->posBD_pix.setY(max_area_cmp_Green.centroid.y()); //[pixel]
            //convert to meter
            int u = max_area_cmp_Green.centroid.x();
            int v = max_area_cmp_Green.centroid.y();
            float point[3] = {0.0};

            //uv = (u,v):BD position [pixel]
            //_uv = (u/W, v/H):BD position ration (0.0 ~ 1.0)
            float width = rs2_frames.imgAlignedRGB.cols;
            float height = rs2_frames.imgAlignedRGB.rows;
            QPointF _uv = QPointF((float)u/width, (float)v/height);
            //_hw = (u/W*N, v/H*N):BD grid position (0 ~ N)
            QPointF _hw = QPointF(_uv.x()*(float)data->divN, _uv.y()*(float)data->divN);
            //depth grid index
            int idx = (int)(_hw.x()) + (int)(_hw.y())*data->divN;

            double depth = data->dis[idx]; //depth value from calibration data

            d455->deproject2(u,v,depth,point);//convert [pixel] to [m]

            //finally input meter
            data->posBD_m.setX(point[0]); //meter
            data->posBD_m.setY(point[1]*-1);
            data->wall_depth = depth;//csv z
            cmpBD = max_area_cmp;
//            }else{//only detect Green light
//                data->posBD_m.setX(BD_STATE_COLOR::VISION_DETECTION_ERROR); //meter
//                data->posBD_m.setY(BD_STATE_COLOR::VISION_DETECTION_ERROR);
//                data->posBD_pix.setX(10000); //[pixel]
//                data->posBD_pix.setY(10000); //[pixel]
//            }

        }else{ //no component (white = 0)
            data->posBD_m.setX(BD_STATE_COLOR::VISION_DETECTION_ERROR); //meter
            data->posBD_m.setY(BD_STATE_COLOR::VISION_DETECTION_ERROR);
            data->posBD_pix.setX(10000); //[pixel]
            data->posBD_pix.setY(10000); //[pixel]
            cmpBD = max_area_cmp_Green;
        }
    }

    //--------------------------------------------------
    //Draw grid lines
    if(data->isShowGrids){

        for(int i = 1; i < data->divN; i++){
            cv::line(*imgResult,//horizontal
                             cv::Point(imgResult->cols/data->divN*i, 0),
                             cv::Point(imgResult->cols/data->divN*i, imgResult->rows),
                             cv::Scalar(255, 0, 0),
                             2);
            cv::line(*imgResult,//vertical
                             cv::Point(0, imgResult->rows/data->divN*i),
                             cv::Point(imgResult->cols, imgResult->rows/data->divN*i),
                             cv::Scalar(255, 0, 0),
                             2);
        }
    }
    //--------------------------------------------------

    //--------------------------------------------------
    //After analyzed
    if(!data->BDs.isEmpty()
         && (!data->isAnalyzing && data->isAnalyzed)){

        if(data->isShowLevel_Y){//yellow
            for(int i = 0; i < data->BDs_Yellow.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Yellow[i].UV.x(), data->BDs_Yellow[i].UV.y()),5,cv::Scalar(255,255,0),-1,
                                     cv::LINE_AA);
            }
        }

        if(data->isShowLevel_R){//red
            for(int i = 0; i < data->BDs_Red.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Red[i].UV.x(), data->BDs_Red[i].UV.y()),5,cv::Scalar(255,0,0),-1,
                                     cv::LINE_AA);
            }
        }

        if(data->isShowAnalyzed){//yellow and red and orange
            for(int i = 0; i < data->BDs_Yellow.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Yellow[i].UV.x(), data->BDs_Yellow[i].UV.y()),5,cv::Scalar(255,255,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Orange.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Orange[i].UV.x(), data->BDs_Orange[i].UV.y()),5,cv::Scalar(255,165,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Red.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Red[i].UV.x(), data->BDs_Red[i].UV.y()),5,cv::Scalar(255,0,0),-1,cv::LINE_AA);
            }
        }

        if(data->isShowMeasured){//all (except predict)
            for(int i = 0; i < data->BDs_White.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_White[i].UV.x(), data->BDs_White[i].UV.y()),5,cv::Scalar(255,255,255),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Green.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Green[i].UV.x(), data->BDs_Green[i].UV.y()),5,cv::Scalar(0,128,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Yellow.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Yellow[i].UV.x(), data->BDs_Yellow[i].UV.y()),5,cv::Scalar(255,255,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Orange.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Orange[i].UV.x(), data->BDs_Orange[i].UV.y()),5,cv::Scalar(255,165,0),-1,cv::LINE_AA);
            }
            for(int i = 0; i < data->BDs_Red.count(); i++){
                cv::circle(*imgResult,cv::Point(data->BDs_Red[i].UV.x(), data->BDs_Red[i].UV.y()),5,cv::Scalar(255,0,0),-1,cv::LINE_AA);
            }
            //for(int i = 0; i < data->BDs_Predict.count(); i++){
            //	cv::Scalar *color = data->mapBDState2cvScalar[data->BDs_Predict[i].State];
            //	cv::circle(*imgResult,cv::Point(data->BDs_Predict[i].UV.x(), data->BDs_Predict[i].UV.y()),5,*color,-1,cv::LINE_AA);
            //

            //setAutoColorExposure();
            //saveimage_result();
        }
        if(data->isShowLevel_P){//predict
            for(int i = 0; i < data->BDs_Predict.count(); i++){
                cv::Scalar *color = data->mapBDState2cvScalar[data->BDs_Predict[i].State];
                cv::circle(*imgResult,cv::Point(data->BDs_Predict[i].UV.x(), data->BDs_Predict[i].UV.y()),5,*color,-1,cv::LINE_AA);
            }
        }
    }
    //--------------------------------------------------
    lastimg = imgResult->clone();
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
	QString file_path(savepath + "/image.jpg")
			TRACE(file_path);
	cv::Mat tmp = imgResult->clone();
	cv::cvtColor(tmp,tmp,cv::COLOR_BGR2RGB);
	cv::imwrite(file_path.toStdString(), tmp);
}

void Vision::saveimage_result(QString savepath_R){
	QString file_path(savepath_R + "/img_result.jpg");
			TRACE(savepath_R);
	//cv::imwrite((savepath_R + "/image_result.jpg").toStdString(), *imgResult);
	cv::cvtColor(lastimg,lastimg,cv::COLOR_BGR2RGB);
	cv::imwrite(file_path.toStdString(), lastimg);
}

void Vision::area_Clear(){
	cmpBD.area = -1.0;
	cmpBD.centroid = QPoint(-1,-1);
}

