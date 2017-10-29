//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#include "videooutputwidget.h"
#include <QPainter>
#include <QMouseEvent>

VideoOutputWidget::VideoOutputWidget(QWidget *parent /*= Q_NULLPTR*/)
    : QLabel(parent),
    captureMode(false), mouseHolded(false), mousePosX(0), mousePosY(0), sampleSizeX(20), sampleSizeY(20)
{

}


void VideoOutputWidget::setCaptureMode(bool bCaptureMode)
{
    setMouseTracking(bCaptureMode);
    captureMode = bCaptureMode;
}

void VideoOutputWidget::setSampleSize(int sampleWidth, int sampleHeigth)
{
    sampleSizeX = sampleWidth;
    sampleSizeY = sampleHeigth;
}

void VideoOutputWidget::mousePressEvent(QMouseEvent *event)
{
    mouseHolded = true;
    mousePosX = event->x();
    mousePosY = event->y();
}

void VideoOutputWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouseHolded = false;
    if (captureMode)
    {
        QRect selectedRegion(mousePosX - (sampleSizeX / 2), mousePosY - (sampleSizeY / 2), sampleSizeX, sampleSizeY);
        mouseReleased(selectedRegion);
    }
}

void VideoOutputWidget::paintEvent(QPaintEvent *event)
{
    __super::paintEvent(event);

    //if mouse is holding, draw rectangle where mouse is
    if (mouseHolded && captureMode)
    {
        QPainter painter(this);
        painter.setPen(Qt::blue);
        painter.drawRect(mousePosX - (sampleSizeX / 2), mousePosY - (sampleSizeY / 2), sampleSizeX, sampleSizeY);
    }
}

void VideoOutputWidget::mouseMoveEvent(QMouseEvent *event)
{
    mousePosX = event->x();
    mousePosY = event->y();
}
