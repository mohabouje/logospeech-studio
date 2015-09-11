#include "recorder.h"
#include <settings/recordersettingsdialog.h>

Recorder::Recorder(QObject *parent) :
    QObject(parent),
    _appSettings(new AppSettings)
{

}

void Recorder::init() {

    _audioRecorder =  new QAudioRecorder(this);
    _audioProbe = new QAudioProbe(this);

#ifdef Q_OS_WIN
    _device = _appSettings->currentSettings()->value(KEY_AUDIO_DEVICE,_audioRecorder->audioInputs()[0]).toString();
    _codec = _appSettings->currentSettings()->value(KEY_AUDIO_CODEC,_audioRecorder->supportedAudioCodecs()[0]).toString();
    _container = _appSettings->currentSettings()->value(KEY_AUDIO_CONTAINER,_audioRecorder->supportedContainers()[0]).toString();
    _samplerate = _appSettings->currentSettings()->value(KEY_AUDIO_SAMPLERATE,_audioRecorder->supportedAudioSampleRates()[0]).toInt();
    _folder = _appSettings->currentSettings()->value(KEY_AUDIO_FOLDER, QDir::currentPath()+"/test.wav").toString();
#endif
#ifdef Q_OS_UNIX
    _device = _appSettings->currentSettings()->value(KEY_AUDIO_DEVICE,_audioRecorder->audioInputs()[0]).toString();
    _codec = _appSettings->currentSettings()->value(KEY_AUDIO_CODEC,_audioRecorder->supportedAudioCodecs()[0]).toString();
    _container = _appSettings->currentSettings()->value(KEY_AUDIO_CONTAINER,_audioRecorder->supportedContainers()[0]).toString();
    _samplerate = _appSettings->currentSettings()->value(KEY_AUDIO_SAMPLERATE,DEFAULT_SAMPLERATE).toInt();
    _folder = _appSettings->currentSettings()->value(KEY_AUDIO_FOLDER, QDir::currentPath()+"/test.wav").toString();
#endif
    _audioRecorder->setAudioInput(_device);
    _audioRecorder->setContainerFormat(_container);
    QAudioEncoderSettings audioSetting;
    audioSetting.setCodec(_codec);
    audioSetting.setChannelCount(1);
    audioSetting.setEncodingMode(QMultimedia::ConstantBitRateEncoding);
    audioSetting.setQuality(QMultimedia::NormalQuality);
    audioSetting.setSampleRate(_samplerate);
    _audioRecorder->setEncodingSettings(audioSetting, QVideoEncoderSettings(), _container);
    _audioRecorder->setOutputLocation(QUrl::fromLocalFile(_folder));


    _audioProbe->setSource(_audioRecorder);
    connect(_audioProbe,SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(audioBufferProbed(QAudioBuffer)));
}

void Recorder::audioBufferProbed(const QAudioBuffer &buffer){
    emit bufferReady(buffer);
}
QString Recorder::device() const
{
    return _device;
}

QString Recorder::codec() const
{
    return _codec;
}

QString Recorder::container() const
{
    return _container;
}

int Recorder::samplerate() const
{
    return _samplerate;
}

QString Recorder::folder() const
{
    return _folder;
}


void Recorder::record(){
    _audioRecorder->record();
}

void Recorder::stop(){
    _audioRecorder->stop();
}

void Recorder::pause(){
    _audioRecorder->pause();
}

int Recorder::state(){
    return _audioRecorder->state();
}
int Recorder::status() {
    return _audioRecorder->status();
}
