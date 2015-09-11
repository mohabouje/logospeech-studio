#include "appsettings.h"
#include "processsettingsdialog.h"
#include "ui_processsettingsdialog.h"
#include <math.h>

ProcessSettingsDialog::ProcessSettingsDialog(const int& currentSampleRate, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessSettingsDialog),
    _currentSampleRate(currentSampleRate)
{
    ui->setupUi(this);

    connect(ui->restoreButton,SIGNAL(clicked(bool)),this, SLOT(restore()));
    connect(ui->closeButton, SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->saveButton,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->windowComboBox,SIGNAL(currentIndexChanged(int)),this, SLOT(showWindow()));
    connect(ui->samplesFFT,SIGNAL(valueChanged(int)), this, SLOT(samplesFFTChanged(int)));
    connect(ui->windowTime,SIGNAL(valueChanged(double)),this, SLOT(windowTimeChanged(double)));
    ui->shortTimeMethodCombobox->addItem(tr("Short Time Energy"));
    ui->shortTimeMethodCombobox->addItem(tr("Short Time AZCR"));

    ui->labelInfo->setText(QString("<b>Current SampleRate = %1 Hz</b>").arg(_currentSampleRate));

    loadFromSettings();

}

void ProcessSettingsDialog::setFFTMethod(const int& value) {
    _fftMethod = value;
}
void ProcessSettingsDialog::setPitchMethod(const int& value) {
    _pitchMethod = value;
}


void ProcessSettingsDialog::windowTimeChanged(const double& value) {
    const int windowSamples = ceil(_currentSampleRate * value);
    if (_fftsamples < windowSamples){
        _fftsamples = windowSamples;
        ui->samplesFFT->setValue(windowSamples);
        ui->labelInfo->setText(QString("<b>Current SampleRate = %1 Hz</b><br/><b>Current WindowSamples = %2</b>"
                                       "<br/><b><font color='red'>ERROR: FFT samples must be bigger than WindowSamples.</font></b>"
                                       "<br/><b><font color='red'>Using WindowSamples as current value.</font></b>").arg(_currentSampleRate).arg(windowSamples)
                           );
    } else {
        ui->labelInfo->setText(QString("<b>Current SampleRate = %1 Hz</b><br/><b>Current WindowSamples = %2</b>").arg(_currentSampleRate).arg(windowSamples)
                               );
    }
}

void ProcessSettingsDialog::samplesFFTChanged(const int& value) {
    const double windowTime = ui->windowTime->value();
    const int windowSamples = ceil(_currentSampleRate * windowTime);
    if (value < windowSamples){
        _fftsamples = windowSamples;
        ui->samplesFFT->setValue(windowSamples);
        ui->labelInfo->setText(QString("<b>Current SampleRate = %1 Hz</b><br/><b>Current WindowSamples = %2</b>"
                                       "<br/><b><font color='red'>ERROR: FFT samples must be bigger than WindowSamples.</font></b>"
                                       "<br/><b><font color='red'>Using WindowSamples as current value.</font></b>").arg(_currentSampleRate).arg(windowSamples)
                           );
    } else {
        ui->samplesFFT->setValue(value);
        ui->labelInfo->setText(QString("<b>Current SampleRate = %1 Hz</b><br/><b>Current WindowSamples = %2</b>").arg(_currentSampleRate).arg(windowSamples)
                               );
    }
}

void ProcessSettingsDialog::loadFromSettings(){
    // Assign values on settings
    AppSettings *appSettings= new AppSettings();

    int currentTab = appSettings->currentSettings()->value(KEY_PROCESS_CURRENT_TAB,0).toInt();
    ui->tabWidget->setCurrentIndex(currentTab);


    _isDC = appSettings->currentSettings()->value(KEY_PROCESS_DC,true).toBool();
    _isPE= appSettings->currentSettings()->value(KEY_PROCESS_EMPHASIS,true).toBool();
    _currentWindow = appSettings->currentSettings()->value(KEY_PROCESS_WINDOW,0).toInt();

    ui->useDC->setChecked(_isDC);
    ui->usePreEnphasis->setChecked(_isPE);
    ui->windowComboBox->setCurrentIndex(_currentWindow);
    showWindow();

    _windowTime = appSettings->currentSettings()->value(KEY_PROCESS_WINDOW_TIME,0.04).toDouble();
    _overlapTime = appSettings->currentSettings()->value(KEY_PROCESS_WINDOW_SHIFT_TIME,0.02).toDouble();
    _bufferTime = appSettings->currentSettings()->value(KEY_PROCESS_BUFFER_TIME,5).toInt();
    _shortTimeMethod = appSettings->currentSettings()->value(KEY_PROCESS_METHOD,0).toInt();
    _pitchMethod = appSettings->currentSettings()->value(KEY_PROCESS_PITCH_METHOD,0).toInt();
    _pitchMin = appSettings->currentSettings()->value(KEY_PROCESS_PITCH_MIN,100).toInt();
    _pitchMax = appSettings->currentSettings()->value(KEY_PROCESS_PITCH_MAX,500).toInt();


    ui->windowTime->setValue(_windowTime);
    ui->windowShift->setValue(_overlapTime);
    ui->bufferTime->setValue(_bufferTime);
    ui->shortTimeMethodCombobox->setCurrentIndex(_shortTimeMethod);
    ui->pitchMethodCombobox->setCurrentIndex(_pitchMethod);
    ui->minPitch->setValue(_pitchMin);
    ui->maxPitch->setValue(_pitchMax);

    _fftMethod = appSettings->currentSettings()->value(KEY_PROCESS_FFT_METHOD,0).toInt();
    _fftsamples = appSettings->currentSettings()->value(KEY_PROCESS_FFT_SAMPLES,512).toInt();
    _orderLPC = appSettings->currentSettings()->value(KEY_PROCESS_ORDER_LPC,13).toInt();
    _orderCepstrum = appSettings->currentSettings()->value(KEY_PROCESS_ORDER_CEPSTRUM,15).toInt();
    _orderFilterBanks = appSettings->currentSettings()->value(KEY_PROCESS_ORDER_FILTERBANKS,30).toInt();

    ui->spectrogramMethod->setCurrentIndex(_fftMethod);
    samplesFFTChanged(_fftsamples);
    ui->orderLPC->setValue(_orderLPC);
    ui->orderCepstrum->setValue(_orderCepstrum);
    ui->orderFilterBanks->setValue(_orderFilterBanks);

}
void ProcessSettingsDialog::setCurrentSampleRate(int currentSampleRate)
{
    _currentSampleRate = currentSampleRate;
    windowTimeChanged(ui->windowTime->value());
}

int ProcessSettingsDialog::shortTimeMethod() const
{
    return _shortTimeMethod;
}

bool ProcessSettingsDialog::isDC() const
{
    return _isDC;
}

bool ProcessSettingsDialog::isPE() const
{
    return _isPE;
}

int ProcessSettingsDialog::currentWindow() const
{
    return _currentWindow;
}

double ProcessSettingsDialog::windowTime() const
{
    return _windowTime;
}

double ProcessSettingsDialog::overlapTime() const
{
    return _overlapTime;
}

int ProcessSettingsDialog::bufferTime() const
{
    return _bufferTime;
}


int ProcessSettingsDialog::pitchMethod() const
{
    return _pitchMethod;
}

int ProcessSettingsDialog::pitchMin() const
{
    return _pitchMin;
}

int ProcessSettingsDialog::pitchMax() const
{
    return _pitchMax;
}

int ProcessSettingsDialog::fftMethod() const
{
    return _fftMethod;
}

int ProcessSettingsDialog::fftsamples() const
{
    return _fftsamples;
}

int ProcessSettingsDialog::orderLPC() const
{
    return _orderLPC;
}

int ProcessSettingsDialog::orderCepstrum() const
{
    return _orderCepstrum;
}

int ProcessSettingsDialog::orderFilterBanks() const
{
    return _orderFilterBanks;
}


void ProcessSettingsDialog::restore(){
    // Assign values on settings
    AppSettings *appSettings= new AppSettings();

    appSettings->currentSettings()->setValue(KEY_PROCESS_DC,true);
    appSettings->currentSettings()->setValue(KEY_PROCESS_EMPHASIS,true);
    appSettings->currentSettings()->setValue(KEY_PROCESS_WINDOW,0);

    appSettings->currentSettings()->setValue(KEY_PROCESS_WINDOW_TIME,0.04);
    appSettings->currentSettings()->setValue(KEY_PROCESS_WINDOW_SHIFT_TIME,0.02);
    appSettings->currentSettings()->setValue(KEY_PROCESS_BUFFER_TIME,5);
    appSettings->currentSettings()->setValue(KEY_PROCESS_METHOD,0);
    appSettings->currentSettings()->setValue(KEY_PROCESS_PITCH_METHOD,0);
    appSettings->currentSettings()->setValue(KEY_PROCESS_PITCH_MIN,100);
    appSettings->currentSettings()->setValue(KEY_PROCESS_PITCH_MAX,500);


    appSettings->currentSettings()->setValue(KEY_PROCESS_FFT_METHOD,0);
    appSettings->currentSettings()->setValue(KEY_PROCESS_FFT_SAMPLES,512);
    appSettings->currentSettings()->setValue(KEY_PROCESS_ORDER_LPC,13);
    appSettings->currentSettings()->setValue(KEY_PROCESS_ORDER_CEPSTRUM,10);
    appSettings->currentSettings()->setValue(KEY_PROCESS_ORDER_FILTERBANKS,30);

    // Refresh UI elements
    loadFromSettings();

}


void ProcessSettingsDialog::showWindow(){
    int index = ui->windowComboBox->currentIndex();
    switch (index) {
       case 0:{
           QPixmap pic(":/resources/windows/hamming.svg");
           ui->windowImage->setPixmap(pic.scaled(ui->windowImage->width(),ui->windowImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 1:{
           QPixmap pic(":/resources/windows/hann.svg");
           ui->windowImage->setPixmap(pic.scaled(ui->windowImage->width(),ui->windowImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 2:{
           QPixmap pic(":/resources/windows/bartlet.svg");
           ui->windowImage->setPixmap(pic.scaled(ui->windowImage->width(),ui->windowImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 3:{
           QPixmap pic(":/resources/windows/blackman.svg");
           ui->windowImage->setPixmap(pic.scaled(ui->windowImage->width(),ui->windowImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 4:{
           QPixmap pic(":/resources/windows/kaiser.svg");
           ui->windowImage->setPixmap(pic.scaled(ui->windowImage->width(),ui->windowImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }case 5:{
           QPixmap pic(":/resources/windows/gauss.svg");
           ui->windowImage->setPixmap(pic.scaled(ui->windowImage->width(),ui->windowImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }default:
           QPixmap pic(":/resources/windows/hamming.svg");
           ui->windowImage->setPixmap(pic.scaled(ui->windowImage->width(),ui->windowImage->height(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
           break;
       }

}


void ProcessSettingsDialog::save(){


    // Assign values on settings
    AppSettings *appSettings= new AppSettings();


    appSettings->currentSettings()->setValue(KEY_PROCESS_CURRENT_TAB,ui->tabWidget->currentIndex());

    appSettings->currentSettings()->setValue(KEY_PROCESS_DC,ui->useDC->isChecked());
    appSettings->currentSettings()->setValue(KEY_PROCESS_EMPHASIS,ui->usePreEnphasis->isChecked());
    appSettings->currentSettings()->setValue(KEY_PROCESS_WINDOW,ui->windowComboBox->currentIndex());


    appSettings->currentSettings()->setValue(KEY_PROCESS_WINDOW_TIME,ui->windowTime->value());
    appSettings->currentSettings()->setValue(KEY_PROCESS_WINDOW_SHIFT_TIME,ui->windowShift->value());
    appSettings->currentSettings()->setValue(KEY_PROCESS_BUFFER_TIME,ui->bufferTime->value());
    appSettings->currentSettings()->setValue(KEY_PROCESS_METHOD,ui->shortTimeMethodCombobox->currentIndex());
    appSettings->currentSettings()->setValue(KEY_PROCESS_PITCH_METHOD,ui->pitchMethodCombobox->currentIndex());
    appSettings->currentSettings()->setValue(KEY_PROCESS_PITCH_MIN,ui->minPitch->value());
    appSettings->currentSettings()->setValue(KEY_PROCESS_PITCH_MAX,ui->maxPitch->value());


    appSettings->currentSettings()->setValue(KEY_PROCESS_FFT_METHOD,ui->spectrogramMethod->currentIndex());
    appSettings->currentSettings()->setValue(KEY_PROCESS_FFT_SAMPLES,ui->samplesFFT->value());
    appSettings->currentSettings()->setValue(KEY_PROCESS_ORDER_LPC,ui->orderLPC->value());
    appSettings->currentSettings()->setValue(KEY_PROCESS_ORDER_CEPSTRUM,ui->orderCepstrum->value());
    appSettings->currentSettings()->setValue(KEY_PROCESS_ORDER_FILTERBANKS,ui->orderFilterBanks->value());

    setFFTMethod(ui->spectrogramMethod->currentIndex());
    setPitchMethod(ui->spectrogramMethod->currentIndex());

    //Save on container
    loadFromSettings();
    accept();
}


ProcessSettingsDialog::~ProcessSettingsDialog()
{
    delete ui;
}

