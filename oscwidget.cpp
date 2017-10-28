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
    ui(new Ui::OscWidget)
{
    ui->setupUi(this);

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
