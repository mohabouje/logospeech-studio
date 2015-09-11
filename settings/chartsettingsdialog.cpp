#include "appsettings.h"
#include "chartsettingsdialog.h"
#include "ui_chartsettingsdialog.h"



ChartSettingsDialog::ChartSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChartSettingsDialog)
{
    ui->setupUi(this);

    connect(ui->spectrogramColorMap,SIGNAL(currentIndexChanged(int)), this, SLOT(showColorMap(int)));
    connect(ui->restoreButton,SIGNAL(clicked(bool)),this, SLOT(restore()));
    connect(ui->closeButton, SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->saveButton,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->energyColor, SIGNAL(clicked(bool)),this, SLOT(changeColor()));
    connect(ui->pitchColor, SIGNAL(clicked(bool)),this, SLOT(changeColor()));
    connect(ui->muLaw,SIGNAL(valueChanged(int)), this, SLOT(muLawValueChanged(int)));

    loadFromSettings();

}


QSlider* ChartSettingsDialog::slider() {
    return ui->muLaw;
}


void ChartSettingsDialog::muLawValueChanged(const int& currentValue){
    ui->valueLabel->setText(QString("<b>%1</b>").arg(_muLawValue));
    _muLawValue = currentValue;
}


static void changeQPushButtonBackGroundColor(QPushButton *button,const  QColor& color){
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, color);
    button->setAutoFillBackground(true);
    button->setPalette(pal);
    button->update();
}


void ChartSettingsDialog::changeColor(){

     QPushButton *sender =  qobject_cast<QPushButton *>(QObject::sender());
     QColorDialog *colorDialog = new QColorDialog(this);
     colorDialog->setCurrentColor(sender->palette().color(QPalette::Button));
     int res = colorDialog->exec();
     if ( res == QDialog::Accepted){
         QColor colorSelected = colorDialog->currentColor();
         changeQPushButtonBackGroundColor(sender,colorSelected);
     }
}




void ChartSettingsDialog::loadFromSettings(){



    // Assign values on settings
    AppSettings *appSettings= new AppSettings();

    int currentTab = appSettings->currentSettings()->value(KEY_ENERGY_CURRENT_TAB,0).toInt();
    ui->tabWidget->setCurrentIndex(currentTab);



    // Energy options
    _energy_LineColor =  appSettings->currentSettings()->value(KEY_ENERGY_CHART_LINECOLOR,ENERGY_CHART_LINECOLOR).toString();
    ui->energyColor->setStyleSheet(styleSheet());
    changeQPushButtonBackGroundColor(ui->energyColor, QColor(_energy_LineColor));


    _energy_LineWidth = appSettings->currentSettings()->value(KEY_ENERGY_CHART_LINEWIDTH, ENERGY_CHART_LINEWIDTH).toDouble();
    ui->energyLineWidth->setValue(_energy_LineWidth);


    _energy_X_Axis = appSettings->currentSettings()->value(KEY_ENERGY_CHART_X_AXIS,ENERGY_CHART_X_AXIS).toBool();
    _energy_Y_Axis = appSettings->currentSettings()->value(KEY_ENERGY_CHART_Y_AXIS,ENERGY_CHART_Y_AXIS).toBool();
    _energy_X_Grid = appSettings->currentSettings()->value(KEY_ENERGY_CHART_X_GRID,ENERGY_CHART_X_GRID).toBool();
    _energy_Y_Grid = appSettings->currentSettings()->value(KEY_ENERGY_CHART_Y_GRID,ENERGY_CHART_Y_GRID).toBool();
    ui->energyXAxis->setChecked(_energy_X_Axis);
    ui->energyYAxis->setChecked(_energy_Y_Axis);
    ui->energyXGrid->setChecked(_energy_X_Grid);
    ui->energyYGrid->setChecked(_energy_Y_Grid);

    // Pitch options

    // Energy options
    _pitch_LineColor =  appSettings->currentSettings()->value(KEY_PITCH_CHART_LINECOLOR,PITCH_CHART_LINECOLOR).toString();
    ui->pitchColor->setStyleSheet(styleSheet());
    changeQPushButtonBackGroundColor(ui->pitchColor, QColor(_pitch_LineColor));


    _pitch_LineWidth = appSettings->currentSettings()->value(KEY_PITCH_CHART_LINEWIDTH, PITCH_CHART_LINEWIDTH).toDouble();
    ui->pitchLineWidth->setValue(_pitch_LineWidth);


    _pitch_X_Axis = appSettings->currentSettings()->value(KEY_PITCH_CHART_X_AXIS,PITCH_CHART_X_AXIS).toBool();
    _pitch_Y_Axis = appSettings->currentSettings()->value(KEY_PITCH_CHART_Y_AXIS,PITCH_CHART_Y_AXIS).toBool();
    _pitch_X_Grid = appSettings->currentSettings()->value(KEY_PITCH_CHART_X_GRID,PITCH_CHART_X_GRID).toBool();
    _pitch_Y_Grid = appSettings->currentSettings()->value(KEY_PITCH_CHART_Y_GRID,PITCH_CHART_Y_GRID).toBool();
    ui->pitchXAxis->setChecked(_pitch_X_Axis);
    ui->pitchYAxis->setChecked(_pitch_Y_Axis);
    ui->pitchXGrid->setChecked(_pitch_X_Grid);
    ui->pitchYGrid->setChecked(_pitch_Y_Grid);


    // Spectrogram options
    ui->spectrogramColorMap->setCurrentText(appSettings->currentSettings()->value(KEY_SPECTROGRAM_CHART_COLORMAP,SPECTROGRAM_CHART_COLORMAP).toString());
    _spectrogram_colormap = ui->spectrogramColorMap->currentIndex();
    showColorMap(ui->spectrogramColorMap->currentIndex());
    _spectrogram_X_Axis = appSettings->currentSettings()->value(KEY_SPECTROGRAM_CHART_X_AXIS,SPECTROGRAM_CHART_X_AXIS).toBool();
    _spectrogram_Y_Axis = appSettings->currentSettings()->value(KEY_SPECTROGRAM_CHART_Y_AXIS,SPECTROGRAM_CHART_Y_AXIS).toBool();
    _muLawValue = appSettings->currentSettings()->value(KEY_SPECTROGRAM_MU_LAW,0).toDouble();
    ui->valueLabel->setText(QString("<b>%1</b>").arg(_muLawValue));
    ui->muLaw->setValue(_muLawValue);
    ui->spectrogramXAxis->setChecked(_spectrogram_X_Axis);;
    ui->spectrogramYAxis->setChecked(_spectrogram_Y_Axis);
}
int ChartSettingsDialog::muLawValue() const
{
    return _muLawValue;
}

int ChartSettingsDialog::spectrogram_colormap() const
{
    return _spectrogram_colormap;
}

bool ChartSettingsDialog::spectrogram_X_Axis() const
{
    return _spectrogram_X_Axis;
}

bool ChartSettingsDialog::spectrogram_Y_Axis() const
{
    return _spectrogram_Y_Axis;
}

bool ChartSettingsDialog::pitch_Y_Grid() const
{
    return _pitch_Y_Grid;
}

bool ChartSettingsDialog::pitch_X_Grid() const
{
    return _pitch_X_Grid;
}

bool ChartSettingsDialog::pitch_Y_Axis() const
{
    return _pitch_Y_Axis;
}

bool ChartSettingsDialog::pitch_X_Axis() const
{
    return _pitch_X_Axis;
}

QString ChartSettingsDialog::pitch_LineColor() const
{
    return _pitch_LineColor;
}

int ChartSettingsDialog::pitch_LineWidth() const
{
    return _pitch_LineWidth;
}

bool ChartSettingsDialog::energy_Y_Grid() const
{
    return _energy_Y_Grid;
}

bool ChartSettingsDialog::energy_X_Grid() const
{
    return _energy_X_Grid;
}

bool ChartSettingsDialog::energy_Y_Axis() const
{
    return _energy_Y_Axis;
}

bool ChartSettingsDialog::energy_X_Axis() const
{
    return _energy_X_Axis;
}

QString ChartSettingsDialog::energy_LineColor() const
{
    return _energy_LineColor;
}

int ChartSettingsDialog::energy_LineWidth() const
{
    return _energy_LineWidth;
}




void ChartSettingsDialog::restore(){
    // Assign values on settings
    AppSettings *appSettings= new AppSettings();
    // pitch options
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_LINECOLOR,ENERGY_CHART_LINECOLOR);
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_LINEWIDTH, ENERGY_CHART_LINEWIDTH);
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_X_AXIS,ENERGY_CHART_X_AXIS);
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_Y_AXIS,ENERGY_CHART_Y_AXIS);
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_X_GRID,ENERGY_CHART_X_GRID);
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_Y_GRID,ENERGY_CHART_Y_GRID);

    // Pitch options
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_LINECOLOR,PITCH_CHART_LINECOLOR);
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_LINEWIDTH, PITCH_CHART_LINEWIDTH);
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_X_AXIS,PITCH_CHART_X_AXIS);
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_Y_AXIS,PITCH_CHART_Y_AXIS);
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_X_GRID,PITCH_CHART_X_GRID);
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_Y_GRID,PITCH_CHART_Y_GRID);

    // Spectrogram options
    appSettings->currentSettings()->setValue(KEY_SPECTROGRAM_CHART_COLORMAP,SPECTROGRAM_CHART_COLORMAP);
    appSettings->currentSettings()->setValue(KEY_SPECTROGRAM_CHART_X_AXIS,SPECTROGRAM_CHART_X_AXIS);
    appSettings->currentSettings()->setValue(KEY_SPECTROGRAM_CHART_Y_AXIS,SPECTROGRAM_CHART_Y_AXIS);
    appSettings->currentSettings()->setValue(KEY_SPECTROGRAM_MU_LAW,0);
    // Refresh UI elements
    loadFromSettings();

}



void ChartSettingsDialog::save(){



    // Assign values on settings
    AppSettings *appSettings= new AppSettings();

    appSettings->currentSettings()->setValue(KEY_ENERGY_CURRENT_TAB,ui->tabWidget->currentIndex());


    // Energy options
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_LINECOLOR,ui->energyColor->palette().color(QPalette::Button).name());
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_LINEWIDTH, ui->energyLineWidth->value());
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_X_AXIS,ui->energyXAxis->isChecked());
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_Y_AXIS,ui->energyYAxis->isChecked());
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_X_GRID,ui->energyXGrid->isChecked());
    appSettings->currentSettings()->setValue(KEY_ENERGY_CHART_Y_GRID,ui->energyYGrid->isChecked());

    // Pitch options
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_LINECOLOR,ui->pitchColor->palette().color(QPalette::Button).name());
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_LINEWIDTH, ui->pitchLineWidth->value() );
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_X_AXIS,ui->pitchXAxis->isChecked());
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_Y_AXIS,ui->pitchYAxis->isChecked());
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_X_GRID,ui->pitchXGrid->isChecked());
    appSettings->currentSettings()->setValue(KEY_PITCH_CHART_Y_GRID,ui->pitchYGrid->isChecked());

    // Spectrogram options
    appSettings->currentSettings()->setValue(KEY_SPECTROGRAM_CHART_COLORMAP,ui->spectrogramColorMap->currentText());
    appSettings->currentSettings()->setValue(KEY_SPECTROGRAM_CHART_X_AXIS, ui->spectrogramXAxis->isChecked());
    appSettings->currentSettings()->setValue(KEY_SPECTROGRAM_CHART_Y_AXIS, ui->spectrogramYAxis->isChecked());
    appSettings->currentSettings()->setValue(KEY_SPECTROGRAM_MU_LAW,ui->muLaw->value());


    loadFromSettings();
    accept();
}


ChartSettingsDialog::~ChartSettingsDialog()
{
    delete ui;
}




void ChartSettingsDialog::showColorMap(const int index){
    switch (index) {
       case 0:{
           QPixmap pic(":/resources/colormaps/spectral_colormap.png");
           ui->colormapImage->setPixmap(pic.scaled(ui->colormapImage->width(),ui->colormapImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 1:{
           QPixmap pic(":/resources/colormaps/jet_colormap.png");
           ui->colormapImage->setPixmap(pic.scaled(ui->colormapImage->width(),ui->colormapImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 2:{
           QPixmap pic(":/resources/colormaps/gray_colormap.png");
           ui->colormapImage->setPixmap(pic.scaled(ui->colormapImage->width(),ui->colormapImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 3:{
           QPixmap pic(":/resources/colormaps/hot_colormap.png");
           ui->colormapImage->setPixmap(pic.scaled(ui->colormapImage->width(),ui->colormapImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 4:{
           QPixmap pic(":/resources/colormaps/cool_colormap.png");
           ui->colormapImage->setPixmap(pic.scaled(ui->colormapImage->width(),ui->colormapImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 5:{
           QPixmap pic(":/resources/colormaps/cooper_colormap.png");
           ui->colormapImage->setPixmap(pic.scaled(ui->colormapImage->width(),ui->colormapImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }default:
           QPixmap pic(":/resources/colormaps/spectrogram_colormap.png");
           ui->colormapImage->setPixmap(pic.scaled(ui->colormapImage->width(),ui->colormapImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }

}


