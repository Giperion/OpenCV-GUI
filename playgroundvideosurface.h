//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#pragma once
#include <QAbstractVideoSurface>

struct PlaygroundImage;

class PlaygroundVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    PlaygroundVideoSurface();
    virtual ~PlaygroundVideoSurface();

    virtual bool present(const QVideoFrame &frame) override;
    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;

    void setRetranslatorObject(QAbstractVideoSurface* surface);

Q_SIGNALS:
    void update();

private:
    void UpdateImage(const QVideoFrame &frame);

    QAbstractVideoSurface* retranslatorObject;

    //Image ptr in PlaygroundApp, using to share image data for OpenCV thread
    PlaygroundImage* originalImage;

};
