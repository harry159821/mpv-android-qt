#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QtGui>
#include <QMainWindow>
#include <QWidget>
#include <QtCore>
#include <videosurface.h>
#include "oscwidget.h"

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

    void setupOscUi(QWidget* wparent);

    // osc
    OscWidget *oscWidget;
    void resizeEvent(QResizeEvent *event);

public Q_SLOTS:
    void openMedia();
    void openFileNameReady(QString);
    void seek(int);

    void setPosition(double);
    void setSliderRange(double);

    void setSliderNoResponFromMpv();
    void setSeekPosition();
    
    void pauseResume();
    void switchSub();
    void switchStatScript();

    void orientationChanged(Qt::ScreenOrientation orientation);
    void changeOrientation();

    void checkIntentContent();

    // animation/gesture actions

public Q_SLOTS:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
