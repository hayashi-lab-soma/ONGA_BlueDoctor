#include "spoit_imageviewer.h"
#include <iostream>
#include <math.h>
#include <regex>
#include <QMessageBox>

Spoit_ImageViewer::Spoit_ImageViewer(QString windowTitle, QWidget *parent)
    : ImageViewer(windowTitle, parent)
{
    fuzzy_th = 0.1; //<-???
    imgFuzzyRes = new cv::Mat();
}

Spoit_ImageViewer::~Spoit_ImageViewer()
{

}

void Spoit_ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        QPoint _p = event->pos(); //clicked position in window coordinate

        if((_p.x() > imgRect.right()) || (_p.y() > imgRect.bottom())){//out of range
            //			qCritical() << __FUNCTION__ << "Over";
            //			qDebug() << _p << imgRect.right() << imgRect.bottom();
            return;
        }

        double fx = (double)img->cols/(double)imgRect.width();
        double fy = (double)img->rows/(double)imgRect.height();

        cv::Point p;
        p.x = fx*(double)_p.x();
        p.y = fy*(double)_p.y();

        if(p.x > img->cols || p.y > img->rows){
            qCritical() << __FUNCTION__ << p.x << p.y;
            exit(1);
        }

        //		cv::Vec3d scalar = img->at<cv::Vec3b>(p.y, p.x);
        //		qInfo() << scalar[0] << scalar[1] << scalar[2];
        FuzzySelect(p);
    }
}

//void Spoit_ImageViewer::FuzzySelect(cv::Point sp)
//{
//    //RGB -> HSV
//    cv::Mat matHSV;
//    cv::cvtColor(*img, matHSV, cv::COLOR_RGB2HSV);

//    QQueue<cv::Point> scanns;
//    scanns.enqueue(sp);
//    cv::Vec3b t_hsv = matHSV.at<cv::Vec3b>(sp.y, sp.x);

//    QList<cv::Point> targets;
//    targets << sp;

//    int cnt = 0;

//    //ここから時間のかかるループ
//    QMessageBox *msgbx = new QMessageBox(
//                QMessageBox::NoIcon,
//                QString("Processing ..."),
//                QString("Processing ..."),
//                QMessageBox::NoButton);
//    //    msgbx->setStandardButtons(0);
//    //    QMessageBox msgbx;
//    //    msgbx.setText("Wait for processing ...");
//    msgbx->open();

//    do{
//        cv::Point p = scanns.dequeue(); //take first element
//        //		qDebug() << cnt << ":" << scanns.count();
//        cnt++;
//        //Check 4-neighbor pixels
//        cv::Point np[4] = {cv::Point(p.x - 1, p.y),	//left
//                           cv::Point(p.x, p.y - 1),	//avobe
//                           cv::Point(p.x + 1, p.y),	//right
//                           cv::Point(p.x, p.y + 1)};//under


//        for(int i = 0; i < 4; i++){
//            //			qDebug() << "?" << np[i].x << np[i].y;
//            if(np[i].x < 0 || img->cols <= np[i].x) continue;
//            if(np[i].y < 0 || img->rows <= np[i].y) continue;

//            bool isAlready = false;
//            Q_FOREACH(cv::Point tp, targets){
//                if(np[i] == tp){
//                    isAlready = true;
//                    break;
//                }
//            }
//            if(isAlready) continue;

//            cv::Vec3b hsv = matHSV.at<cv::Vec3b>(np[i].y, np[i].x);
// //                       qDebug() << "HSV:" << hsv[0] << hsv[1] << hsv[2];
//            double max_diff = -INFINITY;

//            for(int dim = 0; dim < 3; dim++){
//                double diff = (double)abs(hsv[dim] - t_hsv[dim]);
//                if(dim == 0) diff = diff/180.0;							//Normalized only Hue coordinate
//                if(dim == 1 || dim == 2) diff = diff/255.0;	//
//                max_diff = std::max(diff, max_diff);
//            }

//            //
//            //			qDebug() << "Max diff:" << max_diff;

//            if(max_diff > fuzzy_th)	continue;
//            if(max_diff <= fuzzy_th) targets << np[i];

//            //enque
//            scanns.enqueue(np[i]);
//        }

//    }while(!scanns.isEmpty());
//    msgbx->close();
//    //	qDebug() << scanns.count() << targets.count();

//    cv::Range h, s, v;
//    h.start = 180; h.end = 0;
//    s.start = 255; s.end = 0;
//    v.start = 255; v.end = 0;

// //   Q_FOREACH(cv::Point p, targets){
// //       cv::Vec3b hsv = matHSV.at<cv::Vec3b>(p.y, p.x);
// //       h.start = std::min<int>(h.start, hsv[0]); h.end = std::max<int>(h.end, hsv[0]);
// //       s.start = std::min<int>(s.start, hsv[1]); s.end = std::max<int>(s.end, hsv[1]);
// //       v.start = std::min<int>(v.start, hsv[2]); v.end = std::max<int>(v.end, hsv[2]);
// //   }


//    //Blue or Green?
//    switch(color_tag) {
//    case 0  :
//        //set HSV range
//        Q_FOREACH(cv::Point p, targets){
//            cv::Vec3b hsv = matHSV.at<cv::Vec3b>(p.y, p.x);
//            h.start = std::min<int>(h.start, hsv[0]-10); h.end = std::max<int>(h.end, hsv[0]+10);
//            s.start = std::min<int>(s.start, hsv[1]-30); s.end = std::max<int>(s.end, hsv[1]+30);
//            v.start = std::min<int>(v.start, hsv[2]-50); v.end = std::max<int>(v.end, hsv[2]+50);
//        }
//        emit fuzzySelectedBlue(h, s, v);
//        break;
//    case 1  :
//        Q_FOREACH(cv::Point p, targets){
//            cv::Vec3b hsv = matHSV.at<cv::Vec3b>(p.y, p.x);
//            h.start = std::min<int>(h.start, hsv[0]-10); h.end = std::max<int>(h.end, hsv[0]+10);
//            s.start = std::min<int>(s.start, hsv[1]-75); s.end = std::max<int>(s.end, hsv[1]+75);
//            v.start = std::min<int>(v.start, hsv[2]-50); v.end = std::max<int>(v.end, hsv[2]+50);
//        }
//        emit fuzzySelectedGreen(h, s, v);
//        break;
//    }

//    qDebug() << "h:" << t_hsv(0);
//    qDebug() << "h.start:" << h.start;
//    qDebug() << "h.end:" << h.end;
//    qDebug() << "s:" << t_hsv(1);
//    qDebug() << "s.start:" << s.start;
//    qDebug() << "s.end:" << s.end;
//    qDebug() << "v:" << t_hsv(2);
//    qDebug() << "v.start:" << v.start;
//    qDebug() << "v.end:" << v.end;
//    qInfo() << "----------------";

//}
void Spoit_ImageViewer::FuzzySelect(cv::Point sp)
{
    //RGB -> HSV
    cv::Mat matHSV;
    cv::cvtColor(*img, matHSV, cv::COLOR_RGB2HSV);


    cv::Vec3b t_hsv = matHSV.at<cv::Vec3b>(sp.y, sp.x);


    cv::Range h, s, v;

//        QMessageBox *msgbx = new QMessageBox(
//                    QMessageBox::NoIcon,
//                    QString("Processing ..."),
//                    QString("Blue DoctorとLEDを選んでください"),
//                    QMessageBox::NoButton);
//        //    msgbx->setStandardButtons(0);
//        //    QMessageBox msgbx;
//        //    msgbx.setText("Wait for processing ...");
//        msgbx->open();

    //Blue or Green?
    switch(color_tag) {
    case 0  :
        //set HSV range
        //blue range
        //10,30,50 ->10,50,50 ->12,65,65
        if(t_hsv[0] >= 40 && t_hsv[0] <= 140){//赤に近い色は排除
            h.start = t_hsv[0]-12;h.end = t_hsv[0]+12;
            s.start = t_hsv[1]-65;s.end = t_hsv[1]+65;
            v.start = t_hsv[2]-65;v.end = t_hsv[2]+65;
            emit fuzzySelectedBlue(h, s, v);
        }else{

                QMessageBox *msgbx = new QMessageBox(
                            QMessageBox::NoIcon,
                            QString("caution"),
                            QString("青か緑を選んでください"),
                            QMessageBox::NoButton);
//                msgbx->setStandardButtons(0);
                //    QMessageBox msgbx;
                //    msgbx.setText("Wait for processing ...");
                msgbx->open();
        }
        break;
    case 1  :
        //green range
        //10,75,50 ->12,90,65
        if(t_hsv[0] >= 40 && t_hsv[0] <= 140){
            h.start = t_hsv[0]-12;h.end = t_hsv[0]+12;
            s.start = t_hsv[1]-90;s.end = t_hsv[1]+90;
            v.start = t_hsv[2]-65;v.end = t_hsv[2]+65;
            emit fuzzySelectedGreen(h, s, v);
        }else{
            QMessageBox *msgbx = new QMessageBox(
                        QMessageBox::NoIcon,
                        QString("caution"),
                        QString("青か緑を選んでください"),
                        QMessageBox::NoButton);
//                msgbx->setStandardButtons(0);
            //    QMessageBox msgbx;
            //    msgbx.setText("Wait for processing ...");
            msgbx->open();
        }
        break;
    }

    qDebug() << "h:" << t_hsv(0);
    qDebug() << "h.start:" << h.start;
    qDebug() << "h.end:" << h.end;
    qDebug() << "s:" << t_hsv(1);
    qDebug() << "s.start:" << s.start;
    qDebug() << "s.end:" << s.end;
    qDebug() << "v:" << t_hsv(2);
    qDebug() << "v.start:" << v.start;
    qDebug() << "v.end:" << v.end;
    qInfo() << "----------------";

}
