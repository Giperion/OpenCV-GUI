//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#include "imageprocessorthread.h"
#include "playgroundapp.h"
#include "opencv2/core.hpp"

ImageProcessorThread::ImageProcessorThread(QObject* parent)
    : QThread(parent)
{
    //And this is why I actually create a separate object for thread. This call will give me a thread name in the debugger.
    setObjectName("ImageProcessorThread");
}

void ImageProcessorThread::run()
{
    try
    {
        do
        {
            gApp.ProcessLoop();

            while (gApp.lifetimeController == 2)
            {
                QThread::msleep(50);
            }
        } while (gApp.lifetimeController != 0);
    }
    catch (cv::Exception& e)
    {
        haveMessage(e.what());
    }
}
