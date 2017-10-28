#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "androidfiledialog.h"

#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <jni.h>

#include <qpa/qplatformnativeinterface.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    oscWidget(nullptr),
    ui(new Ui::MainWindow)
{
    QString mpvConfigDir = configDir();
    QDir().mkpath(mpvConfigDir);
    QStringList fileNeedCopy;

    fileNeedCopy << "cacert.pem" << "subfont.ttf" << "stats.lua";
    foreach(QString fileName, fileNeedCopy) {
        QFile file("assets:/" + fileName);
        if (!QFileInfo(mpvConfigDir + fileName).isFile()) {
            file.copy(QString(mpvConfigDir + fileName));
        }
    }

    ui->setupUi(this);
    this->setupOscUi(this);

    m_mpv = ui->openGLWidget;

    connect(this->oscWidget->progressSlider, SIGNAL(sliderReleased()), SLOT(setSeekPosition()));
    connect(this->oscWidget->progressSlider, SIGNAL(sliderPressed()), SLOT(setSliderNoResponFromMpv()));
    connect(this->oscWidget->openFileButton, SIGNAL(clicked()), SLOT(openMedia()));
    connect(this->oscWidget->playButton, SIGNAL(clicked()), SLOT(pauseResume()));
    connect(this->oscWidget->statButton, SIGNAL(clicked()), SLOT(switchStatScript()));
    connect(m_mpv, SIGNAL(positionChanged(double)), this, SLOT(setPosition(double)));
    connect(m_mpv, SIGNAL(durationChanged(double)), this, SLOT(setSliderRange(double)));

    //Rotation
    QScreen *s = QGuiApplication::primaryScreen();
    connect(s, SIGNAL(orientationChanged(Qt::ScreenOrientation)),
        this, SLOT(orientationChanged(Qt::ScreenOrientation)));

    s->setOrientationUpdateMask(
        Qt::PortraitOrientation
        | Qt::LandscapeOrientation
        | Qt::InvertedPortraitOrientation
        | Qt::InvertedLandscapeOrientation);

    QTimer::singleShot(50, this, SLOT(checkIntentContent()));

    m_mpv->command(QVariantList() << "load-script" << configDir() + "stats.lua");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::configDir()
{
    return "/storage/emulated/0/mpv/";
}

void MainWindow::setupOscUi(QWidget* wparent)
{
    int leftRightPadding = 20;
    int bottomPadding = 160;
    int width = this->width();
    int height = this->height();

    // osc
    if(oscWidget == nullptr)
    {
        oscWidget = new OscWidget(wparent);
    }
    if(wparent->size() == oscWidget->currentParentSize)
    {
        return;
    }

    oscWidget->currentParentSize = wparent->size();
    oscWidget->resize(width-2 * leftRightPadding, 220);
    oscWidget->move(leftRightPadding, height - oscWidget->height() - bottomPadding);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    this->setupOscUi(this);
}

void MainWindow::checkIntentContent()
{
    //handle intent
    QString fileFromIntent = QAndroidJniObject::callStaticObjectMethod("com.harry.mpv.QtFullscreenActivity"
                                                                                  , "getUrl"
                                                                                  , "()Ljava/lang/String;")
                .toString();

    qDebug() << "open file:" << fileFromIntent;
    if (!fileFromIntent.isEmpty()) {
        openFileNameReady(fileFromIntent);
    }

    //init surface
    QPlatformNativeInterface *nativeInterfaceAPP = QApplication::platformNativeInterface();
    jobject activity = (jobject)(nativeInterfaceAPP->nativeResourceForIntegration("QtActivity"));
    QAndroidJniEnvironment * qjniEnv;
    JNIEnv * jniEnv;
    JavaVM * jvm = qjniEnv->javaVM();
    jvm->GetEnv(reinterpret_cast<void**>(&qjniEnv), JNI_VERSION_1_6);
    jvm->AttachCurrentThread(&jniEnv,NULL);

    jint r_id_content = QAndroidJniObject::getStaticField<jint>("android/R$id", "content");

    QAndroidJniObject view = ((QAndroidJniObject) activity).callObjectMethod("findViewById", "(I)Landroid/view/View;", r_id_content);
    if (view.isValid()) {
        QAndroidJniObject child1 = view.callObjectMethod("getChildAt", "(I)Landroid/view/View;", 0);
        QAndroidJniObject child2 = child1.callObjectMethod("getChildAt", "(I)Landroid/view/View;", 0);
        if (child2.isValid()) {
            QAndroidJniObject sHolder = child2.callObjectMethod("getHolder","()Landroid/view/SurfaceHolder;");
            if (sHolder.isValid()) {
                QAndroidJniObject theSurface = sHolder.callObjectMethod("getSurface","()Landroid/view/Surface;");
                if (theSurface.isValid()) {
                    ANativeWindow* awindow = ANativeWindow_fromSurface(jniEnv, theSurface.object());
                    qDebug() << "This is a ANativeWindow " << awindow;
                    WId wid = winId();
                    qDebug() << "winid returned " << wid;

                    // based on https://github.com/mpv-android/mpv-android/pull/53
                    jobject surface_ = theSurface.object<jobject>();
                    // surface = env->NewGlobalRef(surface_);
                    int64_t Wid = (int64_t)(intptr_t) (jniEnv->NewGlobalRef(surface_));
                    qDebug() << "Surface WID:" << Wid;
                }
            }
        } else {
            qDebug() << "Views are not loaded yet or you are not in the Qt UI Thread";
        }
    }

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

void MainWindow::switchSub()
{
    m_mpv->command(QVariantList() << "cycle" << "sub");   
}

void MainWindow::pauseResume()
{
    const bool paused = m_mpv->getProperty("pause").toBool();
    m_mpv->setProperty("pause", !paused);
}

void MainWindow::setPosition(double position)
{
    position = position * 10000;
    this->oscWidget->progressSlider->setValue((int)position);
}

void MainWindow::setSliderRange(double duration)
{
    duration = duration * 10000;
    this->oscWidget->progressSlider->setRange(0, (int)duration);
}

void MainWindow::setSliderNoResponFromMpv()
{
    disconnect(m_mpv, SIGNAL(positionChanged(double)), this, SLOT(setPosition(double)));
}

void MainWindow::setSeekPosition()
{
//    bool paused = false;
//    paused = m_mpv->getProperty("pause").toBool();
//    m_mpv->setProperty("pause", !paused);

    double adjustTime = this->oscWidget->progressSlider->value()/10000.0 - m_mpv->currentTime;
    m_mpv->command(QVariantList() << "seek" << adjustTime);

//    paused = m_mpv->getProperty("pause").toBool();
//    m_mpv->setProperty("pause", !paused);

    // restore signal
    connect(m_mpv, SIGNAL(positionChanged(double)), this, SLOT(setPosition(double)));
}

void MainWindow::orientationChanged(Qt::ScreenOrientation orientation)
{
    qDebug("orientationChanged");
//    if(orientation == Qt::LandscapeOrientation || orientation == Qt::InvertedLandscapeOrientation)
//    {
//        ui->videoSlider->setVisible(false);
//        ui->pushButton->setVisible(false);
//        ui->pushButton2->setVisible(false);
//        ui->pushButton3->setVisible(false);
//        ui->horizontalLayout->setSpacing(0);
//    }
//    else
//    {
//        ui->videoSlider->setVisible(true);
//        ui->pushButton->setVisible(true);
//        ui->pushButton2->setVisible(true);
//        ui->pushButton3->setVisible(true);
//        ui->horizontalLayout->setSpacing(7);
//    }
}

void MainWindow::changeOrientation()
{
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();
    jint orient = activity.callMethod<jint>( "getRequestedOrientation" );
    if(env->ExceptionCheck())
    {
        qDebug() << "exception occured";
        env->ExceptionClear();
    }

    orient = !orient;
    activity.callMethod<void>("setRequestedOrientation", "(I)V", orient);
    if(env->ExceptionCheck())
    {
        qDebug() << "exception occured";
        env->ExceptionClear();
    }
}

void MainWindow::switchStatScript()
{
    m_mpv->command(QVariantList() << "script-message" << "togglestats");
    //m_mpv->command(QVariantList() << "load-script" << configDir() + "stats.lua");
}

// animation/gesture actions

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);

    this->oscWidget->triggerVisiableAnim();
    qDebug() << "triggerVisiableAnim";
}
