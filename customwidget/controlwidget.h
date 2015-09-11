#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QDial>
#include <QSlider>
#include <QWidget>

namespace Ui {
class ControlWidget;
}

class ControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(QWidget *parent = 0);
    ~ControlWidget();

    QSlider* slider();
    QDial* toleranceDial();
    QDial* levelDial();
private slots:
    void setLabel(const int &label);
private:
    Ui::ControlWidget *ui;
};

#endif // CONTROLWIDGET_H
