#ifndef RECORDER_H
#define RECORDER_H

#include <QAudioProbe>
#include <QAudioRecorder>
#include <QObject>

#include <settings/appsettings.h>

class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject *parent = 0);
    void init();
    QString folder() const;
    int samplerate() const;
    QString container() const;
    QString codec() const;
    QString device() const;


    int status();
signals:
    void bufferReady(const QAudioBuffer &buffer);
public slots:

    void record();
    void stop();
    void pause();
    int state();
private slots:
    void audioBufferProbed(const QAudioBuffer &buffer);
private:

    QString             _device;
    QString             _codec;
    QString             _container;
    int                 _samplerate;
    QString             _folder;
    QAudioRecorder*     _audioRecorder;
    QAudioProbe*        _audioProbe;
    AppSettings*         _appSettings;
};

#endif // RECORDER_H
