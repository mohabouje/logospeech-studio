#include "wavreader.h"
#include <QDebug>


WavReader::WavReader(QObject *parent) :
    QObject(parent),
    _audioOutputDevice(QAudioDeviceInfo::defaultOutputDevice()),
    _audioOutput(0),
    _playPosition(0),
    _bufferPosition(0),
    _bufferLength(0),
    _dataLength(0),
    _headerLength(0)

{

}

WavReader::WavReader(QObject *parent, const QAudioDeviceInfo& audioOutputDevice) :
    QObject(parent),
    _audioOutputDevice(audioOutputDevice),
    _audioOutput(0),
    _playPosition(0),
    _bufferPosition(0),
    _bufferLength(0),
    _dataLength(0)

{

}


bool isPCM(const QAudioFormat &format)
{
    return (format.codec() == "audio/pcm");
}


bool isPCMS16LE(const QAudioFormat &format)
{
    return isPCM(format) &&
           format.channelCount() == 1 &&
           format.sampleType() == QAudioFormat::SignedInt &&
           format.sampleSize() == 16 &&
           format.byteOrder() == QAudioFormat::LittleEndian;
}



void WavReader::setNotifyInterval(const int& ms) {
    _audioOutput->setNotifyInterval(ms);
}


void WavReader::play(){
    if (!_file)
        return;
    if (!isPCMS16LE(_file->fileFormat()))
        return;
    if (!_audioOutput)
        return ;
    _audioOutput->start(_file);
}

void WavReader::stop(){
    _audioOutput->stop();

}

void WavReader::release(){
    _audioOutput->reset();
    delete _audioOutput;
    _audioOutput = 0;
}

QAudio::State WavReader::state(){
    if (_audioOutput)
        return _audioOutput->state();
    else
        return QAudio::IdleState;
}

QAudioFormat WavReader::format() const
{
    return _format;
}

QAudioOutput *WavReader::audioOutput() const
{
    return _audioOutput;
}


int WavReader::setSource(const QString& fileName){
    _file = new WavFile(this);
    if (_file->open(fileName)) {
        if (isPCMS16LE(_file->fileFormat())){
                _fileName = fileName;
                _bufferLength = _file->size();
                _format = _file->fileFormat();
                _headerLength = _file->headerLength();
                _audioOutput = new QAudioOutput(_audioOutputDevice, _format, this);
                connect(_audioOutput, SIGNAL(notify()), this, SLOT(audioNotify()));
                connect(_audioOutput,SIGNAL(stateChanged(QAudio::State)),this, SIGNAL(stateChangeg(QAudio::State)));

                _analysisFile = new WavFile(this);
                _analysisFile->open(fileName);
               return Succes;
        } else {
             _fileName = "Error";
             return FileFormatNotSupported;
        }
    } else {
        _fileName = "Error";
        return FileNotFound;
    }

}


qint64 audioLength(const QAudioFormat &format, qint64 microSeconds)
{
   qint64 result = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
       * microSeconds / 1000000;
   result -= result % (format.channelCount() * format.sampleSize());
   return result;
}

void WavReader::readAll(){
    if (_analysisFile->seek(_headerLength)){
        _buffer.resize(_bufferLength - _headerLength);
        _dataLength = _analysisFile->read(_buffer.data(),_buffer.size());
        QAudioBuffer audioBuffer = QAudioBuffer(_buffer,_format,-1);
        const qint16 *data = audioBuffer.data<qint16>();
        QVector<double> readAll(audioBuffer.sampleCount(),0);
        for(int i=0;i<audioBuffer.sampleCount();i++){
            readAll[i]=(double)(data[i]);
        }
        emit readAllData(readAll);
    }
}



void WavReader::audioNotify(){
    const qint64 endBufferPosition = audioLength(_format, _audioOutput->processedUSecs());
    const qint64 _readLength = _audioOutput->periodSize();
    const qint64 startBufferPosition = endBufferPosition - _readLength;
    if (_file) {
        if (_analysisFile->seek(_headerLength + startBufferPosition) && endBufferPosition < _bufferLength) {
            _buffer.resize(_readLength);
            _dataLength = _analysisFile->read(_buffer.data(), _readLength);
            if (_dataLength == _readLength){
                QAudioBuffer audioBuffer = QAudioBuffer(_buffer,_format,-1);
                emit bufferReady(audioBuffer);
                emit processedUSecs(_audioOutput->processedUSecs());
            } else {
                qDebug() << "Problem with audio buffer";
            }
        } else {
            qDebug() << "Elapsed buffer position";
        }
    } else {
        qDebug() << "File not found!";
    }

}
QString WavReader::fileName() const
{
    return _fileName;
}

