#ifndef SPOIT_IMAGEVIEWER_H
#define SPOIT_IMAGEVIEWER_H

#include <QDebug>
#include <QString>
#include <QQueue>

#include "../ImageViewer/imageviewer.h"

class Spoit_ImageViewer : public ImageViewer
{
	Q_OBJECT
public:
	explicit Spoit_ImageViewer(QString windowTitle, QWidget *parent = 0);
	~Spoit_ImageViewer();

	// QWidget interface
protected:
	void mousePressEvent(QMouseEvent *event);

signals:
	void fuzzySelected(cv::Range h, cv::Range s, cv::Range v);

private:
	void FuzzySelect(cv::Point sp);

private:
	double fuzzy_th;
	cv::Mat *imgFuzzyRes;
};

#endif // SPOIT_IMAGEVIEWER_H
