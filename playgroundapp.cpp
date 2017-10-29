//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#include "playgroundapp.h"
#include "opencv2/opencv.hpp"
#include <QThread>
#include <QPainter>
#include <QFont>
#include <QApplication>

void _drawTxt(cv::Point &CenterRozha, cv::Rect &Rozha, cv::Mat &imageCVRGB);

bool operator==(const PlaygroundImage& left, const PlaygroundImage& right)
{
    return left.name == right.name;
}

PlaygroundApp gApp;

PlaygroundApp::PlaygroundApp()
{
    currentCameraIndx = 0;
    camera = nullptr;
    imageSurface = nullptr;
    lifetimeController = 0;
}

void Sharpen(const cv::Mat& myImage, cv::Mat& Result)
{
    CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images

    Result.create(myImage.size(), myImage.type());
    const int nChannels = myImage.channels();

    for (int j = 1; j < myImage.rows - 1; ++j)
    {
        const uchar* previous = myImage.ptr<uchar>(j - 1);
        const uchar* current = myImage.ptr<uchar>(j);
        const uchar* next = myImage.ptr<uchar>(j + 1);

        uchar* output = Result.ptr<uchar>(j);

        for (int i = nChannels; i < nChannels * (myImage.cols - 1); ++i)
        {
            *output++ = cv::saturate_cast<uchar>(
                5 * current[i]
                - current[i - nChannels]
                - current[i + nChannels]
                - previous[i] 
                - next[i]);
        }
    }

    Result.row(0).setTo(cv::Scalar(0));
    Result.row(Result.rows - 1).setTo(cv::Scalar(0));
    Result.col(0).setTo(cv::Scalar(0));
    Result.col(Result.cols - 1).setTo(cv::Scalar(0));
}

void Blur_test(const cv::Mat& myImage, cv::Mat& Result)
{
    CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images

    Result.create(myImage.size(), myImage.type());

    for (int iter = 0; iter < 100; iter++)
    {
        for (int j = 1; j < myImage.rows - 1; ++j)
        {
            const int* previous = myImage.ptr<int>(j - 1);
            const int* current = myImage.ptr<int>(j);
            const int* next = myImage.ptr<int>(j + 1);

            int* output = Result.ptr<int>(j);

            for (int i = 1; i < myImage.cols - 1; ++i)
            {
                //int mixture2 = (previous[i] + current[i] + next[i]);
                ushort pix1 = (current[i] & 0x000000FF);
                ushort pix2 = (current[i] & 0x0000FF00) >> 8;
                ushort pix3 = (current[i] & 0x00FF0000) >> 16;
                ushort pix4 = (current[i] & 0xFF000000) >> 24;

                pix1 += (previous[i] & 0x000000FF);
                pix2 += (previous[i] & 0x0000FF00) >> 8;
                pix3 += (previous[i] & 0x00FF0000) >> 16;
                pix4 += (previous[i] & 0xFF000000) >> 24;

                pix1 += (next[i] & 0x000000FF);
                pix2 += (next[i] & 0x0000FF00) >> 8;
                pix3 += (next[i] & 0x00FF0000) >> 16;
                pix4 += (next[i] & 0xFF000000) >> 24;

                pix1 /= 3; pix2 /= 3; pix3 /= 3; pix4 /= 3;
                int finalMix = pix4 << 24 | pix3 << 16 | pix2 << 8 | pix1;
                *output++ = cv::saturate_cast<int>(finalMix);
            }
        }
    }

    Result.row(0).setTo(cv::Scalar(0));
    Result.row(Result.rows - 1).setTo(cv::Scalar(0));
    Result.col(0).setTo(cv::Scalar(0));
    Result.col(Result.cols - 1).setTo(cv::Scalar(0));
}

void subtractTest(const cv::Mat& myImage, const cv::Mat& previousImage, cv::Mat& Result)
{
    Result.create(myImage.size(), myImage.type());
    if (previousImage.empty()) return;

    const uchar* currentImgPix = myImage.ptr<uchar>(0, 0);
    const uchar* previousImgPix = previousImage.ptr<uchar>(0, 0);

    uchar* outputImgPix = Result.ptr<uchar>(0, 0);

    int TotalNumOfBytes = myImage.rows * myImage.cols * 4;
    for (int pixelID = 0; pixelID < TotalNumOfBytes; pixelID++)
    {
        const uchar& currentVal = currentImgPix[pixelID];
        const uchar& previousVal = previousImgPix[pixelID];

        uchar MaxValue = 0;
        uchar MinValue = 0;
        if (currentVal > previousVal)
        {
            MaxValue = currentVal;
            MinValue = previousVal;
        }
        else
        {
            MaxValue = previousVal;
            MinValue = currentVal;
        }

        uchar Final = MaxValue - MinValue;

        outputImgPix[pixelID] = Final;
    }

}


void PlaygroundApp::ProcessLoop()
{
    const QString previousFrameName = "CV Output";

#ifdef USE_OPENCV_WINDOW
    const cv::String TestWindowName = "TestCV";
    cv::namedWindow(TestWindowName, cv::WINDOW_AUTOSIZE);
#endif

    //for face recognization test
    cv::CascadeClassifier rozhaClassifier;
    cv::CascadeClassifier eyesClassifier;

    eyesClassifier.load("F:\\[EUREKA]\\OpenCV\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml");
    rozhaClassifier.load("F:\\[EUREKA]\\OpenCV\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt2.xml");

    //Get ptr to original image
    int hOrigImg = GetImageIndexByName("Original");
    PlaygroundImage* OriginalImg = GetImageByIndex(hOrigImg);
    Q_ASSERT(OriginalImg);

    cv::Mat previousFrame;
    //register previousFace as one of the avaliable frame outputs
    PlaygroundImage* hPreviousFrame = RegisterImage(previousFrameName);

    while (gApp.lifetimeController == 1)
    {
        //take image
        uchar* imagePtr = nullptr;
        QSize imageSize;
        quint64 imageBytesCount;

        OriginalImg->TakeImage(imagePtr, imageBytesCount, imageSize);

        //image not avaliable yet, try again
        if (imagePtr == nullptr) continue;

        //FINALLY!
        //OPENCV TIME!
        cv::Mat imageCVRGBX;
        cv::Mat imageCVRGB;
        imageCVRGBX.create(imageSize.height(), imageSize.width(), CV_8UC4);
        imageCVRGB.create(imageSize.height(), imageSize.width(), CV_8UC3);
        memcpy(imageCVRGBX.ptr(0, 0), imagePtr, imageBytesCount);

        //create RGB image
        uchar* RGBImgPtr = imageCVRGB.ptr(0, 0);
        for (uchar* imgIter = imagePtr; imgIter != (imagePtr + imageBytesCount); imgIter += 4)
        {
            *RGBImgPtr++ = imgIter[0];
            *RGBImgPtr++ = imgIter[1];
            *RGBImgPtr++ = imgIter[2];
        }

        //imageProcessingTests(imageCVRGB);

        cv::Mat grayImg;
        cv::cvtColor(imageCVRGBX, grayImg, cv::COLOR_RGBA2GRAY);
        cv::equalizeHist(grayImg, grayImg);

        std::vector<cv::Rect> Rozhas;

        rozhaClassifier.detectMultiScale(grayImg, Rozhas, 1.1, 2, cv::CASCADE_SCALE_IMAGE, cv::Size(20, 20));
        //eyesClassifier.detectMultiScale(grayImg, Rozhas, 1.1, 2, cv::CASCADE_SCALE_IMAGE, cv::Size(20, 20));

        //draw result
        for (int i = 0; i < Rozhas.size(); ++i)
        {
            cv::Rect& Rozha = Rozhas[i];
            cv::Point CenterRozha(Rozha.x + Rozha.width / 2, Rozha.y + Rozha.height / 2);

            cv::ellipse(imageCVRGB, CenterRozha, cv::Size(Rozha.width / 2, Rozha.height / 2), 0.0, 0.0, 360.0, cv::Scalar(0, 0, 255), 3);

            if (i == 0)
            {
                _drawTxt(CenterRozha, Rozha, imageCVRGB);
            }

        }

#ifdef USE_OPENCV_WINDOW
        cv::imshow(TestWindowName, imageCVRGB);
        cv::waitKey(2);
#else
        QThread::msleep(2);
#endif
        
        //QThread::msleep(2);
        imageCVRGBX.copyTo(previousFrame);

        //i'm lazy ass, i know
        cv::Mat JustConvertTo32BitImg;
        //JustConvertTo32BitImg.create(imageCVRGB.rows, imageCVRGB.cols, CV_8UC4);
        cv::cvtColor(imageCVRGB, JustConvertTo32BitImg, cv::COLOR_RGB2RGBA);

        int OutputFrameByteCount = JustConvertTo32BitImg.cols * JustConvertTo32BitImg.rows * 4;
        hPreviousFrame->GiveImage(JustConvertTo32BitImg.ptr(0, 0), OutputFrameByteCount, QSize(JustConvertTo32BitImg.cols, JustConvertTo32BitImg.rows));

        //delete image after using
        delete[] imagePtr;

        //process Qt events
        QApplication::processEvents();
    }

    gApp.UnregisterImage(previousFrameName);

#ifdef USE_OPENCV_WINDOW
    cv::destroyWindow(TestWindowName);
#endif

    OnOpenCVThreadShutdowned();
}

void _drawTxt(cv::Point &CenterRozha, cv::Rect &Rozha, cv::Mat &imageCVRGB)
{
    //draw text if a first face (demo)
    QImage textImg(300, 100, QImage::Format::Format_RGB888);
    textImg.fill(QColor(255, 255, 255));

    {
        QPainter qtPainter(&textImg);
        //qtPainter.fillRect(0, 0, 300, 100, QColor(0, 255, 0));
        qtPainter.setFont(QFont("Arial", 24));
        qtPainter.drawText(5, 30, "Nasty human");
    }

    //perform super effective direct blitting :)
    uchar* textImgPtr = textImg.bits();
    int textRowIter = 0;
    int bytesPerRow = textImg.width() * 3;
    int StartRow = (CenterRozha.y - Rozha.height / 2) - 45;

    for (int RowNum = StartRow; RowNum < (StartRow + textImg.height()); ++RowNum)
    {
        if (RowNum < 0 || RowNum >= imageCVRGB.rows) continue;
        uchar* RowPtr = imageCVRGB.ptr<uchar>(RowNum);
        RowPtr += (CenterRozha.x - Rozha.width / 4) * 3;

        uchar* textRowPtr = textImgPtr + (bytesPerRow * textRowIter++);

        for (int x = 0; x < bytesPerRow; x += 3)
        {
            if (textRowPtr[x] < 80)
            {
                RowPtr[x] = 0;
                RowPtr[x + 1] = 0;
                RowPtr[x + 2] = 255;
            }
        }
        //memcpy(RowPtr, textImgPtr + (bytesPerRow * textRowIter++), bytesPerRow);
    }
}

void PlaygroundApp::imageProcessingTests(cv::Mat imageCVRGB)
{
    //         cv::Mat grayImage;
    //         cv::cvtColor(imageCV, grayImage, cv::COLOR_RGBA2GRAY);

    cv::Mat sharpedImg;
    ////Blur_test(imageCV, sharpenImg);
    ////cv::blur(imageCV, blurredImg, cv::Size(20, 20));
    //cv::bilateralFilter(imageCVRGB, sharpedImg, -1, 30.0, 15.0);

    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
        cv::Size(2 * 5 + 1, 2 * 5 + 1),
        cv::Point(5, 5));

    //cv::dilate(imageCVRGB, sharpedImg, element, cv::Point(-1, -1), 1);
    cv::morphologyEx(imageCVRGB, sharpedImg, cv::MORPH_HITMISS, element, cv::Point(-1, -1), 1);

    cv::imshow("TestCV", sharpedImg);
    cv::waitKey(10);
}

PlaygroundImage* PlaygroundApp::RegisterImage(QString name)
{
    int ExistanceCheck = GetImageIndexByName(name);
    //Image already registering
    Q_ASSERT(ExistanceCheck == -1);

    int NextKey = 0;
    if (!sharedImages.isEmpty())
    {
        NextKey = sharedImages.lastKey(); ++NextKey;
    }

    PlaygroundImage newImage;
    newImage.name = name;
    auto NewImageRecordIter = sharedImages.insert(NextKey, newImage);

    OnImageRegistered(name);
    return &NewImageRecordIter.value();
}

void PlaygroundApp::UnregisterImage(QString name)
{
    int ExistanceCheck = GetImageIndexByName(name);
    //Image not registered
    Q_ASSERT(ExistanceCheck != -1);
    PlaygroundImage* image = GetImageByIndex(ExistanceCheck);

    delete[] image->imageData;

    sharedImages.remove(ExistanceCheck);
    OnImageUnRegistered(name);
}

int PlaygroundApp::GetImageIndexByName(QString imageName)
{
    for (const PlaygroundImage& image : sharedImages)
    {
        if (imageName == image.name)
        {
            return sharedImages.key(image, -1);
        }
    }

    return -1;
}

PlaygroundImage* PlaygroundApp::GetImageByIndex(int Index)
{
    auto imgIter = sharedImages.find(Index);
    if (imgIter != sharedImages.end())
    {
        return &imgIter.value();
    }

    return nullptr;
}


PlaygroundImage& PlaygroundImage::operator=(const PlaygroundImage& other)
{
    name = other.name;
    imageData = other.imageData;
    imageBytesCount = other.imageBytesCount;
    imageSize = other.imageSize;
    return *this;
}

PlaygroundImage::PlaygroundImage(const PlaygroundImage& other)
{
    name = other.name;
    imageData = other.imageData;
    imageBytesCount = other.imageBytesCount;
    imageSize = other.imageSize;
}

void PlaygroundImage::GiveImage(uchar* inImageData, quint64 bytesCount, QSize& inImageSize)
{
    QMutexLocker locker(&imageDataGuard);

    if (imageData == nullptr || imageSize != inImageSize)
    {
        delete[] imageData;
        imageData = new uchar[bytesCount];
    }

    memcpy(imageData, inImageData, bytesCount);
    imageBytesCount = bytesCount;
    imageSize = inImageSize;
}

void PlaygroundImage::TakeImage(uchar*& outImageData, quint64& outBytesCount, QSize& outImageSize)
{
    QMutexLocker locker(&imageDataGuard);
    if (imageData == nullptr || imageBytesCount < 1) return;

    outImageData = new uchar[imageBytesCount];
    memcpy(outImageData, imageData, imageBytesCount);
    outImageSize = imageSize;
    outBytesCount = imageBytesCount;
}
