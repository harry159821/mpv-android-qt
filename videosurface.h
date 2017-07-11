#ifndef VIDEOSURFACE_H
#define VIDEOSURFACE_H

#include <QtWidgets>
#include <QtGui>
#include <QtCore>
#include <QDebug>
#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>

class VideoSurface : public QOpenGLWidget
{
    Q_OBJECT
public:
    VideoSurface(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~VideoSurface();

    void command(const QVariant& params);
    void setProperty(const QString& name, const QVariant& value);
    QVariant getProperty(const QString& name) const;
    QSize sizeHint() const { return QSize(480, 270);}

signals:
    void durationChanged(int value);
    void positionChanged(int value);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

//    bool event(QEvent *event)
//    {
//        if (event->type() == QEvent::TouchEnd) {
//            qDebug() << "Get TouchEnd Event";
//            return touchEvent(static_cast<QTouchEvent*>(event));
//        }
//        return QOpenGLWidget::event(event);
//    }
//    bool touchEvent(QTouchEvent *e);

    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void swapped();
    void on_mpv_events();
    void maybeUpdate();

private:
    void handle_mpv_event(mpv_event *event);
    static void on_update(void *ctx);

    mpv::qt::Handle mpv;
    mpv_opengl_cb_context *mpv_gl;

};

#endif // VIDEOSURFACE_H
