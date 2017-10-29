//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#pragma once
#include <QLabel>

class VideoOutputWidget : public QLabel
{
    Q_OBJECT
public:
    VideoOutputWidget(QWidget *parent = Q_NULLPTR);

    void setCaptureMode(bool bCaptureMode);
    void setSampleSize(int sampleWidth, int sampleHeigth);

signals:
    void mouseReleased(QRect selectedRegion);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    bool captureMode;
private:

    bool mouseHolded;

    int mousePosX;
    int mousePosY;

    int sampleSizeX;
    int sampleSizeY;
};
