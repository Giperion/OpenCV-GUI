//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#pragma once
#include <QCamera>
#include <QMutex>
#include <QMap>
#include <QAtomicInteger>

namespace cv
{
    class Mat;
}

class PlaygroundVideoSurface;
class QImage;

struct PlaygroundImage
{
    QString name;
    uchar* imageData;
    quint64 imageBytesCount;
    QSize imageSize;
    QMutex imageDataGuard;

    PlaygroundImage()
        : name ("ERROR"), imageData(nullptr), imageBytesCount(0)
    {
    }

    PlaygroundImage(const PlaygroundImage& other);

    PlaygroundImage& operator=(const PlaygroundImage& other);

    void GiveImage(uchar* inImageData, quint64 bytesCount, QSize& inImageSize);
    void TakeImage(uchar*& outImageData, quint64& outBytesCount, QSize& outImageSize);
};

bool operator== (const PlaygroundImage& left, const PlaygroundImage& right);

class PlaygroundApp : public QObject
{
    Q_OBJECT
public:

    PlaygroundApp();

    void ProcessLoop();


    int currentCameraIndx;
    QCamera* camera;
    PlaygroundVideoSurface* imageSurface;
    QImage* videoOutput;

    QAtomicInt lifetimeController;
    
    PlaygroundImage* RegisterImage(QString name);
    void UnregisterImage(QString name);
    int GetImageIndexByName(QString imageName);
    PlaygroundImage* GetImageByIndex(int Index);

signals:
    void OnImageRegistered(QString imageName);
    void OnImageUnRegistered(QString imageName);
    void OnOpenCVThreadShutdowned();

private:

    void imageProcessingTests(cv::Mat imageCVRGB);

    QMap<int,PlaygroundImage> sharedImages;
};

extern PlaygroundApp gApp;