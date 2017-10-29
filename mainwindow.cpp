//Giperion 2017 September
//[EUREKA] 3.3
//OpenCV Playground
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCameraInfo>
#include <QCamera>
#include <QImage>
#include <QMessageBox>
#include <QFileDialog>
#include <QRect>
#include <QDir>
#include <QIcon>
#include "playgroundvideosurface.h"
#include "playgroundapp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    sampleWidth(20), sampleHeight(20),
    videoSourceIndex(0), imageIterator(0)
{
    mode = AM_VIEW;

    const QString settingsFileName = QStringLiteral ("OpenCVSettings.ini");

    settingsInit:
    if (QFile::exists(settingsFileName))
    {
        pSettings = new QSettings(settingsFileName, QSettings::IniFormat, this);
        pSettings->sync();
        if (pSettings->status() == QSettings::FormatError)
        {
            QMessageBox::warning(this, "Error", "Application settings contains errors. The settings will be recreated. If you want - you can backup them right now.", QMessageBox::Ok, QMessageBox::NoButton);
            delete pSettings;
            QFile::remove(settingsFileName);
            goto settingsInit;
        }
    }
    else
    {
        //write default settings
        pSettings = new QSettings(settingsFileName, QSettings::IniFormat, this);
        pSettings->setValue("AppMode", "View");
        pSettings->setValue("SampleSaveFolder", QDir::tempPath());
        pSettings->setValue("SampleWidth", "20");
        pSettings->setValue("SampleHeight", "20");
        pSettings->setValue("CameraName", "");

        pSettings->sync();
    }

    ui->setupUi(this);

    QIcon playgroundIcon(":icons/OpenCV-Playground-Logo-256px.ico");
    setWindowIcon(playgroundIcon);

    ui->appModeComboBox->addItem("View");
    ui->appModeComboBox->addItem("Capture");
    ui->appModeComboBox->setCurrentIndex(0);
    connect(ui->appModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnAppModeChanges(int)));
    QString savedAppMode = pSettings->value("AppMode").toString();
    if (savedAppMode == "Capture")
    {
        ui->appModeComboBox->setCurrentIndex(1);
    }

    ui->sampleSizeComboBox->addItem("10x10");
    ui->sampleSizeComboBox->addItem("20x20");
    ui->sampleSizeComboBox->addItem("40x40");
    ui->sampleSizeComboBox->addItem("80x80");
    ui->sampleSizeComboBox->addItem("Custom");
    ui->sampleSizeComboBox->setCurrentIndex(1); //20x20
    connect(ui->sampleSizeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSampleSizeChanges(int)));
    bool castOk = false;
    qint32 savedSampleWidth = pSettings->value("SampleWidth").toInt(&castOk);
    if (!castOk)
    {
        savedSampleWidth = 20;
    }
    qint32 savedSampleHeight = pSettings->value("SampleHeight").toInt(&castOk);
    if (!castOk)
    {
        savedSampleHeight = 20;
    }
    SetSampleWidthHeight(savedSampleWidth, savedSampleHeight);
    if (sampleWidth != sampleHeight)
    {
        ui->sampleSizeComboBox->setCurrentIndex(4); //Custom
    }
    else
    {
        //we save to check only width
        if (sampleWidth == 10)
        {
            ui->sampleSizeComboBox->setCurrentIndex(0); //10x10
        }
        if (sampleWidth == 20)
        {
            ui->sampleSizeComboBox->setCurrentIndex(1); //20x20
        }
        if (sampleWidth == 40)
        {
            ui->sampleSizeComboBox->setCurrentIndex(2); //40x40
        }
        if (sampleWidth == 80)
        {
            ui->sampleSizeComboBox->setCurrentIndex(3); //80x80
        }
    }

    QString savedSampleFilePath = pSettings->value("SampleSaveFolder").toString();
    ui->filePathTxtBox->setPlainText(savedSampleFilePath);
    connect(ui->filePathTxtBox, SIGNAL(textChanged()), this, SLOT(OnSampleFilePathTxtChanged()));

    ui->videoSourceComboBox->addItem("Original");
    ui->videoSourceComboBox->setCurrentIndex(0);
    connect(ui->videoSourceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnVideoOutputChanged(int)));

    connect(ui->filePathDialogBtn, SIGNAL(released()), this, SLOT(OnFilePathDialogBtn()));
    
    connect(ui->sampleWidthTxt, SIGNAL(textChanged()), this, SLOT(OnSampleWidthTxtChanged()));
    connect(ui->sampleHeightTxt, SIGNAL(textChanged()), this, SLOT(OnSampleHeightTxtChanged()));

    connect(&gApp, SIGNAL(OnImageRegistered(QString)), this, SLOT(OnImageRegistered(QString)));
    connect(&gApp, SIGNAL(OnImageUnRegistered(QString)), this, SLOT(OnImageUnRegistered(QString)));

    connect(ui->videoLabel, SIGNAL(mouseReleased(QRect)), this, SLOT(OnRegionSelected(QRect)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete pSettings;
}

bool MainWindow::Initialize()
{
    //check camera
    if (QCameraInfo::availableCameras().count() == 0)
    {
        QMessageBox::critical(this, QStringLiteral("No Camera"), QStringLiteral("This application require camera attached to PC. Application will now close."), QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }

    QComboBox* cameraSelector = ui->cameraComboBox;
    QList<QCameraInfo> cameraInfos = QCameraInfo::availableCameras();
    QString SavedDesiredCamera = pSettings->value("CameraName").toString();
    for (QCameraInfo& info : cameraInfos)
    {
        QString deviceDesc = info.description();
        cameraSelector->addItem(deviceDesc);
        if (deviceDesc == SavedDesiredCamera)
        {
            gApp.currentCameraIndx = cameraSelector->count() - 1;
        }
    }

    //set default camera
    connect(ui->cameraComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCameraIndexChanges(int)));

    int CurrentCamUIIndex = cameraSelector->currentIndex();
    if (CurrentCamUIIndex != gApp.currentCameraIndx)
    {
        cameraSelector->setCurrentIndex(gApp.currentCameraIndx);
    }
    else
    {
        OnCameraIndexChanges(gApp.currentCameraIndx);
    }

    return true;
}

void MainWindow::OnCVThreadMessage(QString message)
{
    QMessageBox::warning(this, "CV Error", message, QMessageBox::Ok, QMessageBox::NoButton);
}

void MainWindow::OnCameraIndexChanges(int Index)
{
    //stop OpenCV thread
    //a) subscribe to shutdown event
    gApp.currentCameraIndx = Index;

    if (gApp.lifetimeController == 0)
    {
        OnChangingCamera();
    }
    else
    {
        connect(&gApp, SIGNAL(OnOpenCVThreadShutdowned()), this, SLOT(OnChangingCamera()));
        gApp.lifetimeController = 2;
    }
}

void MainWindow::OnChangingCamera()
{
    if (gApp.camera != nullptr)
    {
        gApp.camera->stop();
        delete gApp.camera;
        gApp.camera = nullptr;
    }

    if (gApp.imageSurface != nullptr)
    {
        disconnect(gApp.imageSurface, SIGNAL(update()), this, SLOT(FrameUpdate()));
        delete gApp.imageSurface;
    }

    QList<QCameraInfo> cameraInfos = QCameraInfo::availableCameras();
    QCameraInfo& targetCamera = cameraInfos[gApp.currentCameraIndx];
    pSettings->setValue("CameraName", targetCamera.description());

    gApp.camera = new QCamera(targetCamera, this);

    gApp.imageSurface = new PlaygroundVideoSurface();
    connect(gApp.imageSurface, SIGNAL(update()), this, SLOT(FrameUpdate()));

    gApp.camera->setViewfinder(gApp.imageSurface);
    gApp.camera->setCaptureMode(QCamera::CaptureStillImage);

    gApp.camera->start();
    gApp.lifetimeController = 1;

    disconnect(&gApp, SIGNAL(OnOpenCVThreadShutdowned()), this, SLOT(OnChangingCamera()));
}

void MainWindow::OnAppModeChanges(int Index)
{
    AppMode newMode = (AppMode)Index;

    if (newMode == mode) return;

    switch (newMode)
    {
    case MainWindow::AM_VIEW:
        ui->sampleCaptureGroup->setEnabled(false);
        ui->videoLabel->setCaptureMode(false);
        pSettings->setValue("AppMode", "View");
        break;
    case MainWindow::AM_CAPTURE:
        ui->sampleCaptureGroup->setEnabled(true);
        ui->videoLabel->setCaptureMode(true);
        pSettings->setValue("AppMode", "Capture");
        break;
    default:
        Q_ASSERT(false && "You specified unknown app mode index");
        break;
    }
    mode = newMode;
}

void MainWindow::OnSampleSizeChanges(int Index)
{
    SampleSize sizeEnum = (SampleSize)Index;

    switch (sizeEnum)
    {
    case MainWindow::x10:
        sampleWidth = 10;
        sampleHeight = 10;
        EnableCustomSampleSizeControls(false);
        break;
    case MainWindow::x20:
        sampleWidth = 20;
        sampleHeight = 20;
        EnableCustomSampleSizeControls(false);
        break;
    case MainWindow::x40:
        sampleWidth = 40;
        sampleHeight = 40;
        EnableCustomSampleSizeControls(false);
        break;
    case MainWindow::x80:
        sampleWidth = 80;
        sampleHeight = 80;
        EnableCustomSampleSizeControls(false);
        break;
    case MainWindow::Custom:
        EnableCustomSampleSizeControls(true);
        break;
    default:
        Q_ASSERT(false && "Specified unknown sample size index");
        break;
    }

    SetSampleWidthHeight(sampleWidth, sampleHeight);
}

void MainWindow::OnVideoOutputChanged(int Index)
{
    PlaygroundImage* img = gApp.GetImageByIndex(Index);
    Q_ASSERT(img);

    videoSourceIndex = Index;
}

void MainWindow::EnableCustomSampleSizeControls(bool bEnable)
{
    ui->sampleWidthTxt->setEnabled(bEnable);
    ui->sampleHeightTxt->setEnabled(bEnable);
}

void MainWindow::SetSampleWidthHeight(int width, int height)
{
    sampleWidth = width;
    sampleHeight = height;
    ui->sampleWidthTxt->setPlainText(QString::number(width));
    ui->sampleHeightTxt->setPlainText(QString::number(height));
    ui->videoLabel->setSampleSize(sampleWidth, sampleHeight);

    pSettings->setValue("SampleWidth", width);
    pSettings->setValue("SampleHeight", height);
}

QString MainWindow::GetClearedNumbericText(QString InText)
{
    QVector<QChar> newTextContainer;
    newTextContainer.reserve(InText.length());

    for (QChar ch : InText)
    {
        if (ch.isDigit())
        {
            newTextContainer.append(ch);
        }
    }

    return QString(newTextContainer.data(), newTextContainer.size());
}

void MainWindow::OnFilePathDialogBtn()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::ShowDirsOnly);

    if (dialog.exec())
    {
        QStringList selectedFolders = dialog.selectedFiles();
        QString selectedFolder = selectedFolders[0];
        ui->filePathTxtBox->setPlainText(selectedFolder);

        //check folder for existance sample files
        updateImageIteratorBasedOnFilesInDirectory(selectedFolder);
    }
}

void MainWindow::updateImageIteratorBasedOnFilesInDirectory(QString selectedFolder)
{
    QDir folderDir(selectedFolder);

    QStringList allFiles = folderDir.entryList(QDir::Filter::Files);

    for (QString& fileName : allFiles)
    {
        if (fileName.startsWith("sample"))
        {
            QString NumStr = fileName.remove(0, 6);
            NumStr = GetClearedNumbericText(NumStr);
            bool castOk = false;
            int fileImageIterator = NumStr.toInt(&castOk);
            if (castOk && fileImageIterator > imageIterator)
            {
                imageIterator = fileImageIterator;
            }
        }
    }

    //image iterator always must start from 1. Zero is reserved as indicator, that there is no samples files (or 'not initialized')
    if (imageIterator == 0) imageIterator = 1;
    else ++imageIterator; //point to next future file, not to last existant
}

void MainWindow::OnSampleWidthTxtChanged()
{
    QString newText = ui->sampleWidthTxt->toPlainText();
    QString ClearedText = GetClearedNumbericText(newText);

    if (newText != ClearedText)
    {
        disconnect(ui->sampleWidthTxt, SIGNAL(textChanged()), this, SLOT(OnSampleWidthTxtChanged()));
        ui->sampleWidthTxt->setPlainText(ClearedText);
        connect(ui->sampleWidthTxt, SIGNAL(textChanged()), this, SLOT(OnSampleWidthTxtChanged()));
    }

    int newSampleWidth = ClearedText.toInt();
    if (newSampleWidth > 0)
    {
        sampleWidth = newSampleWidth;
    }

    pSettings->setValue("SampleWidth", sampleWidth);
}

void MainWindow::OnSampleHeightTxtChanged()
{
    QString newText = ui->sampleHeightTxt->toPlainText();
    QString ClearedText = GetClearedNumbericText(newText);

    if (newText != ClearedText)
    {
        disconnect(ui->sampleHeightTxt, SIGNAL(textChanged()), this, SLOT(OnSampleHeightTxtChanged()));
        ui->sampleHeightTxt->setPlainText(ClearedText);
        connect(ui->sampleHeightTxt, SIGNAL(textChanged()), this, SLOT(OnSampleHeightTxtChanged()));
    }

    int newSampleHeight = ClearedText.toInt();
    if (newSampleHeight > 0)
    {
        sampleHeight = newSampleHeight;
    }

    pSettings->setValue("SampleHeight", sampleHeight);
}

void MainWindow::OnSampleFilePathTxtChanged()
{
    pSettings->setValue("SampleSaveFolder", ui->filePathTxtBox->toPlainText());
}

void MainWindow::OnImageRegistered(QString imageName)
{
    //#HACK: Don't allow Original to be registered
    if (imageName == QStringLiteral("Original")) return;
    ui->videoSourceComboBox->addItem(imageName);
}

void MainWindow::OnImageUnRegistered(QString imageName)
{
    //#HACK: Don't allow Original to be destroyed
    if (imageName == QStringLiteral("Original")) return;

    int elemIndex = ui->videoSourceComboBox->findText(imageName);
    ui->videoSourceComboBox->removeItem(elemIndex);
}

void MainWindow::FrameUpdate()
{
    PlaygroundImage* outputImg = gApp.GetImageByIndex(videoSourceIndex);

    if (outputImg != nullptr)
    {
        uchar* imgPtr = nullptr;
        quint64 imgByteCount = 0;
        QSize imgSize;

        outputImg->TakeImage(imgPtr, imgByteCount, imgSize);

        //check size, if we have differents, we need reallocate output buffer
        if (gApp.videoOutput->size() != imgSize)
        {
            delete gApp.videoOutput; gApp.videoOutput = nullptr;
            gApp.videoOutput = new QImage(imgSize.width(), imgSize.height(), QImage::Format::Format_RGB32);
        }

        memcpy(gApp.videoOutput->bits(), imgPtr, imgByteCount);

        delete[] imgPtr;
    }
    else
    {
        //fill with gray color, and show 'NO DATA'
        quint64 imgByteCount = gApp.videoOutput->width() * gApp.videoOutput->height() * 4;
        memset(gApp.videoOutput->bits(), 128, imgByteCount);
    }

    ui->videoLabel->setPixmap(QPixmap::fromImage(*gApp.videoOutput));
}

void MainWindow::OnRegionSelected(QRect region)
{
    //so
    //if we have an output folder
    //and we have valid region
    //we capture this region, and save to file

    QString folderPath = ui->filePathTxtBox->toPlainText();

    if (folderPath.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Folder path to store samples is not set!", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    //if we don't initialize image iterator - do it now
    if (imageIterator == 0)
    {
        updateImageIteratorBasedOnFilesInDirectory(folderPath);
    }

    QImage img = gApp.videoOutput->copy(region);
    QString fileName = folderPath + "\\" + "sample" + QString::number(imageIterator++) + ".jpg";
    img.save(fileName);
}
