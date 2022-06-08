#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <QDebug>
#include <QString>
#include <QPainter>
#include <QPaintEvent>

#include <opencv2/opencv.hpp>

namespace Ui {
class ImageViewer;
}

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewer(QString windowTitle, QWidget *parent = 0);
    ~ImageViewer();

    int initialize(int ImageType = CV_8UC3,
                   QImage::Format qImageFormat = QImage::Format_RGB888);	//初期化処理

public slots:
    void setImage(cv::Mat *img);

signals:
    void ClickedImage(QPoint);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

protected:
    Ui::ImageViewer *ui;
    cv::Mat *img;

    QRect imgRect;

    int viewimg_width;//表示するときにリサイズされたときの画像の大きさ
    int viewimg_height;

    int cvMatFormat;
    QImage::Format qImageFormat;
};

#endif // VIEWERIMAGE_H
