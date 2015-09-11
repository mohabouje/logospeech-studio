#ifndef PROCESSSETTINGSDIALOG_H
#define PROCESSSETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QComboBox>

#define KEY_PROCESS_CURRENT_TAB "processingsettings/currentTab"


#define KEY_PROCESS_DC "processingsettings/activedcfilter"
#define KEY_PROCESS_EMPHASIS "processingsettings/activepreemphasisfilter"
#define KEY_PROCESS_WINDOW "processingsettings/currentwindow"

#define KEY_PROCESS_WINDOW_TIME "processingsettings/windowtime"
#define KEY_PROCESS_WINDOW_SHIFT_TIME "processingsettings/windowshifttime"
#define KEY_PROCESS_BUFFER_TIME "processingsettings/buffertime"
#define KEY_PROCESS_METHOD "processingsettings/method"
#define KEY_PROCESS_PITCH_METHOD "processingsettings/pitchmehod"
#define KEY_PROCESS_PITCH_MIN "processingsettings/pitchmin"
#define KEY_PROCESS_PITCH_MAX "processingsettings/pitchmax"


#define KEY_PROCESS_FFT_METHOD "processingsettings/fftmethod"
#define KEY_PROCESS_FFT_SAMPLES "processingsettings/fftsamples"
#define KEY_PROCESS_ORDER_LPC "processingsettings/orderlpc"
#define KEY_PROCESS_ORDER_CEPSTRUM "processingsettings/ordercepstrum"
#define KEY_PROCESS_ORDER_FILTERBANKS "processingsettings/orderfilterbanks"


#define FILTERBANK_VALUE 3
namespace Ui {
class ProcessSettingsDialog;
}

class ProcessSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessSettingsDialog(const int& currentSampleRate, QWidget *parent = 0);
    ~ProcessSettingsDialog();

    int orderFilterBanks() const;
    int orderCepstrum() const;
    int orderLPC() const;
    int fftsamples() const;
    int fftMethod() const;
    int pitchMax() const;
    int pitchMin() const;
    int pitchMethod() const;
    int bufferTime() const;
    double overlapTime() const;
    double windowTime() const;
    int currentWindow() const;
    bool isPE() const;
    bool isDC() const;
    int shortTimeMethod() const;
    void setCurrentSampleRate(int currentSampleRate);

private slots:
    void restore();
    void save();
    void showWindow();
    void samplesFFTChanged(const int &value);
    void windowTimeChanged(const double &value);
    void setPitchMethod(const int &value);
    void setFFTMethod(const int &value);
private:
    Ui::ProcessSettingsDialog *ui;
    void loadFromSettings();

    bool            _isDC;
    bool            _isPE;
    int             _currentWindow;
    double          _windowTime;
    double          _overlapTime;
    int             _bufferTime;
    int             _shortTimeMethod;
    int             _pitchMethod;
    int             _pitchMin;
    int             _pitchMax;
    int             _fftMethod;
    int             _fftsamples;
    int             _orderLPC;
    int             _orderCepstrum;
    int             _orderFilterBanks;
    int             _currentSampleRate;
};

#endif // PROCESSSETTINGSDIALOG_H
