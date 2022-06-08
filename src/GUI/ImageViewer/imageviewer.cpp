#include "imageviewer.h"
#include "ui_imageviewer.h"

ImageViewer::ImageViewer(QString windowTitle, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageViewer)
{
    ui->setupUi(this);
    this->setWindowTitle(windowTitle);
}

ImageViewer::~ImageViewer()
{
    delete ui;
}

int ImageViewer::initialize(int ImageType, QImage::Format qImageFormat)
{
    img = new cv::Mat(this->height(), this->width(), ImageType, cv::Scalar::all(0));
    this->cvMatFormat = ImageType;
    this->qImageFormat = qImageFormat;
    return 0;
}

void ImageViewer::setImage(cv::Mat *img)
{
    //メンバに複製
    if(cvMatFormat == CV_8UC3) {
        img->copyTo(*(this->img));
    }
    if(cvMatFormat == CV_16UC1){
        cv::Mat tmp;
        cv::resize(*img, tmp, cv::Size(this->img->cols, this->img->rows));
        tmp.convertTo(*(this->img), CV_8UC1, 255/1000.0);
    }
    if(cvMatFormat == CV_8UC1) {
        img->copyTo(*(this->img));
    }

    update();	//paintEvent関数の呼び出しシグナル
    return;
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    blockSignals(true);
    QPainter *painter = new QPainter(this);

    QImage qImg;
    qImg = QImage(this->img->data,
                  this->img->cols,
                  this->img->rows,
                  this->qImageFormat);

    //リサイズ -> ウィジェットのサイズに合わせる
    QImage qImgResized = qImg.scaled(this->width(),
                                     this->height(),
                                     Qt::KeepAspectRatio);

    imgRect = qImgResized.rect();

    viewimg_width = qImgResized.width();
    viewimg_height = qImgResized.height();

    //描画処理
    painter->drawImage(QPoint(0, 0), qImgResized);
    delete painter;
    blockSignals(false);
    return;
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    //	QPoint image_point;//画像上の座標
    //	if(event->button() == Qt::LeftButton){
    //		image_point.setX(event->x() * ((double)(this->img->cols) * 4/(double)(viewimg_width)));//画像は縮小されているのでクリック座標から画像座標に変換する必要がある
    //		image_point.setY(event->y() * ((double)(this->img->rows) * 4/(double)(viewimg_height)));
    //	}
    //	emit ClickedImage(image_point);
}


