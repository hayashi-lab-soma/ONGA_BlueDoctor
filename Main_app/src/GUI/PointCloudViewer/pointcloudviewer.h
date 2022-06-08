#ifndef VIEWER3D_H
#define VIEWER3D_H

#include <vtkAutoInit.h>
#define vtkRenderingCore_AUTOINIT 2(vtkRenderingOpenGL2, vtkInteractionStyle)

#include <QWidget>
#include <QDebug>
#include <QString>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>

#include <vtkObject.h>
#include <vtkRenderWindow.h>

#include <opencv2/opencv.hpp>


typedef pcl::PointXYZRGB PointT;
typedef pcl::PointCloud<PointT> PointCloudT;

namespace Ui {
	class PointCloudViewer;
}

class PointCloudViewer : public QWidget
{
	Q_OBJECT

public:
	explicit PointCloudViewer(QString windowTitle = QString("None"), QWidget *parent = 0);
	~PointCloudViewer();

	int initialize();

public slots:
	void setPointCloud(PointCloudT::Ptr cloud);

protected:
	boost::shared_ptr<pcl::visualization::PCLVisualizer> pclVis;
//	pcl::visualization::PCLVisualizer *pclVis;

private:
	Ui::PointCloudViewer *ui;
	PointCloudT::Ptr pointCloud;

//	QCPGraph *qcpgXYPlane;
//	QCPGraph *qcpgXZPlane;
};

#endif // VIEWER3D_H
