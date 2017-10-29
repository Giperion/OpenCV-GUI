//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#include "mainwindow.h"
#include <QApplication>
#include <QGuiApplication>
#include <QtQml>
#include "playgroundapp.h"
#include "imageprocessorthread.h"
#include <QObject>

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    //if (!w.Initialize()) return 1;

    //ImageProcessorThread imageThread;
    //gApp.lifetimeController = 1;
    //QObject::connect(&imageThread, SIGNAL(haveMessage(QString)), &w, SLOT(OnCVThreadMessage(QString)));
    //imageThread.start();

    //int RetCode = a.exec();
    //gApp.lifetimeController = 0;
    //imageThread.wait();
    //return RetCode;

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine qmlEngine;
    qmlEngine.load(QUrl("qrc:/ui/QMLTest.qml"));
    QList<QObject*> objList = qmlEngine.rootObjects();

    gApp.lifetimeController = 0;
    //imageThread.wait();
    return app.exec();
}
