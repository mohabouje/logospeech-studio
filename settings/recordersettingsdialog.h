#ifndef RECORDERSETTINGSDIALOG_H
#define RECORDERSETTINGSDIALOG_H

#include <QDialog>
#include <QAudioRecorder>
#include "appsettings.h"
#include <QDir>
#include <QFileDialog>

#define KEY_AUDIO_DEVICE "recordersettings/device"
#define KEY_AUDIO_CODEC "recordersettings/codec"
#define KEY_AUDIO_CONTAINER "recordersettings/container"
#define KEY_AUDIO_SAMPLERATE "recordersettings/samplerate"
#define KEY_AUDIO_FOLDER "recordersettings/folder"

#ifdef Q_OS_UNIX
    #define DEFAULT_SAMPLERATE 8000
#endif

namespace Ui {
class RecorderSettingsDialog;
}

class RecorderSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecorderSettingsDialog(QWidget *parent = 0);
    ~RecorderSettingsDialog();

private slots:
    void changeFolder();
    void save();
    void restore();
private:
    Ui::RecorderSettingsDialog*     ui;
    QAudioRecorder*                 audioRecorder;
    void initComponents();
    void loadFromSettings();
};

#endif // RECORDERSETTINGSDIALOG_H
