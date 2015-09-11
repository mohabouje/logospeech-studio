#include "recordersettingsdialog.h"
#include "ui_recordersettingsdialog.h"


RecorderSettingsDialog::RecorderSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecorderSettingsDialog)
{
    ui->setupUi(this);
    connect(ui->restoreButton,SIGNAL(clicked(bool)),this, SLOT(restore()));
    connect(ui->closeButton, SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->saveButton,SIGNAL(clicked(bool)),this,SLOT(save()));
    connect(ui->folderButton,SIGNAL(clicked(bool)), this, SLOT(changeFolder()));
    initComponents();
    loadFromSettings();
}

RecorderSettingsDialog::~RecorderSettingsDialog()
{
    delete ui;
}




void RecorderSettingsDialog::loadFromSettings(){

    audioRecorder = new QAudioRecorder;
    AppSettings *appSettings= new AppSettings();
    QString device = appSettings->currentSettings()->value(KEY_AUDIO_DEVICE,audioRecorder->audioInputs()[0]).toString();
    QString codec = appSettings->currentSettings()->value(KEY_AUDIO_CODEC,audioRecorder->supportedAudioCodecs()[0]).toString();
    QString container = appSettings->currentSettings()->value(KEY_AUDIO_CONTAINER,audioRecorder->supportedContainers()[0]).toString();
    QString folder = appSettings->currentSettings()->value(KEY_AUDIO_FOLDER, QDir::currentPath()+"/test.wav").toString();
#ifdef Q_OS_WIN
    int samplerate = appSettings->currentSettings()->value(KEY_AUDIO_SAMPLERATE,audioRecorder->supportedAudioSampleRates()[0]).toInt();
#endif
#ifdef Q_OS_UNIX
    int samplerate = appSettings->currentSettings()->value(KEY_AUDIO_SAMPLERATE,DEFAULT_SAMPLERATE).toInt();
#endif

    ui->deviceComboBox->setCurrentIndex(ui->deviceComboBox->findText(device));
    ui->codecComboBox->setCurrentIndex(ui->codecComboBox->findText(codec));
    ui->containerComboBox->setCurrentIndex(ui->containerComboBox->findText(container));
    ui->sampleRateComboBox->setCurrentIndex(ui->sampleRateComboBox->findText(QString::number(samplerate)));
    ui->folderText->setText(folder);

    ui->folderText->setMinimumWidth(ui->folderText->minimumSizeHint().width());
    this->setMinimumWidth(ui->folderText->minimumSizeHint().width());

}


void RecorderSettingsDialog::save(){
    AppSettings *appSettings= new AppSettings();
    appSettings->currentSettings()->setValue(KEY_AUDIO_DEVICE,ui->deviceComboBox->currentText());
    appSettings->currentSettings()->setValue(KEY_AUDIO_CODEC,ui->codecComboBox->currentText());
    appSettings->currentSettings()->setValue(KEY_AUDIO_CONTAINER,ui->containerComboBox->currentText());
    appSettings->currentSettings()->setValue(KEY_AUDIO_SAMPLERATE,ui->sampleRateComboBox->currentData().toInt());
    appSettings->currentSettings()->setValue(KEY_AUDIO_FOLDER, ui->folderText->toPlainText());

    accept();
}


void RecorderSettingsDialog::restore(){

    audioRecorder = new QAudioRecorder;
    AppSettings *appSettings = new AppSettings();
    appSettings->currentSettings()->setValue(KEY_AUDIO_DEVICE,audioRecorder->audioInputs()[0]);
    appSettings->currentSettings()->setValue(KEY_AUDIO_CODEC,audioRecorder->supportedAudioCodecs()[0]);
    appSettings->currentSettings()->setValue(KEY_AUDIO_CONTAINER,audioRecorder->supportedContainers()[0]);
    appSettings->currentSettings()->setValue(KEY_AUDIO_SAMPLERATE,audioRecorder->supportedAudioSampleRates()[0]);
    appSettings->currentSettings()->setValue(KEY_AUDIO_FOLDER, QDir::currentPath()+"/test.wav");

    loadFromSettings();
}




void RecorderSettingsDialog::initComponents(){

    audioRecorder=new QAudioRecorder;

    foreach (const QString &device, audioRecorder->audioInputs()) {
        ui->deviceComboBox->addItem(device, QVariant(device));
    }

    //audio codecs

    foreach (const QString &codecName, audioRecorder->supportedAudioCodecs()) {
        ui->codecComboBox->addItem(codecName, QVariant(codecName));
    }

    //containers

    foreach (const QString &containerName, audioRecorder->supportedContainers()) {
        ui->containerComboBox->addItem(containerName, QVariant(containerName));
    }

    //sample rate
#ifdef Q_OS_WIN
    foreach (int sampleRate, audioRecorder->supportedAudioSampleRates()) {
        ui->sampleRateComboBox->addItem(QString::number(sampleRate), QVariant(
                sampleRate));
    }
#endif
#ifdef Q_OS_UNIX
    if (audioRecorder->supportedAudioSampleRates().isEmpty()) {
        const int& size = 5;
        const double supportedSampleRates[5] = {8000,11025,16000,22050,44100};
        for (int i=0;i<size;i++)
            ui->sampleRateComboBox->addItem(QString::number(supportedSampleRates[i]), QVariant(
                   supportedSampleRates[i]));
    } else {
        foreach (int sampleRate, audioRecorder->supportedAudioSampleRates()) {
            ui->sampleRateComboBox->addItem(QString::number(sampleRate), QVariant(
                    sampleRate));
        }
    }
#endif
}

void RecorderSettingsDialog::changeFolder(){
    QString fileName=QFileDialog::getSaveFileName(this,tr("Guardar archivo"),".",tr("WAVFiles (*.wav)\n"));
    if(!fileName.isEmpty()){
        ui->folderText->clear();
        ui->folderText->setText(fileName);
    }
}





