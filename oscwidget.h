#ifndef OSCWIDGET_H
#define OSCWIDGET_H

#include <QtWidgets>
#include <QtGui>
#include "ninepatch.h"

namespace Ui {
class OscWidget;
}

class OscWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OscWidget(QWidget *parent = 0);
    ~OscWidget();

    QSize currentParentSize;

    QPushButton *openFileButton;
    QPushButton *playButton;
    QPushButton *statButton;
    QSlider* progressSlider;

private:
    QImage *backgroundImage;
    TNinePatch *backgroundImage9;
    void paintEvent(QPaintEvent*);

    Ui::OscWidget *ui;
};

#endif // OSCWIDGET_H
