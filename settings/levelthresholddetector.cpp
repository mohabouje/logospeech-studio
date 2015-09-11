#include "levelthresholddetector.h"
#include "ui_levelthresholddetector.h"
#include <capturedata/util.h>
#include <QTimer>
#include <QDebug>

LevelThreSholdDetector::LevelThreSholdDetector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LevelThreSholdDetector)
{
    ui->setupUi(this);
    _tempAZCR = 0.0;
    _tempEnergy = 0.0;
    _iteration = 0;

    _recorder = new Recorder;
    _recorder->init();
    _temp = new_vec(_recorder->samplerate() * 0.04);
    connect(_recorder, SIGNAL(bufferReady(QAudioBuffer)), this, SLOT(process(QAudioBuffer)));
    connect(ui->toolButton,SIGNAL(clicked(bool)), this, SLOT(play()));
}

LevelThreSholdDetector::~LevelThreSholdDetector()
{
    delete ui;
}


void LevelThreSholdDetector::process(const QAudioBuffer& buffer) {
    const qreal max = getPeakValue(buffer.format());
    const qint16 *data = buffer.data<qint16>();

    logo_vector temp = new_vec(buffer.sampleCount());
    for (int i=0;i <buffer.frameCount();i++)
         temp[i] = data[i]/max;
    logo_real tempValue = 0.0;
    logo_energy_db_spl(temp,tempValue);
    _tempEnergy  += tempValue;
    logo_zero_crossing_rate(temp, tempValue);
    _tempAZCR += tempValue;
    _iteration++;
    ui->progressBar->setValue(_iteration);

}


void LevelThreSholdDetector::play() {
    ui->toolButton->setEnabled(false);
    QTimer::singleShot(4000, this, SLOT(getResult()));
    _recorder->record();
}

void LevelThreSholdDetector::getResult() {
    _recorder->stop();
    _tempEnergy /= _iteration;
    _tempAZCR /= _iteration;
    accept();
}
logo_real LevelThreSholdDetector::getTempAZCR() const
{
    return _tempAZCR;
}

logo_real LevelThreSholdDetector::getTempEnergy() const
{
    return _tempEnergy;
}

