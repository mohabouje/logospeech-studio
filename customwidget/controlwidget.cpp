#include "controlwidget.h"
#include "ui_controlwidget.h"

ControlWidget::ControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlWidget)
{
    ui->setupUi(this);
    ui->compressionFactor->setMinimum(-100);
    ui->compressionFactor->setMaximum(100);
    ui->compressionFactor->setValue(0);

    ui->pitchTolerance->setMinimum(0);
    ui->pitchTolerance->setMaximum(100);

    ui->pitchLevel->setMinimum(-90);
    ui->pitchLevel->setMaximum(30);

    connect(ui->pitchLevel, SIGNAL(valueChanged(int)),this, SLOT(setLabel(int)));
    connect(ui->pitchTolerance, SIGNAL(valueChanged(int)),this, SLOT(setLabel(int)));

}

void ControlWidget::setLabel(const int& label) {
    if (QObject::sender() == ui->pitchLevel) {
        ui->levelValue->setText(QString("%1 dB").arg(label));
    } else {
        ui->toleranceValue->setText(QString("%1 %").arg(label));
    }
}

ControlWidget::~ControlWidget()
{
    delete ui;
}


QDial* ControlWidget::levelDial() {
    return ui->pitchLevel;
}

QDial* ControlWidget::toleranceDial() {
    return ui->pitchTolerance;
}

QSlider* ControlWidget::slider() {
    return ui->compressionFactor;
}
