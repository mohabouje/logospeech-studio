#include "aboutapp.h"
#include "ui_aboutapp.h"

AboutApp::AboutApp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutApp)
{
    ui->setupUi(this);
}

AboutApp::~AboutApp()
{
    delete ui;
}
