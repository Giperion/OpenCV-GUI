//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#pragma once

#include <QMainWindow>
#include <QSettings>

class QImage;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    bool Initialize();

public slots:
    void OnCVThreadMessage(QString message);

private slots:
    void OnCameraIndexChanges(int Index);

    void OnChangingCamera();

    void OnAppModeChanges(int Index);
    void OnSampleSizeChanges(int Index);
    void OnVideoOutputChanged(int Index);

    void OnFilePathDialogBtn();

    void updateImageIteratorBasedOnFilesInDirectory(QString selectedFolder);

    void OnSampleWidthTxtChanged();
    void OnSampleHeightTxtChanged();
    void OnSampleFilePathTxtChanged();

    void OnImageRegistered(QString imageName);
    void OnImageUnRegistered(QString imageName);

    void FrameUpdate();
    void OnRegionSelected(QRect region);
private:

    void EnableCustomSampleSizeControls(bool bEnable);
    void SetSampleWidthHeight(int width, int height);

    QString GetClearedNumbericText(QString InText);

    enum AppMode
    {
        AM_VIEW,
        AM_CAPTURE
    };

    enum SampleSize
    {
        x10,
        x20,
        x40,
        x80,
        Custom
    };

    qint32 imageIterator;

    qint32 sampleWidth;
    qint32 sampleHeight;

    qint32 videoSourceIndex;

    AppMode mode;
    QSettings* pSettings;
    Ui::MainWindow *ui;
};

