#-------------------------------------------------
#
# Project created by QtCreator 2019-07-18T15:50:25
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport multimedia

TARGET = ONGProj
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        ./src/main.cpp \
        ./src/mainwindow.cpp \
    src/GUI/ImageViewer/imageviewer.cpp \
    src/Vision/vision.cpp \
    src/Vision/R200/r200.cpp \
    src/Vision/Images/images.cpp \
    src/GUI/HSVRangeController/RangeController/rangecontroller.cpp \
    src/GUI/HSVRangeController/hsvrangecontroller.cpp \
    src/Data/data.cpp \
    src/GUI/PointCloudViewer/pointcloudviewer.cpp \
    src/Config/config.cpp \
    src/GUI/PlaneViewer/planeviewer.cpp \
    src/3rdParty/qcustomplot/qcustomplot.cpp \
    src/GUI/CameraParameterController/cameraparametercontoller.cpp \
    src/GUI/ParameterController/parametercontroller.cpp \
    src/GUI/MainController/maincontroller.cpp \
    src/BlueDoctorReceiver/bluedocterreceiver.cpp \
    src/FSM/statebase.cpp \
    src/FSM/stanby.cpp \
    src/FSM/measurement.cpp \
    src/FSM/refresh.cpp \
    src/FSM/fsm.cpp \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.cpp \
    src/3rdParty/qcustomplot/qcustomplot.cpp \
    src/Alarm/alarm.cpp \
    src/BlueDoctorReceiver/bluedocterreceiver.cpp \
    src/Config/config.cpp \
    src/Data/data.cpp \
    src/FSM/fsm.cpp \
    src/FSM/measurement.cpp \
    src/FSM/refresh.cpp \
    src/FSM/stanby.cpp \
    src/FSM/statebase.cpp \
    src/GUI/CameraParameterController/cameraparametercontoller.cpp \
    src/GUI/HSVRangeController/RangeController/rangecontroller.cpp \
    src/GUI/HSVRangeController/hsvrangecontroller.cpp \
    src/GUI/ImageViewer/imageviewer.cpp \
    src/GUI/MainController/maincontroller.cpp \
    src/GUI/ParameterController/parametercontroller.cpp \
    src/GUI/PlaneViewer/planeviewer.cpp \
    src/GUI/PointCloudViewer/pointcloudviewer.cpp \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.cpp \
    src/Vision/Images/images.cpp \
    src/Vision/R200/r200.cpp \
    src/Vision/vision.cpp \
    src/main.cpp \
    build/qrc_rsc.cpp \
    src/3rdParty/qcustomplot/qcustomplot.cpp \
    src/Alarm/alarm.cpp \
    src/BlueDoctorReceiver/bluedocterreceiver.cpp \
    src/Config/config.cpp \
    src/Data/data.cpp \
    src/FSM/fsm.cpp \
    src/FSM/measurement.cpp \
    src/FSM/refresh.cpp \
    src/FSM/stanby.cpp \
    src/FSM/statebase.cpp \
    src/GUI/CameraParameterController/cameraparametercontoller.cpp \
    src/GUI/HSVRangeController/RangeController/rangecontroller.cpp \
    src/GUI/HSVRangeController/hsvrangecontroller.cpp \
    src/GUI/ImageViewer/imageviewer.cpp \
    src/GUI/MainController/maincontroller.cpp \
    src/GUI/ParameterController/parametercontroller.cpp \
    src/GUI/PlaneViewer/planeviewer.cpp \
    src/GUI/PointCloudViewer/pointcloudviewer.cpp \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.cpp \
    src/Vision/Images/images.cpp \
    src/Vision/R200/r200.cpp \
    src/Vision/vision.cpp \
    src/main.cpp \
    src/3rdParty/qcustomplot/qcustomplot.cpp \
    src/BlueDoctorReceiver/bluedocterreceiver.cpp \
    src/Config/config.cpp \
    src/Data/data.cpp \
    src/FSM/fsm.cpp \
    src/FSM/measurement.cpp \
    src/FSM/refresh.cpp \
    src/FSM/stanby.cpp \
    src/FSM/statebase.cpp \
    src/GUI/CameraParameterController/cameraparametercontoller.cpp \
    src/GUI/HSVRangeController/RangeController/rangecontroller.cpp \
    src/GUI/HSVRangeController/hsvrangecontroller.cpp \
    src/GUI/ImageViewer/imageviewer.cpp \
    src/GUI/MainController/maincontroller.cpp \
    src/GUI/ParameterController/parametercontroller.cpp \
    src/GUI/PlaneViewer/planeviewer.cpp \
    src/GUI/PointCloudViewer/pointcloudviewer.cpp \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.cpp \
    src/Vision/Images/images.cpp \
    src/Vision/R200/r200.cpp \
    src/Vision/vision.cpp \
    src/main.cpp \
    src/3rdParty/qcustomplot/qcustomplot.cpp \
    src/Alarm/alarm.cpp \
    src/BlueDoctorReceiver/bluedocterreceiver.cpp \
    src/Config/config.cpp \
    src/Data/data.cpp \
    src/FSM/fsm.cpp \
    src/FSM/measurement.cpp \
    src/FSM/refresh.cpp \
    src/FSM/stanby.cpp \
    src/FSM/statebase.cpp \
    src/GUI/CameraParameterController/cameraparametercontoller.cpp \
    src/GUI/HSVRangeController/RangeController/rangecontroller.cpp \
    src/GUI/HSVRangeController/hsvrangecontroller.cpp \
    src/GUI/ImageViewer/imageviewer.cpp \
    src/GUI/MainController/maincontroller.cpp \
    src/GUI/ParameterController/parametercontroller.cpp \
    src/GUI/PlaneViewer/planeviewer.cpp \
    src/GUI/PointCloudViewer/pointcloudviewer.cpp \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.cpp \
    src/Vision/Images/images.cpp \
    src/Vision/R200/r200.cpp \
    src/Vision/vision.cpp \
    src/main.cpp \
    build/CMakeFiles/3.10.0/CompilerIdC/CMakeCCompilerId.c \
    src/3rdParty/qcustomplot/qcustomplot.cpp \
    src/3rdParty/qcustomplot/qcustomplot.cpp \
    src/Alarm/alarm.cpp \
    src/BlueDoctorReceiver/bluedocterreceiver.cpp \
    src/Config/config.cpp \
    src/Data/data.cpp \
    src/FSM/fsm.cpp \
    src/FSM/measurement.cpp \
    src/FSM/refresh.cpp \
    src/FSM/stanby.cpp \
    src/FSM/statebase.cpp \
    src/GUI/CameraParameterController/cameraparametercontoller.cpp \
    src/GUI/HSVRangeController/RangeController/rangecontroller.cpp \
    src/GUI/HSVRangeController/hsvrangecontroller.cpp \
    src/GUI/ImageViewer/imageviewer.cpp \
    src/GUI/MainController/maincontroller.cpp \
    src/GUI/ParameterController/parametercontroller.cpp \
    src/GUI/PlaneViewer/planeviewer.cpp \
    src/GUI/PointCloudViewer/pointcloudviewer.cpp \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.cpp \
    src/Vision/Images/images.cpp \
    src/Vision/R200/r200.cpp \
    src/Vision/vision.cpp \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS += \
        ./src/mainwindow.h \
    src/GUI/ImageViewer/imageviewer.h \
    src/Vision/vision.h \
    src/Vision/R200/r200.h \
    src/Vision/Images/images.h \
    src/GUI/HSVRangeController/RangeController/rangecontroller.h \
    src/GUI/HSVRangeController/hsvrangecontroller.h \
    src/Data/data.h \
    src/GUI/PointCloudViewer/pointcloudviewer.h \
    src/Config/config.h \
    src/GUI/PlaneViewer/planeviewer.h \
    src/3rdParty/qcustomplot/qcustomplot.h \
    src/GUI/CameraParameterController/cameraparametercontoller.h \
    src/GUI/ParameterController/parametercontroller.h \
    src/GUI/MainController/maincontroller.h \
    src/BlueDoctorReceiver/bluedocterreceiver.h \
    src/FSM/statebase.h \
    src/FSM/stanby.h \
    src/FSM/measurement.h \
    src/FSM/refresh.h \
    src/FSM/fsm.h \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.h \
    src/3rdParty/qcustomplot/qcustomplot.h \
    src/BlueDoctorReceiver/bluedocterreceiver.h \
    src/Config/config.h \
    src/Data/data.h \
    src/FSM/fsm.h \
    src/FSM/measurement.h \
    src/FSM/refresh.h \
    src/FSM/stanby.h \
    src/FSM/statebase.h \
    src/GUI/CameraParameterController/cameraparametercontoller.h \
    src/GUI/HSVRangeController/RangeController/rangecontroller.h \
    src/GUI/HSVRangeController/hsvrangecontroller.h \
    src/GUI/ImageViewer/imageviewer.h \
    src/GUI/MainController/maincontroller.h \
    src/GUI/ParameterController/parametercontroller.h \
    src/GUI/PlaneViewer/planeviewer.h \
    src/GUI/PointCloudViewer/pointcloudviewer.h \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.h \
    src/Vision/Images/images.h \
    src/Vision/R200/r200.h \
    src/Vision/vision.h \
    src/mainwindow.h \
    src/3rdParty/qcustomplot/qcustomplot.h \
    src/Alarm/alarm.h \
    src/BlueDoctorReceiver/bluedocterreceiver.h \
    src/Config/config.h \
    src/Data/data.h \
    src/FSM/fsm.h \
    src/FSM/measurement.h \
    src/FSM/refresh.h \
    src/FSM/stanby.h \
    src/FSM/statebase.h \
    src/GUI/CameraParameterController/cameraparametercontoller.h \
    src/GUI/HSVRangeController/RangeController/rangecontroller.h \
    src/GUI/HSVRangeController/hsvrangecontroller.h \
    src/GUI/ImageViewer/imageviewer.h \
    src/GUI/MainController/maincontroller.h \
    src/GUI/ParameterController/parametercontroller.h \
    src/GUI/PlaneViewer/planeviewer.h \
    src/GUI/PointCloudViewer/pointcloudviewer.h \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.h \
    src/Vision/Images/images.h \
    src/Vision/R200/r200.h \
    src/Vision/vision.h \
    src/mainwindow.h \
    src/3rdParty/qcustomplot/qcustomplot.h \
    src/BlueDoctorReceiver/bluedocterreceiver.h \
    src/Config/config.h \
    src/Data/data.h \
    src/FSM/fsm.h \
    src/FSM/measurement.h \
    src/FSM/refresh.h \
    src/FSM/stanby.h \
    src/FSM/statebase.h \
    src/GUI/CameraParameterController/cameraparametercontoller.h \
    src/GUI/HSVRangeController/RangeController/rangecontroller.h \
    src/GUI/HSVRangeController/hsvrangecontroller.h \
    src/GUI/ImageViewer/imageviewer.h \
    src/GUI/MainController/maincontroller.h \
    src/GUI/ParameterController/parametercontroller.h \
    src/GUI/PlaneViewer/planeviewer.h \
    src/GUI/PointCloudViewer/pointcloudviewer.h \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.h \
    src/Vision/Images/images.h \
    src/Vision/R200/r200.h \
    src/Vision/vision.h \
    src/mainwindow.h \
    src/3rdParty/qcustomplot/qcustomplot.h \
    src/Alarm/alarm.h \
    src/BlueDoctorReceiver/bluedocterreceiver.h \
    src/Config/config.h \
    src/Data/data.h \
    src/FSM/fsm.h \
    src/FSM/measurement.h \
    src/FSM/refresh.h \
    src/FSM/stanby.h \
    src/FSM/statebase.h \
    src/GUI/CameraParameterController/cameraparametercontoller.h \
    src/GUI/HSVRangeController/RangeController/rangecontroller.h \
    src/GUI/HSVRangeController/hsvrangecontroller.h \
    src/GUI/ImageViewer/imageviewer.h \
    src/GUI/MainController/maincontroller.h \
    src/GUI/ParameterController/parametercontroller.h \
    src/GUI/PlaneViewer/planeviewer.h \
    src/GUI/PointCloudViewer/pointcloudviewer.h \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.h \
    src/Vision/Images/images.h \
    src/Vision/R200/r200.h \
    src/Vision/vision.h \
    src/mainwindow.h \
    src/3rdParty/qcustomplot/qcustomplot.h \
    src/3rdParty/qcustomplot/qcustomplot.h \
    src/Alarm/alarm.h \
    src/BlueDoctorReceiver/bluedocterreceiver.h \
    src/Config/config.h \
    src/Data/data.h \
    src/FSM/fsm.h \
    src/FSM/measurement.h \
    src/FSM/refresh.h \
    src/FSM/stanby.h \
    src/FSM/statebase.h \
    src/GUI/CameraParameterController/cameraparametercontoller.h \
    src/GUI/HSVRangeController/RangeController/rangecontroller.h \
    src/GUI/HSVRangeController/hsvrangecontroller.h \
    src/GUI/ImageViewer/imageviewer.h \
    src/GUI/MainController/maincontroller.h \
    src/GUI/ParameterController/parametercontroller.h \
    src/GUI/PlaneViewer/planeviewer.h \
    src/GUI/PointCloudViewer/pointcloudviewer.h \
    src/GUI/Spoit_ImageViewer/spoit_imageviewer.h \
    src/Vision/Images/images.h \
    src/Vision/R200/r200.h \
    src/Vision/vision.h \
    src/mainwindow.h

FORMS += \
        ./src/mainwindow.ui \
    src/GUI/ImageViewer/imageviewer.ui \
    src/GUI/HSVRangeController/RangeController/rangecontroller.ui \
    src/GUI/HSVRangeController/hsvrangecontroller.ui \
    src/GUI/PointCloudViewer/pointcloudviewer.ui \
    src/GUI/PlaneViewer/planeviewer.ui \
    src/GUI/CameraParameterController/cameraparametercontoller.ui \
    src/GUI/ParameterController/parametercontroller.ui \
    src/GUI/MainController/maincontroller.ui \
    src/BlueDoctorReceiver/bluedocterreceiver.ui

DISTFILES += \
    src/CMakeLists.txt \
resource/config.ini

INCLUDEPATH += $$PWD/'../../../../../../../../Program Files/Realsense/include'
DEPENDPATH += $$PWD/'../../../../../../../../Program Files/Realsense/include'
INCLUDEPATH += 'C:\librealsense\include'
DEPENDPATH += 'C:\librealsense\include'

#INCLUDEPATH += $$PWD/../../../../../../../../opencv/opencv344/build/include
#DEPENDPATH += $$PWD/../../../../../../../../opencv/opencv344/build/include
#INCLUDEPATH += 'C:\opencv340\opencv\build\include'
#DEPENDPATH += 'C:\opencv340\opencv\build\include'

#add
INCLUDEPATH += $$PWD/'../../../../../../../../Program Files/opencv341/opencv/build/include'
DEPENDPATH += $$PWD/'../../../../../../../../Program Files/opencv341/opencv/build/include'
INCLUDEPATH += 'C:\Program Files\opencv341\opencv\build\include'
DEPENDPATH += 'C:\Program Files\opencv341\opencv\build\include'

INCLUDEPATH += 'C:\Program Files\PCL\include\pcl-1.8'
DEPENDPATH += 'C:\Program Files\PCL\include\pcl-1.8'

INCLUDEPATH += 'C:\Program Files\PCL\3rdParty\Boost\include\boost-1_64'
DEPENDPATH += 'C:\Program Files\PCL\3rdParty\Boost\include\boost-1_64'

RESOURCES += \
resource/rsc.qrc


#??kinoshita
INCLUDEPATH += 'C:\opencv440\build\include'
win32:CONFIG(release, debug|release): LIBS += -L'C:\opencv440\build\x64\vc15\lib' -lopencv_world440
else:win32:CONFIG(debug, debug|release): LIBS += -L'C:\opencv440\build\x64\vc15\lib' -lopencv_world440d
