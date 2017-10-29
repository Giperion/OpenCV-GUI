//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#pragma once
#include <QThread>

class ImageProcessorThread : public QThread
{
    Q_OBJECT
public:
    ImageProcessorThread(QObject* parent = nullptr);

signals:
    void haveMessage(QString message);
protected:
    virtual void run() override;
};