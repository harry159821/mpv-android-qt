#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "androidfiledialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString mpvConfigDir = "/storage/emulated/0/mpv/";
    QDir().mkpath(mpvConfigDir);

    QFile file("assets:/cacert.pem");
    if (!QFileInfo(mpvConfigDir + "/cacert.pem").isFile()) {
        file.copy(QString(mpvConfigDir + "/cacert.pem"));
    }
    QFile subFile("assets:/subfont.ttf");
    if (!QFileInfo(mpvConfigDir + "/subfont.ttf").isFile()) {
        subFile.copy(QString(mpvConfigDir + "/subfont.ttf"));
    }

    ui->setupUi(this);

    m_mpv = ui->openGLWidget;

    connect(ui->videoSlider, SIGNAL(sliderMoved(int)), SLOT(seek(int)));
    connect(ui->pushButton, SIGNAL(clicked()), SLOT(openMedia()));
    connect(ui->pushButton2, SIGNAL(clicked()), SLOT(pauseResume()));
    connect(ui->pushButton2, SIGNAL(clicked()), SLOT(tiggleSub()));
    connect(m_mpv, SIGNAL(positionChanged(int)), ui->videoSlider, SLOT(setValue(int)));
    connect(m_mpv, SIGNAL(durationChanged(int)), this, SLOT(setSliderRange(int)));

    //Rotation
    QScreen *s = QGuiApplication::primaryScreen();
    connect(s, SIGNAL(orientationChanged(Qt::ScreenOrientation)),
        this, SLOT(orientationChanged(Qt::ScreenOrientation)));

    s->setOrientationUpdateMask(
        Qt::PortraitOrientation
        | Qt::LandscapeOrientation
        | Qt::InvertedPortraitOrientation
        | Qt::InvertedLandscapeOrientation);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openMedia()
{
    //QString file = QFileDialog::getOpenFileName(0, "Open a video");
    //if (file.isEmpty())
    //    return;

    AndroidFileDialog *fileDialog = new AndroidFileDialog();
    connect(fileDialog, SIGNAL(existingFileNameReady(QString)), this, SLOT(openFileNameReady(QString)));
    bool success = fileDialog->provideExistingFileName();
    if (!success) {
        qDebug() << "Problem with JNI or sth like that...";
        disconnect(fileDialog, SIGNAL(existingFileNameReady(QString)), this, SLOT(openFileNameReady(QString)));
        //or just delete fileDialog instead of disconnect
    }
}

void MainWindow::openFileNameReady(QString fileName)
{
    if (!fileName.isNull()) {
        qDebug() << "FileName: " << fileName;
        m_mpv->command(QStringList() << "loadfile" << fileName);
    } else {
        qDebug() << "User did not choose file";
    }
}

void MainWindow::seek(int pos)
{
    m_mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void MainWindow::tiggleSub()
{
    m_mpv->command(QVariantList() << "cycle" << "sub");   
}

void MainWindow::pauseResume()
{
    const bool paused = m_mpv->getProperty("pause").toBool();
    m_mpv->setProperty("pause", !paused);
}

void MainWindow::setSliderRange(int duration)
{
    ui->videoSlider->setRange(0, duration);
}

void MainWindow::orientationChanged(Qt::ScreenOrientation orientation)
{
    if(orientation == Qt::LandscapeOrientation || orientation == Qt::InvertedLandscapeOrientation)
    {
        ui->videoSlider->setVisible(false);
        ui->pushButton->setVisible(false);
        ui->pushButton2->setVisible(false);
        ui->horizontalLayout->setSpacing(0);
    }
    else
    {
        ui->videoSlider->setVisible(true);
        ui->pushButton->setVisible(true);
        ui->pushButton2->setVisible(true);
        ui->horizontalLayout->setSpacing(7);
    }
}
