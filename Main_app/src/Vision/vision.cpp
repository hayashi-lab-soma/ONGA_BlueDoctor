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
    d455 = new D400s();
#endif

    max_area_cmp.area = -INFINITY;
    max_area_cmp.centroid = QPoint(0,0);
    max_area_cmp.size = QSize(0,0);
    max_area_cmp.tl = QPoint(0,0);

    cmpBD.area = 0.0;
    cmpBD.centroid = QPoint(0, 0);
    cmpBD.size = QSize(0, 0);
    cmpBD.tl = QPoint(0, 0);

    isTracking = false;
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

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat hsv, mask;
    cv::Scalar lower = cv::Scalar(49, 100, 33);
    cv::Scalar upper = cv::Scalar(147, 255, 255);

    cv::cvtColor(rs2_frames.imgAlignedRGB, hsv, CV_RGB2HSV, 3);
    cv::inRange(hsv, lower, upper, mask);
    cv::imshow("hsv", hsv);
    cv::imshow("mask", mask);

    cv::findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    int largest_area = 0;
    int largest_contours_index = 0;
    cv::Rect bounding_rect;
    for(size_t i=0; i<contours.size(); i++){
        double area = cv::contourArea(contours[i]);
        if (area > largest_area) {
            largest_area = area;
            largest_contours_index = i;
            bounding_rect = cv::boundingRect(contours[i]);
        }
    }
    cv::drawContours(rs2_frames.imgAlignedRGB, contours, largest_contours_index, cv::Scalar( 0, 255, 0 ),2);
    cv::imshow("result", rs2_frames.imgAlignedRGB);

    //copy RGB image -> imgResult
    *imgResult = rs2_frames.imgAlignedRGB.clone();

	/*
	//------------------------------------------------------------------------------------------------------------------------
	//Detection of Blue Doctor's position and rectangle
	cv::Mat imgHSV;
	cv::cvtColor(rs2_frames.imgAlignedRGB, imgHSV, cv::COLOR_RGB2HSV);
	//Threashold by HSV range
	cv::Scalar lower = cv::Scalar(data->hsvRngsGreen.H.start, data->hsvRngsGreen.S.start, data->hsvRngsGreen.V.start);
	cv::Scalar upper = cv::Scalar(data->hsvRngsGreen.H.end, data->hsvRngsGreen.S.end, data->hsvRngsGreen.V.end);
	cv::inRange(imgHSV, lower, upper, *imgBinBD);
	//Additional image processing
	cv::medianBlur(*imgBinBD,*imgBinBD, 3);
	cv::morphologyEx(*imgBinBD, *imgBinBD, cv::MORPH_CLOSE, 3);

	//labeling process on binary image
	cv::Mat matLabels = cv::Mat();
	cv::Mat stats = cv::Mat();
	cv::Mat centroids = cv::Mat();
	int numComponents = cv::connectedComponentsWithStats(*imgBinBD,
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
		max_area_cmp.area = -1.0;
		max_area_cmp.centroid = QPoint(-1, -1);
		//		cmpBD.area = -1.0;
		//		cmpBD.centroid = QPoint(-1, -1);

		lostBD_num++;

		if(lostBD_num == 15){
			emit lostError();
			lostBD_num = 0;
		}
		goto NoWhite;
	}else{
		lostBD_num = 0;
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
	max_area_cmp.area = A;
	int t = stat[cv::ConnectedComponentsTypes::CC_STAT_LEFT]; //top coordinate
	int l = stat[cv::ConnectedComponentsTypes::CC_STAT_TOP];  //left coordinate
	max_area_cmp.tl = QPoint(t, l);
	int w = stat[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
	int h = stat[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
	max_area_cmp.size = QSize(w, h);
	int cx = (int)((centroids.ptr<double>(idx_max_area))[0]);
	int cy = (int)((centroids.ptr<double>(idx_max_area))[1]);
	max_area_cmp.centroid = QPoint(cx, cy);


	//return Blue Doctor position (x[m], y[m])
	if(max_area_cmp.area >= cmpBD.area){
		//Store result
		//        pos_pix = max_area_cmp.centroid;	//[pix]
		//[meter]
		//        pos_m = r200->deprojectRGB(pos_pix, wall_depth);
		//        pos_m.setY(-pos_m.y());
		cmpBD = max_area_cmp;
	}
	*/

	//------------------------------------------------------------------------------------------------------------------------

NoWhite:

	//Results drawing on RGB image
	DrawResults(data);

	//signals
	emit updatedRGB(imgResult);
	emit updatedImgMask(imgBinBD);
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

/*!
 * \brief Vision::DetectionGreenLED
 * \return
 */
int Vision::DetectionBlueDoctor(cv::Mat *img,
																HSV_Ranges_t hsvRngs,
																double wall_depth,
																QPointF &pos_m,
																QPoint &pos_pix)
{
	//Image convert to HSV
	cv::Mat imgHSV;
	cv::cvtColor(*img, imgHSV, cv::COLOR_RGB2HSV);
	//	cv::cvtColor(*img, imgHSV, CV_RGB2YUV);

	//make mask image
	cv::Scalar lower = cv::Scalar(hsvRngs.H.start, hsvRngs.S.start, hsvRngs.V.start);
	cv::Scalar upper = cv::Scalar(hsvRngs.H.end, hsvRngs.S.end, hsvRngs.V.end);
	cv::inRange(imgHSV, lower, upper, *imgBinBD);

	//Additional processing
	//	cv::medianBlur(*imgBinBD, *imgBinBD, 5);
	//	cv::morphologyEx(*imgBinBD, *imgBinBD, CV_MOP_DILATE, cv::Mat(), cv::Point(-1, -1), 3);

	//labeling (8-neighber)
	cv::Mat matLabels = cv::Mat();
	cv::Mat stats = cv::Mat();
	cv::Mat centroids = cv::Mat();

	int numLabels = cv::connectedComponentsWithStats(*imgBinBD,
																									 matLabels,
																									 stats,
																									 centroids,
																									 8,
																									 CV_32S);
	//Cannot found Green LED
	//If (numLabels == 1) then raw image size will returned

	if(numLabels == 1){
		/*
				pos_pix = QPoint(); //pixel coordinate
				pos_m = r200->deprojectRGB(pos_pix, wall_depth); //to Meter unit
				pos_m.setY(-pos_m.y());
				*/

		//		pos_pix = QPoint(); //pixel coordinate
		//		pos_m = r200->deprojectRGB(pos_pix, wall_depth); //to Meter unit
		//pos_m.setX(0.0);
		//pos_m.setY(0.0);

		//pos_pix.setX(10000);
		//pos_pix.setY(10000);
		//pos_m.setX(10000.0);
		//pos_m.setY(10000.0);

		//reset compornent
		max_area_cmp.area = -1.0;
		max_area_cmp.centroid = QPoint(-1, -1);
		//		cmpBD.area = -1.0;
		//		cmpBD.centroid = QPoint(-1, -1);

		lostBD_num++;
		//qDebug() << "lost_num" << lostBD_num;

		if(lostBD_num == 15){
			emit lostError();
			lostBD_num = 0;
		}
		//Flag off
		return -1;
	}else{
		lostBD_num = 0;
	}

	//find maximum area compornent
	max_area_cmp.area = -INFINITY;

	for(int i = 1; i < numLabels; i++){
		int *stat = stats.ptr<int>(i);
		int area = stat[cv::ConnectedComponentsTypes::CC_STAT_AREA];

		if((double)area > max_area_cmp.area){

			max_area_cmp.area = area;

			int t = stat[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int l = stat[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			max_area_cmp.tl = QPoint(t, l);

			int w = stat[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];
			int h = stat[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			max_area_cmp.size = QSize(w, h);

			int cx = (int)((centroids.ptr<double>(i))[0]);
			int cy = (int)((centroids.ptr<double>(i))[1]);
			max_area_cmp.centroid = QPoint(cx, cy);
			//return ireru
			qDebug() << "max_area_cmp.area" << max_area_cmp.area;
		}
	}


	//return Blue Doctor position (x[m], y[m])
	//tominaga
	if(max_area_cmp.area >= cmpBD.area){
		//Store result
		pos_pix = max_area_cmp.centroid;	//[pix]
		//[meter]
		pos_m = r200->deprojectRGB(pos_pix, wall_depth);
		pos_m.setY(-pos_m.y());
		cmpBD = max_area_cmp;
	}

	return 0;
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
		if(max_area_cmp.centroid != QPoint(-1, -1)){	//if found BD
			cv::rectangle(*imgResult,
										cv::Rect(max_area_cmp.tl.x(),
														 max_area_cmp.tl.y(),
														 max_area_cmp.size.width(),
														 max_area_cmp.size.height()),
										cv::Scalar(0, 255, 0),
										5);
		}
	}
	//--------------------------------------------------

	//--------------------------------------------------
	//Draw grid lines
	if(data->isShowGrids){

		cv::line(*imgResult,//vertical middle
						 cv::Point(0, imgResult->rows/2),
						 cv::Point(imgResult->cols, imgResult->rows/2),
						 cv::Scalar(255, 0, 0),
						 5);

		cv::line(*imgResult,//horizontal middle
						 cv::Point(imgResult->cols/2, 0),
						 cv::Point(imgResult->cols/2, imgResult->rows),
						 cv::Scalar(255, 0, 0),
						 5);

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

		//解析結果の離散化データを画像上に描画
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

		//計測結果の生データを画像上に描画
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

void Vision::saveimage(QString savepath){
	savepath_R = savepath;
	*imgResult = r200Data.imgRGB->clone();
	cv::cvtColor(*imgResult, *imgResult, cv::COLOR_RGB2BGR);
	//    cv::imwrite((savepath + "/image" + ".jpg").toStdString(), *imgResult);
	cv::imwrite((savepath + "/image" + data->disT +"_"+ data->disM +"_"+ data->disB +".jpg").toStdString(), *imgResult);
}

void Vision::saveimage_result(){
	cv::cvtColor(*imgResult, *imgResult, cv::COLOR_BGR2RGB);
	//    cv::imwrite((savepath_R + "/image_result" + ".jpg").toStdString(), *imgResult);
	cv::imwrite((savepath_R + "/image_result" + data->disT +"_"+ data->disM +"_"+ data->disB +".jpg").toStdString(), *imgResult);
	cv::cvtColor(*imgResult, *imgResult, cv::COLOR_RGB2BGR);
}

void Vision::area_Clear(){
	cmpBD.area = -1.0;
	cmpBD.centroid = QPoint(-1,-1);
}
