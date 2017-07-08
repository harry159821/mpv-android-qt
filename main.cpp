#include "mainwindow.h"
#include <QApplication>
#include "qDebug2Logcat.h"

#include <jni.h>

int main(int argc, char *argv[])
{
    installLogcatMessageHandler("com.harry.mpv");

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
