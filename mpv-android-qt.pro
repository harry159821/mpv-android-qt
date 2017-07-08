#-------------------------------------------------
#
# Project created by QtCreator 2017-07-07T08:37:01
#
#-------------------------------------------------

QT       += core gui androidextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mpv-android-qt
TEMPLATE = app

INCLUDEPATH += $$PWD/tools

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    videosurface.cpp \
    tools/androidfiledialog.cpp \
    tools/qDebug2Logcat.cpp

HEADERS += \
        mainwindow.h \
    videosurface.h \
    tools/androidfiledialog.h \
    tools/qDebug2Logcat.h

FORMS += \
        mainwindow.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/src/com/harry/mpv/QtFullscreenActivity.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

LIBS += -L$$PWD/lib -lmpv -lswresample -lavutil -lavcodec -lavformat -lswscale -lavfilter -lavdevice

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $$PWD/android/libs/armeabi-v7a/libavcodec.so \
        $$PWD/android/libs/armeabi-v7a/libavdevice.so \
        $$PWD/android/libs/armeabi-v7a/libavfilter.so \
        $$PWD/android/libs/armeabi-v7a/libavformat.so \
        $$PWD/android/libs/armeabi-v7a/libavutil.so \
        $$PWD/android/libs/armeabi-v7a/libc++_shared.so \
        $$PWD/android/libs/armeabi-v7a/libmpv.so \
        $$PWD/android/libs/armeabi-v7a/libswresample.so \
        $$PWD/android/libs/armeabi-v7a/libswscale.so
}

