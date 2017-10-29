//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#include "playgroundvideosurface.h"
#include "playgroundapp.h"

PlaygroundVideoSurface::PlaygroundVideoSurface()
{
    retranslatorObject = nullptr;
    originalImage = gApp.RegisterImage("Original");
}

PlaygroundVideoSurface::~PlaygroundVideoSurface()
{
    gApp.UnregisterImage("Original");
}

bool PlaygroundVideoSurface::present(const QVideoFrame &frame)
{
    QVideoFrame::PixelFormat pixFormat = frame.pixelFormat();

    //If you hit here, see hardcoded part at the bottom
    Q_ASSERT(pixFormat == QVideoFrame::PixelFormat::Format_RGB32);

    if (retranslatorObject != nullptr)
    {
        retranslatorObject->present(frame);
    }

    QSize& imageSize = frame.size();
    if (gApp.videoOutput == nullptr)
    {
        //#HARDCODE Pixel format
        gApp.videoOutput = new QImage(imageSize.width(), imageSize.height(), QImage::Format::Format_RGB32);
    }
    else
    {
        //handle conditional resize
        if (gApp.videoOutput->size() != imageSize)
        {
            delete gApp.videoOutput;
            gApp.videoOutput = new QImage(imageSize.width(), imageSize.height(), QImage::Format::Format_RGB32);
        }
    }

    UpdateImage(frame);

    //signal that we have a new original frame
    update();

    return true;
}

QList<QVideoFrame::PixelFormat> PlaygroundVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType /*= QAbstractVideoBuffer::NoHandle*/) const
{
    QList<QVideoFrame::PixelFormat> PixelFormats;

    PixelFormats.append(QVideoFrame::PixelFormat::Format_RGB32);
    return PixelFormats;
}

void PlaygroundVideoSurface::setRetranslatorObject(QAbstractVideoSurface* surface)
{
    Q_ASSERT(surface);

    retranslatorObject = surface;
}


void PlaygroundVideoSurface::UpdateImage(const QVideoFrame &frame)
{
    //#HACK
    QVideoFrame& mutableFrame = const_cast <QVideoFrame&>(frame);

    mutableFrame.map(QAbstractVideoBuffer::MapMode::ReadOnly);

    int allBytes = mutableFrame.mappedBytes();

#if 0
    uchar* InFrameData = mutableFrame.bits();
    uchar* OutFrameData = gApp.videoOutput->bits();

    int allPixelsCount = allBytes / 4;
    quint32* InFramePixData = (quint32*)InFrameData;
    quint32* OutFramePixData = (quint32*)OutFrameData;

    //perform special copy
    for (int pixelID = 0; pixelID < allPixelsCount; ++pixelID)
    {
        int ReversedIterator = allPixelsCount - pixelID;
        OutFramePixData[pixelID] = InFramePixData[ReversedIterator];
    }
#endif

    //blit directly to image for optimization
    originalImage->imageDataGuard.lock();

    if (originalImage->imageData == nullptr || originalImage->imageSize != frame.size())
    {
        delete[] originalImage->imageData;
        originalImage->imageData = new uchar[allBytes];
    }

    int allPixelsCount = allBytes / 4;
    quint32* InFramePixData = (quint32*)mutableFrame.bits();
    quint32* OutFramePixData = (quint32*)originalImage->imageData;

    for (int pixelID = 0; pixelID < allPixelsCount; ++pixelID)
    {
        int ReversedIterator = allPixelsCount - pixelID;
        OutFramePixData[pixelID] = InFramePixData[ReversedIterator];
    }

    originalImage->imageBytesCount = allBytes;
    originalImage->imageSize = frame.size();

    originalImage->imageDataGuard.unlock();

    //originalImage->GiveImage(OutFrameData, allBytes, frame.size());

    mutableFrame.unmap();
}
