#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QtGui>
#include <QMainWindow>
#include <QWidget>
#include <QtCore>
#include <videosurface.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    VideoSurface *m_mpv;

    QString configDir();

public Q_SLOTS:
    void openMedia();
    void openFileNameReady(QString);
    void seek(int);
    void setSliderRange(int);
    
    void pauseResume();
    void switchSub();
    void switchStatScript();

    void orientationChanged(Qt::ScreenOrientation orientation);
    void changeOrientation();

    void checkIntentContent();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
