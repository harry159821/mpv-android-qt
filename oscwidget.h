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

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    void setOpacity(qreal opacity)
    {
        m_opacity = opacity;

        if(m_opacity_effect == nullptr)
        {
            m_opacity_effect = new QGraphicsOpacityEffect(this);
            this->setGraphicsEffect(m_opacity_effect);
        }
        m_opacity_effect->setOpacity(m_opacity);

        if(m_opacity == 0) {
            this->hide();
        } else {
            this->show();
        }
    }
    qreal m_opacity;
    QGraphicsOpacityEffect *m_opacity_effect;
    qreal opacity()
    {
        return m_opacity;
    }

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

public:
    QStateMachine *machine;
    QPropertyAnimation *anima;
    QState* visiableState;
    QState* invisiableState;

Q_SIGNALS:
    void hideWithAnim_signal();
    void showWithAnim_signal();
    void triggerVisiableAnim_signal();

public Q_SLOTS:
    void initAnmi();
    void hideWithAnim();
    void showWithAnim();
    void triggerVisiableAnim();
};

#endif // OSCWIDGET_H
