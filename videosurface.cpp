#include "videosurface.h"
#include <stdexcept>

#include <QtAndroidExtras>

extern "C" {
    #include <libavcodec/jni.h>
}

static void wakeup(void *ctx)
{
    QMetaObject::invokeMethod((VideoSurface *)ctx, "on_mpv_events", Qt::QueuedConnection);
}

static void *get_proc_address(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return NULL;
    return (void *)glctx->getProcAddress(QByteArray(name));
}

VideoSurface::VideoSurface(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
    qDebug() << "HelloWorld, VideoSurface Create Start";

    //important
    //For hardware decoding
    av_jni_set_java_vm(QAndroidJniEnvironment::javaVM(), NULL);

    mpv = mpv::qt::Handle::FromRawHandle(mpv_create());
    if (!mpv) {
        qFatal("could not create mpv context");
        throw std::runtime_error("could not create mpv context");
    }

    qDebug() << "create mpv context success!!!";

    mpv_set_option_string(mpv, "config", "yes");
    mpv_set_option_string(mpv, "config-dir", "/storage/emulated/0/mpv/");

    mpv_set_option_string(mpv, "terminal", "yes");
    mpv_set_option_string(mpv, "msg-level", "all=v");       
    if (mpv_initialize(mpv) < 0) {
        qFatal("could not initialize mpv context");
        throw std::runtime_error("could not initialize mpv context");
    }

    //mpv_set_option_string(mpv, "embeddedfonts", "yes");
    mpv_set_option_string(mpv, "vf", "lavfi=\"fps=fps=60:round=down\"");

    qDebug() << "initialize mpv context success";

    // Test To Here
    // Make use of the MPV_SUB_API_OPENGL_CB API.
    mpv::qt::set_option_variant(mpv, "vo", "opengl-cb");

    // Request hw decoding, just for testing.
    mpv::qt::set_option_variant(mpv, "hwdec", "auto");

    mpv_gl = (mpv_opengl_cb_context *)mpv_get_sub_api(mpv, MPV_SUB_API_OPENGL_CB);
    if (!mpv_gl)
        throw std::runtime_error("OpenGL not compiled in");

    mpv_opengl_cb_set_update_callback(mpv_gl, VideoSurface::on_update, (void *)this);
    connect(this, SIGNAL(frameSwapped()), this, SLOT(swapped()));

    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_set_wakeup_callback(mpv, wakeup, this);

    mpv_request_log_messages(mpv, "v");

    qDebug() << "initialize mpv all success";
}

VideoSurface::~VideoSurface()
{
    makeCurrent();
    if (mpv_gl)
        mpv_opengl_cb_set_update_callback(mpv_gl, NULL, NULL);
    // Until this call is done, we need to make sure the player remains
    // alive. This is done implicitly with the mpv::qt::Handle instance
    // in this class.
    mpv_opengl_cb_uninit_gl(mpv_gl);
}

void VideoSurface::command(const QVariant &params)
{
    mpv::qt::command_variant(mpv, params);
}

void VideoSurface::setProperty(const QString &name, const QVariant &value)
{
    mpv::qt::set_property_variant(mpv, name, value);
}

QVariant VideoSurface::getProperty(const QString &name) const
{
    return mpv::qt::get_property_variant(mpv, name);
}

void VideoSurface::initializeGL()
{
    int r = mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address, NULL);
    if (r < 0)
        throw std::runtime_error("could not initialize OpenGL");
}

void VideoSurface::paintGL()
{
    mpv_opengl_cb_draw(mpv_gl, defaultFramebufferObject(), width(), -height());
}

void VideoSurface::swapped()
{
    mpv_opengl_cb_report_flip(mpv_gl, 0);
}

void VideoSurface::on_mpv_events()
{
    // Process all events, until the event queue is empty.
    while (mpv)
    {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
        {
            break;
        }
        handle_mpv_event(event);
    }
}

void VideoSurface::handle_mpv_event(mpv_event *event)
{
    switch (event->event_id)
    {
    case MPV_EVENT_PROPERTY_CHANGE:
    {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        if (strcmp(prop->name, "time-pos") == 0)
        {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                double time = *(double *)prop->data;
                emit positionChanged(time);
            }
        }
        else if (strcmp(prop->name, "duration") == 0)
        {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                double time = *(double *)prop->data;
                emit durationChanged(time);
            }
        }
        break;
    }
    case MPV_EVENT_LOG_MESSAGE:
    {
        mpv_event_log_message* msg = (mpv_event_log_message*)event->data;
        //ALOGV("[%s:%s] %s", msg->prefix, msg->level, msg->text);

        qDebug() << QString(msg->text);

        break;
    }
    default:
        ;
    }
}

// Make Qt invoke mpv_opengl_cb_draw() to draw a new/updated video frame.
void VideoSurface::maybeUpdate()
{
    // If the Qt window is not visible, Qt's update() will just skip rendering.
    // This confuses mpv's opengl-cb API, and may lead to small occasional
    // freezes due to video rendering timing out.
    // Handle this by manually redrawing.
    // Note: Qt doesn't seem to provide a way to query whether update() will
    //       be skipped, and the following code still fails when e.g. switching
    //       to a different workspace with a reparenting window manager.
    if (window()->isMinimized())
    {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        swapped();
        doneCurrent();
    }
    else
    {
        update();
    }
}

void VideoSurface::on_update(void *ctx)
{
    QMetaObject::invokeMethod((VideoSurface *)ctx, "maybeUpdate");
}
