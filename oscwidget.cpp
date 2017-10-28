#include "oscwidget.h"
#include "ui_oscwidget.h"
#include <QtCore>

OscWidget::OscWidget(QWidget *parent) :
    QWidget(parent),
    backgroundImage9(nullptr),
    currentParentSize(QSize(0, 0)),
    openFileButton(nullptr),
    playButton(nullptr),
    statButton(nullptr),
    progressSlider(nullptr),
    m_opacity(1),
    m_opacity_effect(nullptr),
    ui(new Ui::OscWidget)
{

    ui->setupUi(this);
    this->initAnmi();

    backgroundImage = new QImage(":/resource/OscWidgetbackground.9.png");
    backgroundImage9 = new TNinePatch(*backgroundImage);

    // Set Attribute
    this->setWindowFlag(Qt::FramelessWindowHint, true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    openFileButton = ui->ui_openFileButton;
    playButton = ui->ui_playButton;
    statButton = ui->ui_statButton;
    progressSlider = ui->ui_progressSlider;

    // Set Attribute To Sub widget
    openFileButton->setAttribute(Qt::WA_TranslucentBackground, true);
    playButton->setAttribute(Qt::WA_TranslucentBackground, true);
    statButton->setAttribute(Qt::WA_TranslucentBackground, true);
    progressSlider->setAttribute(Qt::WA_TranslucentBackground, true);
}

OscWidget::~OscWidget()
{
    delete ui;
}

void OscWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect rect = this->rect();

    try {
        backgroundImage9->SetImageSize(rect.width(), rect.height());
        backgroundImage9->Draw(painter, 0, 0);
    } catch (NinePatchException* e) {
        qDebug() << QString(e->what());
        delete e;
    }

    // QWidget::paintEvent(event);
}

void OscWidget::initAnmi()
{
    machine = new QStateMachine(this);
    machine->setGlobalRestorePolicy(QStateMachine::RestoreProperties);
    visiableState = new QState(machine);
    visiableState->assignProperty(this, "opacity", 1);
    invisiableState = new QState(machine);
    visiableState->assignProperty(this, "opacity", 0);

    anima = new QPropertyAnimation(this, "opacity", this);
    anima->setDuration(240);
    machine->addDefaultAnimation(anima);

    invisiableState->addTransition(this, SIGNAL(showWithAnim_signal()), visiableState);
    visiableState->addTransition(this, SIGNAL(hideWithAnim_signal()), invisiableState);

    invisiableState->addTransition(this, SIGNAL(triggerVisiableAnim_signal()), visiableState);
    visiableState->addTransition(this, SIGNAL(triggerVisiableAnim_signal()), invisiableState);

    machine->setInitialState(invisiableState);
    machine->start();
}

void OscWidget::hideWithAnim()
{
    emit this->hideWithAnim_signal();
}

void OscWidget::showWithAnim()
{
    emit this->showWithAnim_signal();
}

void OscWidget::triggerVisiableAnim()
{
    emit this->triggerVisiableAnim_signal();
}

