#-------------------------------------------------
#
# Project created by QtCreator 2017-09-05T13:10:29
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets qml quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenCVPlayground
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    playgroundapp.cpp \
    playgroundvideosurface.cpp \
    imageprocessorthread.cpp \
    videooutputwidget.cpp

HEADERS  += mainwindow.h \
    playgroundapp.h \
    playgroundvideosurface.h \
    imageprocessorthread.h \
    videooutputwidget.h

FORMS    += mainwindow.ui

RC_ICONS += OpenCV-Playground-Logo-256px.ico
VERSION = 1.2017.09.15
QMAKE_TARGET_COPYRIGHT = © Giperion


win32:CONFIG(release, debug|release): LIBS += -L'F:/[EUREKA]/OpenCV/opencv/build/x64/vc14/lib/' -lopencv_world330
else:win32:CONFIG(debug, debug|release): LIBS += -L'F:/[EUREKA]/OpenCV/opencv/build/x64/vc14/lib/' -lopencv_world330d

INCLUDEPATH += 'F:/[EUREKA]/OpenCV/opencv/build/include'
DEPENDPATH += 'F:/[EUREKA]/OpenCV/opencv/build/include'

RESOURCES += \
    opencvplaygroundresources.qrc

DISTFILES += \
    QMLTest.qml \
    VideoOutputWidget.qml
