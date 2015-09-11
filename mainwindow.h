#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QObject>
#include <QCloseEvent>
#include <QAudioBuffer>
#include <QLabel>
#include <QElapsedTimer>
#include <QSlider>
#include <QProgressBar>

#include <settings/appsettings.h>
#include <settings/chartsettingsdialog.h>
#include <settings/processsettingsdialog.h>
#include <settings/recordersettingsdialog.h>
#include <capturedata/recorder.h>
#include <logo/logo_structs.h>
#include <logo/logo_preprocessing.h>
#include <logo/logo_sta.h>
#include <logo/logo_util.h>
#include <customwidget/aboutapp.h>


#include "qcustomplot.h"

#define BUFFER_TIME 0.04
#define BUFFER_TIME_MS 40

//#define DEBUG

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setDefaultLanguage(QString fileName);
    void changeEvent(QEvent *event);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void save();
    void open();
    void print();
    void exit();
    void record();
    void processTools();
    void recordTools();
    void chartTools();
    void aboutApp();
    void toggleFullScreen();
    void changeView();
    void process(const QAudioBuffer &buffer);
    void showStatusMessage(const QString &mess);
    void restartPlot();
    void play();
    void fileLoaded(const bool &state);

#ifdef DEBUG
    void exportDebugData();
#endif

    void showChartContexMenu(const QPoint &pos);
    void overView();
    void preview(QPrinter *printer);


    void setLevelThreShold(const int &value);
    void setTolerance(const int &value);
    void estimateLevels();
    void updateSpectrogramChart();
    void changeLanguage();
private:
    Ui::MainWindow *ui;
    void connectGUI();
    void saveSettings();
    void loadSettings();
    void loadComponents();
    void loadProcessingComponents();
    void refreshCharts();
    void configureCharts();
    void normalizeSpectrogram();
    void shortTimeAnalysis();
    void frequencyAnalysis();
    void changeRecorderButtonIcon();
    void checkRecorderStatus();
    void matrixToData();

    // Status bar
    QLabel*                     _folderName;
    QElapsedTimer*              _timer;
    qlonglong                   _elapsedTime;
    QProgressBar*               _progressBar;


    // Record audio
    Recorder*                   _recorder;

    // Settings Dialog
    AppSettings*                _appSettings;
    RecorderSettingsDialog*     _recordSettingsDialog;
    ProcessSettingsDialog*      _procSettingsDialog;
    ChartSettingsDialog*        _chartSettingsDialog;
    AboutApp*                   _aboutAppDialog;


    // Chart
    QCPGraph*                   _energyPlot;
    QCPGraph*                   _pitchPlot;
    QCPColorScale*              _spectrogramScale;
    QCPColorMap*                _spectrogramPlot;


    // Logo Library
    logo_data                   _data;
    logo_slicer                 _slicer;
    logo_freq_analyzer          _freq;
    logo_pitch                  _pitch;



    logo_vector                 _tempBuffer;
#ifdef Q_OS_UNIX
    int                         _tempBufferCount;
#endif

    bool                        _fileLoaded;


    QTranslator* translator;


#ifdef DEBUG
    QList<logo_vector>                 _framesRecord;
    QList<logo_vector>                 _rifBufferFrames;
    QList<logo_matrix>                 _windowSlicer;
#endif
};

#endif // MAINWINDOW_H
