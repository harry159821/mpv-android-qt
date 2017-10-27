#include "mainwindow.h"
#include <QApplication>
#include "qDebug2Logcat.h"
#include <QDebug>

#include <jni.h>

int main(int argc, char *argv[])
{
    installLogcatMessageHandler("com.harry.mpv");

    QApplication a(argc, argv);

    int id = QFontDatabase::addApplicationFont("://resource/DroidSans.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont appFont = qApp->font();
    appFont.setFamily(family);
    qDebug() << family;
    qApp->setFont(appFont);

    MainWindow w;
    w.show();

    return a.exec();
}
