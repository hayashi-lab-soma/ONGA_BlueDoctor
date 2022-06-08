#include "r200.h"

using namespace cv;

rs::context *R200::rsCntxt = nullptr;

/*!
 * \brief R200::R200
 * \param parent
 */
R200::R200(QObject *parent) : QObject(parent)
{
    isUse = false;

    if(rsCntxt == nullptr) rsCntxt = new rs::context();
    rsDev = nullptr;
}

/*!
 * \brief R200::initialize
 * R200デバイスの初期化処理
 * 引数
 * QString qsSerial : 接続先R200をシリアル番号で指定できる
 * 戻り値
 *	int : -1:失敗, 0:成功
 */
int R200::initialize(bool isUse)
{
    this->isUse = isUse;

    //librealsenseログファイル作成(ログレベルwarn以上) ログファイル自体がstaticなものなので, 複数のrealsenseデバイスから同一ファイルに書き込まれる
    rs::log_to_file(rs::log_severity::info, "librealsense.log");

    //デバイス探索，オープン処理, rs::deviceの実態作成
    if(initDevice() == -1) return -1;

    //ストリーム有効化
    if(initStreams() == -1) return -1;

    //Intstrincs, Extstrincsの初期化
    if(initIntrinsicsAndExtrinsics() == -1) return -1;

    return 0;
}

void R200::concreate(R200Data_t &data)
{
    data.imgRGB = new cv::Mat(sizeImgRGB.height(), sizeImgRGB.width(), CV_8UC3, cv::Scalar::all(0));
    data.imgDepth = new cv::Mat(sizeImgDepth.height(), sizeImgDepth.width(), CV_8UC1, cv::Scalar::all(0));
    data.imgInfrared = new cv::Mat(sizeImgInfrared.height(), sizeImgInfrared.width(), CV_16UC1, cv::Scalar(0));

    //    data.pntCldPtr.reset(new PointCloudT);
    //    data.pntCldPtr->clear();
}

/*!
 * \brief R200::wait4Frame
 * \return
 */
int R200::waitFrame()
{
    if(!isUse) return 0;

    //デバイスのフレーム更新を待つ
    try{
        if(rsDev->is_streaming())	rsDev->wait_for_frames();
        else	return -1;
    }
    catch(const rs::error e){
        qWarning() << e.get_failed_args().c_str();
        qWarning() << e.get_failed_function().c_str();
        qWarning() << e.what();
        return -1;
    }
    return 0;
}

/*!
 * \brief R200::getAllData
 * \param r200Data
 * \return
 */
int R200::getData(R200Data_t &r200Data)
{
    //R200のフレーム更新
    if(waitFrame() == -1) return -1;
    //各ストリームから画像の取得
    if(getImage("RGB", (r200Data.imgRGB)) == -1)			return -1;
    if(getImage("Depth", (r200Data.imgDepth)) == -1)	return -1;

#if USE_INFRARED == 1
    if(getImage("Infrared", (r200Data.imgInfrared)) == -1)	return -1;
#endif

    //point cloudの取得
    //    r200Data.pntCldPtr->clear();
    //    if(getPointCloud(r200Data.pntCldPtr) == -1) return -1;

    return 0;
}

/*!
 * \brief R200::getImage
 * \param qsStream
 * \param img
 * \return
 */
int R200::getImage(QString qsStream, Mat *img)
{
    //Mat型テンポラリ作成
    cv::Mat tmpImg;

    if(!isUse){
        //		QImage _img = QImage("://IMAGE/Mandrill.bmp", "BMP");
        QImage _img = QImage("://IMAGE/LED.jpg", "JPG");
        QImage imgCvt = _img.convertToFormat(QImage::Format_RGB888);

        tmpImg= cv::Mat(imgCvt.height(),
                        imgCvt.width(),
                        CV_8UC3,
                        const_cast<uchar *>(imgCvt.bits()),
                        imgCvt.bytesPerLine());

        //		cv::cvtColor(tmpImg, tmpImg, CV_RGB2BGR);

        cv::resize(tmpImg, tmpImg,
                   cv::Size(sizeImgRGB.width(), sizeImgRGB.height()));

        tmpImg.copyTo(*img);
        tmpImg.release();
        return 0;
    }

    try{
        if(qsStream == R200_IMG_TYPES::Color){
            tmpImg = cv::Mat(sizeImgRGB.height(),
                             sizeImgRGB.width(),
                             CV_8UC3,
                             (uchar *)rsDev->get_frame_data(rs::stream::color));
        }

        if(qsStream == R200_IMG_TYPES::Depth){

            tmpImg = cv::Mat(sizeImgDepth.height(),
                             sizeImgDepth.width(),
                             CV_16UC1,
                             (uchar *)rsDev->get_frame_data(rs::stream::depth));
        }

        if(qsStream == R200_IMG_TYPES::IR){

            tmpImg = cv::Mat(sizeImgInfrared.height(),
                             sizeImgInfrared.width(),
                             CV_8UC1,
                             (uchar *)rsDev->get_frame_data(rs::stream::infrared2));
        }
    }
    catch(const rs::error &e){
        qWarning() << e.get_failed_args().c_str();
        qWarning() << e.get_failed_function().c_str();
        qWarning() << e.what();
        return -1;
    }
    catch(const cv::Exception &e){
        qWarning() << "cv::Exception :" << e.code;
        qWarning() <<	e.msg.c_str();
        return -1;
    }

    *img = tmpImg.clone();
    tmpImg.release();

    return 0;
}

/*!
 * \brief R200::Pixel2Meter
 * \param pix
 * \param depth
 * \return
 */
QPointF R200::deprojectRGB(QPoint p, double depth)
{
    if(!isUse) return QPointF(0.0, 0.0);	//center

    //    qInfo() << "Model" << (int)rsIntrRGB.model();
    //    rs::float2 uv = {(float)p.x(), (float)p.y()};
    //    rs::float3 xy = rsIntrRGB.deproject(uv, depth);

    float xyz[3] = {0.0};
    float uv[2] = {(float)p.x(), (float)p.y()};
    rsIntrRGB.rs_intrinsics::model = (rs_distortion)rs::distortion::inverse_brown_conrady;
    //    qInfo() << "Model" << (int)rsIntrRGB.model();
    rs_deproject_pixel_to_point(xyz, &rsIntrRGB, uv, (float)depth);
    //    return QPointF(xy.x, xy.y);
    return QPointF(xyz[0], xyz[1]);
}

QPoint R200::projectRGB(QPointF p, double depth)
{
    if(!isUse) return QPoint(0, 0);
    rsIntrRGB.rs_intrinsics::model = (rs_distortion)rs::distortion::modified_brown_conrady;
    rs::float3 xyz = {(float)p.x(), (float)p.y(), (float)depth};
    rs::float2 uv = rsIntrRGB.project(xyz);
    return QPoint((int)uv.x, (int)uv.y);
}

QRectF R200::getFOV(double depth)
{
    QRectF fov;
    if(!isUse){
        fov = QRectF(-2.5, -2.0, 5.0, 4.0);
        return fov;
    }

    QPointF bl = deprojectRGB(QPoint(0, sizeImgRGB.height()), depth);
    QPointF tr = deprojectRGB(QPoint(sizeImgRGB.width(), 0), depth);
    fov.setBottomLeft(bl);
    fov.setTopRight(tr);
    return fov;
}

double R200::getPixelWidth(double meter, double depth)
{
    if(!isUse){
        return 50;
    }

    QPointF _p0(0.0, 0.0);
    QPointF _p1(meter, 0.0);

    QPoint p0 = projectRGB(_p0, depth);
    QPoint p1 = projectRGB(_p1, depth);
    return (p1.x() - p0.x());
}

QPointF R200::getCenter(double depth)
{
    QPoint uv_center = QPoint(sizeImgRGB.width()/2, sizeImgRGB.height()/2);
    return QPointF(deprojectRGB(uv_center, depth));
}

void R200::setAutoColorExposure()
{
    try{
        QThread::msleep(10);
        rsDev->set_option(rs::option::color_enable_auto_exposure, 1);
        QThread::msleep(10);
    }
    catch(rs::error &e){
        qDebug() << QString::fromStdString(e.get_failed_args());
        qDebug() << QString::fromStdString(e.get_failed_function());
        qDebug() << e.what();
        exit(1);
    }
}

void R200::setColorExposure(int value)
{
    if(!isUse) return;

    try{
        QThread::msleep(10);
        rsDev->set_option(rs::option::color_enable_auto_exposure, 0);
        QThread::msleep(10);
        rsDev->set_option(rs::option::color_exposure, value);
        QThread::msleep(10);
    }
    catch(rs::error &e){
        qDebug() << QString::fromStdString(e.get_failed_args());
        qDebug() << QString::fromStdString(e.get_failed_function());
        qDebug() << e.what();
        exit(1);
    }
}

void R200::setIrExposure(int value)
{
    if(!isUse) return;

    QThread::msleep(10);
    rsDev->set_option(rs::option::r200_lr_exposure, value);
    QThread::msleep(10);
    return;
}

/*!
 * \brief R200::initDevice
 * \return
 */
int R200::initDevice()
{
    if(!isUse) return 0;

    qInfo() << "--- Search devices ---";

    //realseseデバイスの接続確認
    try{
        if(rsCntxt->get_device_count() <= 0){
            qCritical() << "Not found RealSense device";
            return -1;
        }
        else{
            qInfo() << "Found" << rsCntxt->get_device_count() << "devices";
            for(int i = 0; i < rsCntxt->get_device_count(); i++){
                qInfo() << "Device" << i << ", Serial number" << rsCntxt->get_device(i)->get_serial();
            }
        }
    }
    catch(const rs::error &e){
        qCritical() << e.get_failed_args().c_str();
        qCritical() << e.get_failed_function().c_str();
        qCritical() << e.what();
        return -1;
    }


    //rs::deviceインスタンス作成
    //シリアル番号指定がある場合は従う
    try{
        if(qsSerial == QString("none")){
            //デバイス指定されていない場合は0番デバイスをオープン
            rsDev = rsCntxt->get_device(0);
        }
        else{
            for(int i = 0; i < rsCntxt->get_device_count(); i++){
                //QString tmpQsSerial = QString(rsCntxt->get_device(i)->get_serial());

                //if(tmpQsSerial == qsSerial){
                rsDev = rsCntxt->get_device(i);
                break;
                //}
            } //end for

            if(rsDev == nullptr){
                qCritical() << "Cannot foud device at" << qsSerial;
                return -1;
            }
        }
    }
    catch(const rs::error &e){
        qCritical() << e.get_failed_args().c_str();
        qCritical() << e.get_failed_function().c_str();
        qCritical() << e.what();
        return -1;
    }

    qInfo() << " --- Opened RealSense device ---";
    qInfo() << "Device :" << rsDev->get_name() << "," << rsDev->get_serial();
    scale = rsDev->get_depth_scale();
    return 0;
}

/*!
 * \brief R200::initStreams
 * \return
 */
int R200::initStreams()
{
    if(!isUse){
        sizeImgRGB= QSize(1920, 1080);
        sizeImgDepth = QSize(640, 480);
        sizeImgInfrared= QSize(640, 480);
        return 0;
    }

    qInfo() << " --- Enable streams ---";

    try{
        //        rsDev->enable_stream(rs::stream::color, rs::preset::best_quality);//元
        rsDev->enable_stream(rs::stream::color,1920,1080,rs::format::rgb8,15);//画像サイズ編集
        //rsDev->enable_stream(rs::stream::color,1280,720,rs::format::rgb8,15);//画像サイズ編集
        //rsDev->enable_stream(rs::stream::color,320,240,rs::format::rgb8,30);//画像サイズ編集

        sizeImgRGB.setWidth(rsDev->get_stream_width(rs::stream::color));
        sizeImgRGB.setHeight(rsDev->get_stream_height(rs::stream::color));

        rsDev->enable_stream(rs::stream::depth, rs::preset::best_quality);
        //rsDev->enable_stream(rs::stream::depth,640,480, rs::format::z16,60);

        sizeImgDepth.setWidth(rsDev->get_stream_width(rs::stream::depth));
        sizeImgDepth.setHeight(rsDev->get_stream_height(rs::stream::depth));

        //赤外線画像ストリーム設定(二つのうちどちらか)
#if USE_INFRARED == 1
        rsDev->enable_stream(rs::stream::infrared2, rs::preset::best_quality);
        sizeImgInfrared.setWidth(rsDev->get_stream_width(rs::stream::infrared2));
        sizeImgInfrared.setHeight(rsDev->get_stream_height(rs::stream::infrared2));
#else
        //赤外線画像ストリームを使わないとき，適当な画像サイズを設定しておく
        sizeImgInfrared.setWidth(1);
        sizeImgInfrared.setHeight(1);
#endif

        qInfo() << "<Success all streams enable>";
        qInfo() << "Color(WxH)   :" << sizeImgRGB.width() << "x" << sizeImgRGB.height();
        qInfo() << "Depth(WxH)   :" << sizeImgDepth.width() << "x" << sizeImgDepth.height();
        qInfo() << "Infrared(WxH):" << sizeImgInfrared.width() << "x" << sizeImgInfrared.height();
    }
    catch(const rs::error &e){
        //エラーハンドリング
        qCritical() << e.get_failed_args().c_str();
        qCritical() << e.get_failed_function().c_str();
        qCritical() << e.what();
        return -1;
    }

    return 0;
}

/*!
 * \brief R200::initIntrinsicsAndExtrinsics
 * \return
 */
int R200::initIntrinsicsAndExtrinsics()
{
    if(!isUse) return 0;

    qInfo() << " --- Get intrinsics and extrinsics ---";

    try{
        //"組み込み関数(Intrinsics)"を作成
        rsIntrRGB = rsDev->get_stream_intrinsics(rs::stream::color);
        rsIntrDepth = rsDev->get_stream_intrinsics(rs::stream::depth);


#if USE_INFRARED == 1
        //rsIntrInfrared = rsDev->get_stream_intrinsics(rs::stream::infrared);
        rsIntrInfrared = rsDev->get_stream_intrinsics(rs::stream::infrared2);
#endif
        //"外部組み込み関数(extrinsics)"を作成
        //depth画像とカラー画像のマッピングを提供する
        rsExtrDepthToColor = rsDev->get_extrinsics(rs::stream::depth, rs::stream::color);
    }
    catch(const rs::error &e){
        qCritical() << e.get_failed_args().c_str();
        qCritical() << e.get_failed_function().c_str();
        qCritical() << e.what();
        return -1;
    }

    return 0;
}

/*!
 * \brief R200::startDevice
 * \return
 */
int R200::startDevice()
{
    if(!isUse) return 0;

    qInfo() << " --- Start device ---";
    try{
        rsDev->start();

        //ストリームがちゃんと動くまで待機
        while(!rsDev->is_streaming()){
            QThread::sleep(33);
        }

        qInfo() << "<Success start device>";
    }
    catch(const rs::error &e){
        qCritical() << e.get_failed_args().c_str();
        qCritical() << e.get_failed_function().c_str();
        qCritical() << e.what();
        return -1;
    }

    return 0;
}

//int R200::getPointCloud(PointCloudT::Ptr pointCloud)
//{
//    //
//    // point cloudの作成
//    //
//    uint16_t *depth_image;
//    uint8_t *color_image;

//    try{
//        depth_image = (uint16_t *)rsDev->get_frame_data(rs::stream::depth);
//        color_image = (uint8_t *)rsDev->get_frame_data(rs::stream::color);
//    }
//    catch(const rs::error &e){
//        qWarning() << e.get_failed_args().c_str();
//        qWarning() << e.get_failed_function().c_str();
//        qWarning() << e.what();
//        return -1;
//    }


//    for(int y = 0; y < sizeImgDepth.height(); y++){
//        for(int x = 0; x < sizeImgDepth.width(); x++){

//            uint i = y*rsIntrDepth.width + x;				//画像配列のインデックス計算
//            uint16_t depth_value = depth_image[i];	//白-黒の値を取得(16bit値)

//            //0ならピクセル(x, y)についてスキップ
//            if(depth_value == 0) continue;

//            //[mm]単位で三次元座標取得
//            rs::float2 depth_pixel = {(float)x, (float)y};
//            float depth_in_meters = depth_value*scale;	//[m]単位に変換

//            rs::float3 depth_point = rsIntrDepth.deproject(depth_pixel, depth_in_meters);	//[m]で三次元座標
//            //if(depth_in_meters >1.1)depth_in_meters = calibration_depth(x,y,depth_in_meters);
//            //depth_point = rsIntrDepth.deproject(depth_pixel, depth_in_meters);	//[m]で三次元座標

//            //三次元座標に対応するカラー画像上のピクセル座標を求める
//            rs::float3 color_point = rsExtrDepthToColor.transform(depth_point);	//三次元座標と色情報座標のマッピング
//            rs::float2 color_pixel = rsIntrRGB.project(color_point);				//ピクセル座標取得

//            const int cx = (int)std::round(color_pixel.x);
//            const int cy = (int)std::round(color_pixel.y);

//            // Set up logic to remove bad points
//            bool depth_fail = true;
//            bool color_fail = true;

//            depth_fail = ( depth_point.z > 10.0 );
//            color_fail = ( cx < 0 || cy < 0 || cx >= rsIntrRGB.width || cy >= rsIntrRGB.height );

//            if(depth_fail) continue;
//            if(color_fail) continue;

//            int colorindex;
//            int depthindex;
//            colorindex = cx + cy * rsIntrRGB.width;
//            depthindex = x + y * sizeImgDepth.width();

//            //ポイント(XYZRGB)のテンポラリ作成
//            PointT tmpPoint;
//            tmpPoint.x = color_point.x;
//            tmpPoint.y = color_point.y;
//            tmpPoint.z = color_point.z;

//            const uint8_t *offset = (color_image + (cy * rsIntrRGB.width + cx)*3);
//            tmpPoint.r = *(offset);
//            tmpPoint.g = *(offset + 1);
//            tmpPoint.b = *(offset + 2);

//            pointCloud->push_back(tmpPoint);
//        }
//    }

//    return 0;
//}

void R200::finishDevice()
{
    rsDev->stop();
}
