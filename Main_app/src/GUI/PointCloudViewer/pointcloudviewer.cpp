#include "pointcloudviewer.h"
#include "ui_pointcloudviewer.h"

PointCloudViewer::PointCloudViewer(QString windowTitle, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PointCloudViewer)
{
	//無駄なvtkのwarning表示コンソール画面を消しておく
	vtkObject::GlobalWarningDisplayOff();
	this->setWindowTitle(windowTitle);
	ui->setupUi(this);

	pointCloud.reset(new PointCloudT);
	pointCloud->clear();
}

PointCloudViewer::~PointCloudViewer()
{
	delete ui;
}

int PointCloudViewer::initialize()
{
	//	qInfo() << "Initialize";

	//ランダムデータ挿入
	PointCloudT::Ptr cloud;
	cloud.reset(new PointCloudT);
	cloud->points.resize(1000);

	// Fill the cloud with some points
	for (size_t i = 0; i < cloud->points.size(); ++i)
	{
		cloud->points[i].x = 1024 * rand () / (RAND_MAX + 1.0f);
		cloud->points[i].y = 1024 * rand () / (RAND_MAX + 1.0f);
		cloud->points[i].z = 1024 * rand () / (RAND_MAX + 1.0f);

		cloud->points[i].r = 128;
		cloud->points[i].g = 128;
		cloud->points[i].b = 128;
	}

	//Set up the QVTK window
	pclVis.reset(new pcl::visualization::PCLVisualizer(this->windowTitle().toStdString(), false));
	pclVis->setWindowName(this->windowTitle().toStdString());
	//↑ resetメソッドはboost::shared_ptrクラスのメンバだよ

	//レンダーとQTVKWidgetの関連付け
	//	qInfo() << "Setup QVTKWidget";
	ui->qvtkWdget->SetRenderWindow(pclVis->getRenderWindow());
	pclVis->setupInteractor(ui->qvtkWdget->GetInteractor(), ui->qvtkWdget->GetRenderWindow());
	ui->qvtkWdget->update();

	pclVis->addPointCloud(cloud, "cloud"); //id"cloud"の点群描画実態をここで作成する

	pclVis->addCoordinateSystem(0.3);
	pclVis->initCameraParameters();

	pcl::visualization::Camera camera;
	pclVis->getCameraParameters(camera);
	camera.pos[0] = 2.98;
	camera.pos[1] = -3.44;
	camera.pos[2] = -2.40;
	camera.view[0] = -0.16;
	camera.view[1] = -0.82;
	camera.view[2] = 0.53;
	pclVis->setCameraParameters(camera);

	ui->qvtkWdget->update();
	return 0;
}

void PointCloudViewer::setPointCloud(PointCloudT::Ptr _pointCloud)
{
	this->blockSignals(true);

	pointCloud->clear();
	pcl::copyPointCloud(*_pointCloud, *pointCloud);
	//	qDebug() << _pointCloud.use_count() << pointCloud.use_count();

	pclVis->updatePointCloud(pointCloud, "cloud");
	ui->qvtkWdget->update();

	ui->lblNumPoints->setText(QString::number(pointCloud->points.size()));

	pcl::visualization::Camera camera;
	pclVis->getCameraParameters(camera);
	QString qsCameraPram;
	qsCameraPram += QString("%1,").arg(camera.pos[0]);
	qsCameraPram += QString("%1,").arg(camera.pos[1]);
	qsCameraPram += QString("%1,").arg(camera.pos[2]);
	qsCameraPram += QString("%1,").arg(camera.view[0]);
	qsCameraPram += QString("%1,").arg(camera.view[1]);
	qsCameraPram += QString("%1,").arg(camera.view[2]);
	ui->lblPCLVisCameraParam->setText(qsCameraPram);

	this->blockSignals(false);
	return;
}




