cmake_minimum_required(VERSION 3.5)

project(Main_app LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Charts Concurrent PrintSupport Serialport Multimedia)

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(${QT_MESSAGELOGCONTEXT})

include_directories(${Qt5Core_INCLUDE_DIRS})        #
include_directories(${Qt5Widgets_INCLUDE_DIRS})     #
include_directories(${Qt5Concurrent_INCLUDE_DIRS})  #
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})#
include_directories(${Qt5SerialPort_INCLUDE_DIRS})
include_directories(${Qt5Multimedia_INCLUDE_DIRS})

#include common classes for intellisense
include_directories(.\\src)

#librealsense
include_directories(C:\\librealsense1.12.4\\include)
link_directories(C:\\librealsense1.12.4\\lib\\x64)
include_directories(C:\\librealsense\\include)
link_directories(C:\\librealsense\\lib)
include_directories(C:\\Realsense\\include)
link_directories(C:\\Realsense\\lib)

#librealsense2
include_directories(C:\\librealsense2\\include)
link_directories(C:\\librealsense2\\lib)
link_directories(C:\\librealsense2\\lib\\x64)

#OpenCV
#set(OpenCV_DIR $ENV{OpenCV_ROOT})
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#for resource files
QT5_ADD_RESOURCES(rc ${CMAKE_CURRENT_SOURCE_DIR}/resource/rsc.qrc)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resource/config.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resource/green_target.png
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resource/spoit.png
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resource/grids.png
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resource/tracker.png
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource)

add_executable(Main_app
    ./src/main.cpp
    ./src/mainwindow.h
    ./src/mainwindow.cpp
    ./src/mainwindow.ui
    ./src/Config/config.h
    ./src/Config/config.cpp
    ./src/Data/data.h
    ./src/Data/data.cpp
    ./src/Vision/vision.h
    ./src/Vision/vision.cpp
    ./src/Vision/R200/r200.h
    ./src/Vision/R200/r200.cpp
    ./src/Vision/D400s/d400s.h
    ./src/Vision/D400s/d400s.cpp
    ./src/BlueDoctorReceiver/bluedocterreceiver.h
    ./src/BlueDoctorReceiver/bluedocterreceiver.cpp
    ./src/BlueDoctorReceiver/bluedocterreceiver.ui
    ./src/Alarm/alarm.h
    ./src/Alarm/alarm.cpp
    ./src/GUI/ImageViewer/imageviewer.h
    ./src/GUI/ImageViewer/imageviewer.cpp
    ./src/GUI/ImageViewer/imageviewer.ui
    ./src/GUI/PlaneViewer/planeviewer.h
    ./src/GUI/PlaneViewer/planeviewer.cpp
    ./src/GUI/PlaneViewer/planeviewer.ui
    ./src/GUI/MainController/maincontroller.h
    ./src/GUI/MainController/maincontroller.cpp
    ./src/GUI/MainController/maincontroller.ui
    ./src/GUI/HSVRangeController/RangeController/rangecontroller.h
    ./src/GUI/HSVRangeController/RangeController/rangecontroller.cpp
    ./src/GUI/HSVRangeController/RangeController/rangecontroller.ui
    ./src/GUI/HSVRangeController/hsvrangecontroller.h
    ./src/GUI/HSVRangeController/hsvrangecontroller.cpp
    ./src/GUI/HSVRangeController/hsvrangecontroller.ui
    ./src/GUI/CameraParameterController/cameraparametercontoller.h
    ./src/GUI/CameraParameterController/cameraparametercontoller.cpp
    ./src/GUI/CameraParameterController/cameraparametercontoller.ui
    ./src/GUI/ParameterController/parametercontroller.h
    ./src/GUI/ParameterController/parametercontroller.cpp
    ./src/GUI/ParameterController/parametercontroller.ui
    ./src/3rdParty/qcustomplot/qcustomplot.h
    ./src/3rdParty/qcustomplot/qcustomplot.cpp
    ${rc})

target_link_libraries(Main_app PRIVATE
    Qt5::Widgets
    Qt5::PrintSupport #Library fo 2-D plane
    Qt5::SerialPort #Library for Communication to Bluetooth
    Qt5::Multimedia #Library for sound control
    ${OpenCV_LIBRARIES}
    realsense
    realsense2)
