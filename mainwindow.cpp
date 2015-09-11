#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QAudioFormat>
#include <QAudioBuffer>
#include <QList>
#include <capturedata/util.h>
#include <settings/levelthresholddetector.h>


#define KEY_PROCESS_LEVEL "processingsettings/level"
#define KEY_PROCESS_TOLERANCE "processingsettings/tolerance"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _timer(new QElapsedTimer),
    _elapsedTime(0),
    _progressBar(new QProgressBar(this)),
    _recorder(new Recorder),
    _appSettings(new AppSettings),
    _recordSettingsDialog(new RecorderSettingsDialog),
    _procSettingsDialog(new ProcessSettingsDialog(8000)),
    _chartSettingsDialog(new ChartSettingsDialog),
    _aboutAppDialog(new AboutApp),
    _fileLoaded(false)
{
    ui->setupUi(this);

    _progressBar->setMaximum(BUFFER_TIME_MS);
    _progressBar->setMinimum(0);
    _progressBar->setMaximumWidth(200);
    ui->statusBar->addPermanentWidget(_progressBar);

    _folderName = new QLabel(this);
    ui->statusBar->addWidget(_folderName);

    ui->folderWidget->setParent(ui);
    ui->folderWidget->attachQwtPlot(ui->chartWAVFile);
    ui->folderWidget->controlWidget()->slider()->setValue(_chartSettingsDialog->muLawValue());


    loadSettings();
    configureCharts();
    loadComponents();
    connectGUI();
}


void MainWindow::changeEvent(QEvent* event)
    {
         if(0 != event) {
              switch(event->type()) {
                  case QEvent::LanguageChange:
                        ui->retranslateUi(this);
                        break;
                  case QEvent::LocaleChange:
                        break;
                  default:
                        break;
              }
         }
         QMainWindow::changeEvent(event);

}


void MainWindow::setDefaultLanguage(QString fileName) {
    translator= new QTranslator;
    translator->load(":/"+fileName);
    qApp->removeTranslator(translator);
    qApp->installTranslator(translator);
}


void MainWindow::changeLanguage() {

    QMessageBox::critical(this, tr("Sorry"),
                                   tr("Not implemented yet. Work in progress."),
                                   tr("Accept") );
    return;


    QString language =  "";
    if (QObject::sender() == ui->actionSpanish)
        language = "logo_es.qm";
    else
        language = "logo_en.qm";

    setDefaultLanguage(language);
    _appSettings->currentSettings()->setValue(KEY_DEFAULT_LANGUGE,language);

}


void MainWindow::loadSettings(){
    resize(_appSettings->currentSettings()->value(KEY_MAINWINDOW_SIZE, size()).toSize());
    QPoint point = _appSettings->currentSettings()->value(KEY_MAINWINDOW_POS, pos()).toPoint();
    if (geometry().contains(point))
        move(point);


    QString currentLanguage =  _appSettings->currentSettings()->value(KEY_DEFAULT_LANGUGE, "tfg_es.qm").toString();
    setDefaultLanguage(currentLanguage);
    ui->folderWidget->controlWidget()->toleranceDial()->setValue(_appSettings->currentSettings()->value(KEY_PROCESS_TOLERANCE, 30).toInt());
    ui->folderWidget->controlWidget()->levelDial()->setValue(_appSettings->currentSettings()->value(KEY_PROCESS_LEVEL, -80).toInt());


    ui->actionShowEnergy->setChecked(_appSettings->currentSettings()->value(KEY_SHOW_ENERGY_CHART, true).toBool());
    ui->actionShowPitch->setChecked(_appSettings->currentSettings()->value(KEY_SHOW_PITCH_CHART, true).toBool());
    ui->actionShowSpectrogram->setChecked(_appSettings->currentSettings()->value(KEY_SHOW_SPECTROGRAM, true).toBool());
    ui->actionShowTerminal->setChecked(_appSettings->currentSettings()->value(KEY_SHOW_TERMINAL, true).toBool());

    ui->chartEnergy->setVisible(ui->actionShowEnergy->isChecked());
    ui->chartPitch->setVisible(ui->actionShowPitch->isChecked());
    ui->chartSpectrogram->setVisible(ui->actionShowSpectrogram->isChecked());
    ui->infoPanel->setVisible(ui->actionShowTerminal->isChecked());

}


void MainWindow::saveSettings(){
    _appSettings->currentSettings()->setValue(KEY_MAINWINDOW_SIZE, size());
    _appSettings->currentSettings()->setValue(KEY_MAINWINDOW_POS, pos());
    _appSettings->currentSettings()->setValue(KEY_SHOW_ENERGY_CHART,ui->actionShowEnergy->isChecked());
    _appSettings->currentSettings()->setValue(KEY_SHOW_PITCH_CHART,ui->actionShowPitch->isChecked());
    _appSettings->currentSettings()->setValue(KEY_SHOW_SPECTROGRAM,ui->actionShowSpectrogram->isChecked());
    _appSettings->currentSettings()->setValue(KEY_SHOW_TERMINAL,ui->actionShowTerminal->isChecked());
    _appSettings->currentSettings()->setValue(KEY_PROCESS_TOLERANCE,ui->folderWidget->controlWidget()->toleranceDial()->value());
    _appSettings->currentSettings()->setValue(KEY_PROCESS_LEVEL,ui->folderWidget->controlWidget()->levelDial()->value());

}


// Load components

void MainWindow::loadComponents(){
    _recorder->init();
    _procSettingsDialog->setCurrentSampleRate(_recorder->samplerate());
    loadProcessingComponents();
}

window_type getWindowType(const int& control) {
    switch (control) {
    case 0:
        return window_hamming;
    case 1:
        return window_hanningz;
    case 2:
        return window_blackman;
    case 3:
        return window_blackman_harris;
    case 4:
        return window_rectangle;
    case 5:
        return window_gaussian;
    }
    return window_default;
}

pitch_algorithm getPitchAlgo(const int& control) {
    switch (control) {
    case 0:
        return pitch_algo_autocor;
        break;
    case 1:
        return pitch_algo_amdf;
        break;
    case 2:
        return pitch_algo_cepstrum;
    }
    return pitch_algo_default;
}


void MainWindow::setTolerance(const int& value) {
    logo_set_vad_tolerance(_pitch.vad, (logo_real)value/100);

}

void MainWindow::setLevelThreShold(const int& value){
    logo_set_vad_threshold(_pitch.vad, (logo_real)value);
}


void MainWindow::loadProcessingComponents(){
    int sampleRate;
    if (_fileLoaded) {
        sampleRate = ui->folderWidget->wavReader()->format().sampleRate();
        _procSettingsDialog->setCurrentSampleRate(sampleRate);
    } else
        sampleRate = _recorder->samplerate();
#ifdef Q_OS_WIN
    _tempBuffer = new_vec(BUFFER_TIME * sampleRate);
#endif

#ifdef Q_OS_UNIX
    _tempBuffer = new_vec(0);
    _tempBufferCount = 0;
#endif
#ifdef DEBUG
    _framesRecord.clear();
    _rifBufferFrames.clear();
    _windowSlicer.clear();
#endif

    // Logo Library
    _slicer = new_buffer_slicer(BUFFER_TIME,
                                _procSettingsDialog->overlapTime(),
                                _procSettingsDialog->windowTime(),
                                sampleRate,
                                getWindowType(_procSettingsDialog->currentWindow()));
    logo_set_offset_filter_parameter(_slicer, -0.9998);
    logo_set_preemphasis_filter_parameter(_slicer, -0.97);
    _freq = new_freq_analyzer(sampleRate,
                              _procSettingsDialog->fftsamples(),
                              _procSettingsDialog->orderLPC(),
                              _procSettingsDialog->orderCepstrum(),
                              _procSettingsDialog->orderFilterBanks());
    _pitch =  new_pitch_tracker(getPitchAlgo(_procSettingsDialog->pitchMethod()),
                                _slicer.window_samples,
                                sampleRate,
                                _procSettingsDialog->pitchMin(),
                                _procSettingsDialog->pitchMax());
    logo_set_vad_tolerance(_pitch.vad, ui->folderWidget->controlWidget()->toleranceDial()->value());
    logo_set_vad_threshold(_pitch.vad, ui->folderWidget->controlWidget()->levelDial()->value());
    _data = new_logo_data(_procSettingsDialog->bufferTime(), _slicer, _freq);
    logo_print_buffer_slicer_info(_slicer);


    ui->chartSpectrogram->clearPlottables();
    _spectrogramScale = new QCPColorScale(ui->chartSpectrogram);
    _spectrogramScale->setDataRange(QCPRange(0,1));
    _spectrogramScale->setType(QCPAxis::atRight);
    _spectrogramPlot = new QCPColorMap(ui->chartSpectrogram->xAxis, ui->chartSpectrogram->yAxis);
    if (_procSettingsDialog->fftMethod() != 3)
        _spectrogramPlot->data()->setSize(_data.frames, _data.fft_samples);
    else
        _spectrogramPlot->data()->setSize(_data.frames, _data.filterbank_order);
    _spectrogramPlot->data()->setRange(QCPRange(0,_data.tmax), QCPRange(0,_data.fmax));
    _spectrogramPlot->setColorScale(_spectrogramScale);
    _spectrogramPlot->rescaleDataRange();
    ui->chartSpectrogram->addPlottable(_spectrogramPlot);
    refreshCharts();
}


void MainWindow::matrixToData() {
    logo_matrix matrix;
    if (_procSettingsDialog->fftMethod() != 3)
        matrix  = _data.spectrogram_plot;
    else
        matrix = _data.filterbank_plot;

    double min = 1e6;
    double max = -1e6;
    logo_pair minmax;
    foreach (const logo_vector row, matrix) {
        logo_minmax_vec(row,minmax);
        if (minmax.second > max)
            max = minmax.second;
        if (minmax.first < min)
            min =  minmax.first;
    }

    unsigned int rowCount = matrix.size();
    unsigned int columnCount = matrix[0].size();
    int mu = _chartSettingsDialog->muLawValue();
    double value = 0.0;
    if (mu > 0) {
        double logMu_1 =  log(mu+1);
        for (unsigned int i = 0; i < rowCount; ++i)
            for (unsigned int j = 0; j < columnCount; ++j) {
                value = (matrix[i][j]-min)/(max-min);
                _spectrogramPlot->data()->setCell(i, j, log(1+mu*value)/logMu_1);
            }
    } else if (mu < 0) {
        mu = -mu;
        double logMu_1 =  log(mu+1);
        double inverseMu = (double)(1.0/mu);
        for (unsigned int i = 0; i < rowCount; ++i)
            for (unsigned int j = 0; j < columnCount; ++j) {
                value = (matrix[i][j]-min)/(max-min);
                _spectrogramPlot->data()->setCell(i, j, inverseMu*(exp(logMu_1*value)-1));
            }
    } else {
        for (unsigned int i = 0; i < rowCount; ++i)
            for (unsigned int j = 0; j < columnCount; ++j) {
                value = (matrix[i][j]-min)/(max-min);
                _spectrogramPlot->data()->setCell(i, j, value);
            }
    }
}


void MainWindow::configureCharts(){
    const QColor backgroundColor = this->palette().background().color();
    ui->chartEnergy->setBackground(backgroundColor);
    ui->chartEnergy->axisRect()->setBackground(backgroundColor);
    ui->chartPitch->setBackground(backgroundColor);
    ui->chartPitch->axisRect()->setBackground(backgroundColor);
    ui->chartSpectrogram->setBackground(backgroundColor);
    ui->chartSpectrogram->axisRect()->setBackground(backgroundColor);
    ui->chartEnergy->clearGraphs();
    ui->chartPitch->clearGraphs();
    _energyPlot =  ui->chartEnergy->addGraph();
    _pitchPlot = ui->chartPitch->addGraph();
   // _pitchPlot->setLineStyle(QCPGraph::lsNone);
   // _pitchPlot->setScatterStyle(QCPScatterStyle::ssDisc);
}




void MainWindow::refreshCharts(){


    ui->chartEnergy->xAxis->setTicks(_chartSettingsDialog->energy_X_Axis());
    ui->chartEnergy->yAxis->setTicks(_chartSettingsDialog->energy_Y_Axis());
    ui->chartEnergy->xAxis->grid()->setVisible(_chartSettingsDialog->energy_X_Grid());
    ui->chartEnergy->yAxis->grid()->setVisible(_chartSettingsDialog->energy_Y_Grid());
    ui->chartEnergy->xAxis->setRange(0, _data.tmax);

    ui->chartEnergy->xAxis->setLabel(tr("Time (s)"));
    if (_procSettingsDialog->shortTimeMethod() == 0)
        ui->chartEnergy->yAxis->setLabel(tr("Energy (dB)"));
    else
        ui->chartEnergy->yAxis->setLabel(tr("Normalized AZCR"));


    ui->chartPitch->xAxis->setTicks(_chartSettingsDialog->pitch_X_Axis());
    ui->chartPitch->yAxis->setTicks(_chartSettingsDialog->pitch_Y_Axis());
    ui->chartPitch->xAxis->grid()->setVisible(_chartSettingsDialog->pitch_X_Grid());
    ui->chartPitch->yAxis->grid()->setVisible(_chartSettingsDialog->pitch_Y_Grid());
    ui->chartPitch->xAxis->setRange(0, _data.tmax);

    ui->chartPitch->xAxis->setLabel(tr("Time (s)"));
    ui->chartPitch->yAxis->setLabel(tr("Frequency (Hz)"));

    ui->chartSpectrogram->xAxis->setTicks(_chartSettingsDialog->spectrogram_X_Axis());
    ui->chartSpectrogram->yAxis->setTicks(_chartSettingsDialog->spectrogram_Y_Axis());
    ui->chartSpectrogram->xAxis->setRange(0, _data.tmax);
    ui->chartSpectrogram->yAxis->setRange(0, _data.fmax);

    ui->chartSpectrogram->xAxis->setLabel(tr("Time (s)"));
    ui->chartSpectrogram->yAxis->setLabel(tr("Frequency (Hz)"));


    switch (_chartSettingsDialog->spectrogram_colormap()){
    case 0:
        _spectrogramPlot->setGradient(QCPColorGradient::gpSpectrum);
        break;
    case 1:
        _spectrogramPlot->setGradient(QCPColorGradient::gpJet);
        break;
    case 2:
        _spectrogramPlot->setGradient(QCPColorGradient::gpGrayscale);
        break;
    case 3:
        _spectrogramPlot->setGradient(QCPColorGradient::gpHot);
        break;
    case 4:
        _spectrogramPlot->setGradient(QCPColorGradient::gpCold);
        break;
    case 5:
        _spectrogramPlot->setGradient(QCPColorGradient::gpHues);
        break;
    default:
        _spectrogramPlot->setGradient(QCPColorGradient::gpSpectrum);
        break;
    }

    ui->chartEnergy->replot();
    ui->chartPitch->replot();
    ui->chartSpectrogram->replot();
}

void MainWindow::fileLoaded(const bool& state) {
    if (!state)
        return;
    if (_recorder->state() == QAudioRecorder::RecordingState)
        record();
    _fileLoaded = true;
    ui->actionPlay->setIcon(QIcon(":resources/icons/play.png"));
    restartPlot();
    loadProcessingComponents();
}

void MainWindow::connectGUI(){
    connect(ui->folderWidget, SIGNAL(showStatusMesasge(QString)), this, SLOT(showStatusMessage(QString)));
    connect(ui->folderWidget,SIGNAL(restartPlot()),this,SLOT(restartPlot()));
    connect(ui->folderWidget->wavReader(),SIGNAL(bufferReady(QAudioBuffer)),this, SLOT(process(QAudioBuffer)));
    connect(ui->folderWidget,SIGNAL(enablePlayButton(bool)),ui->actionPlay,SLOT(setEnabled(bool)));
    connect(ui->folderWidget, SIGNAL(enablePlayButton(bool)), this, SLOT(fileLoaded(bool)));
#ifdef DEBUG
    connect(ui->folderWidget,SIGNAL(idleState()),this, SLOT(exportDebugData()));
#endif
    connect(ui->actionOpen,SIGNAL(triggered(bool)), ui->folderWidget,SLOT(openFolder()));
    connect(ui->actionPlay,SIGNAL(triggered(bool)),this,SLOT(play()));
    connect(ui->actionRecord, SIGNAL(triggered(bool)), this, SLOT(record()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(open()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(ui->actionPrint, SIGNAL(triggered(bool)), this, SLOT(print()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(exit()));
    connect(ui->actionProcessSettings, SIGNAL(triggered(bool)), this, SLOT(processTools()));
    connect(ui->actionRecordSettings, SIGNAL(triggered(bool)), this, SLOT(recordTools()));
    connect(ui->actionChartSettings, SIGNAL(triggered(bool)), this, SLOT(chartTools()));
    connect(ui->actionFull_Screen,SIGNAL(triggered(bool)),this, SLOT(toggleFullScreen()));
    connect(ui->actionAbout_TFG,SIGNAL(triggered(bool)),this, SLOT(aboutApp()));
    connect(ui->actionAbout_Qt,SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    connect(ui->actionShowEnergy,SIGNAL(changed()), this, SLOT(changeView()));
    connect(ui->actionShowEnergy,SIGNAL(triggered(bool)), ui->chartEnergy, SLOT(setVisible(bool)));
    connect(ui->actionShowPitch,SIGNAL(changed()), this, SLOT(changeView()));
    connect(ui->actionShowPitch,SIGNAL(triggered(bool)), ui->chartPitch, SLOT(setVisible(bool)));
    connect(ui->actionShowSpectrogram,SIGNAL(changed()), this, SLOT(changeView()));
    connect(ui->actionShowSpectrogram,SIGNAL(triggered(bool)), ui->chartSpectrogram, SLOT(setVisible(bool)));
    connect(ui->actionShowTerminal,SIGNAL(changed()), this, SLOT(changeView()));
    connect(ui->actionPreview,SIGNAL(triggered(bool)), this, SLOT(overView()));
    connect(_recorder,SIGNAL(bufferReady(QAudioBuffer)),this,SLOT(process(QAudioBuffer)));
    connect(ui->chartEnergy, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showChartContexMenu(QPoint)));
    connect(ui->chartPitch, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showChartContexMenu(QPoint)));
    connect(ui->chartSpectrogram, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showChartContexMenu(QPoint)));
    connect(ui->actionEstimate_Levels,SIGNAL(triggered(bool)), this, SLOT(estimateLevels()));

    connect(_chartSettingsDialog->slider(), SIGNAL(valueChanged(int)),ui->folderWidget->controlWidget()->slider() , SLOT(setValue(int)));
    connect(ui->folderWidget->controlWidget()->slider(), SIGNAL(valueChanged(int)), _chartSettingsDialog->slider(), SLOT(setValue(int)));
    connect(ui->folderWidget->controlWidget()->toleranceDial(), SIGNAL(valueChanged(int)), this, SLOT(setTolerance(int)));
    connect(ui->folderWidget->controlWidget()->levelDial(), SIGNAL(valueChanged(int)), this, SLOT(setLevelThreShold(int)));
    connect(ui->folderWidget->controlWidget()->slider(), SIGNAL(valueChanged(int)), this, SLOT(updateSpectrogramChart()));

    connect(ui->actionSpanish, SIGNAL(triggered(bool)), this, SLOT(changeLanguage()));
    connect(ui->actionEnglish, SIGNAL(triggered(bool)), this, SLOT(changeLanguage()));

}

void MainWindow::updateSpectrogramChart() {
    if (_recorder->state() == QAudioRecorder::RecordingState || ui->folderWidget->wavReader()->state() == QAudio::ActiveState)
        return;
    matrixToData();
    ui->chartSpectrogram->replot();
}



MainWindow::~MainWindow() {
    delete      ui;
}


void MainWindow::changeRecorderButtonIcon() {
    if (_recorder->state() == QAudioRecorder::RecordingState)
        ui->actionRecord->setIcon(QIcon(":resources/icons/stop_record.png"));
    else
        ui->actionRecord->setIcon(QIcon(":resources/icons/record.png"));
}



void MainWindow::checkRecorderStatus() {
    int status = _recorder->state();
    switch (status) {
    case QMediaRecorder::UnavailableStatus:
        QMessageBox::critical(this, tr("Error"),
                                       tr("The recorder is not available or not supported. Maybe your device settings may be blocking the microphone. It may be necessary to adjust Firewall settings to allow access."),
                                       tr("Accept") );
        break;
    }

}

void MainWindow::record(){
    if (_fileLoaded) {
        _fileLoaded = false;
        loadComponents();
    }
    int state = _recorder->state();
    switch(state){
        case QAudioRecorder::RecordingState:
            _recorder->stop();
#ifdef DEBUG
            exportDebugData();
#endif
            break;
        case QAudioRecorder::StoppedState:
            if (ui->folderWidget->wavReader()->state() == QAudio::ActiveState) {
                ui->actionPlay->setIcon(QIcon(":resources/icons/play.png"));
                ui->folderWidget->restartWavReader();
            }
            restartPlot();
            _recorder->record();
            checkRecorderStatus();
            break;
        case QAudioRecorder::PausedState:
            if (ui->folderWidget->wavReader()->state() == QAudio::ActiveState) {
                ui->actionPlay->setIcon(QIcon(":resources/icons/play.png"));
                ui->folderWidget->restartWavReader();
            }
            restartPlot();
            _recorder->record();
            break;
        default:
            break;
    }
    changeRecorderButtonIcon();
}


void MainWindow::play(){
    if (!_fileLoaded)
        return;
    QAudio::State state = ui->folderWidget->wavReader()->state();
    if (state == QAudio::ActiveState) {
        ui->actionPlay->setIcon(QIcon(":resources/icons/play.png"));
        ui->folderWidget->restartWavReader();
    } else if(state == QAudio::IdleState) {
        ui->folderWidget->restartWavReader();
        ui->actionPlay->setIcon(QIcon(":resources/icons/stop.png"));
        restartPlot();
        if (_recorder->state() == QAudioRecorder::RecordingState)
            record();
        ui->folderWidget->wavReader()->play();
    } else {
        ui->actionPlay->setIcon(QIcon(":resources/icons/stop.png"));
        restartPlot();
        if (_recorder->state() == QAudioRecorder::RecordingState)
            record();
        ui->folderWidget->wavReader()->play();
    }
}


void MainWindow::processTools(){
    int res = _procSettingsDialog->exec();
    if (res == QDialog::Accepted)
        loadProcessingComponents();

}

void MainWindow::recordTools(){
    int state = _recorder->state();
    if (state == QAudioRecorder::RecordingState)
        record();


    int res = _recordSettingsDialog->exec();
    if (res == QDialog::Accepted)
        loadComponents();


}

void MainWindow::chartTools(){
    int res = _chartSettingsDialog->exec();
    if (res == QDialog::Accepted)
        refreshCharts();

}


void MainWindow::restartPlot(){
    _energyPlot->clearData();
    _pitchPlot->clearData();

    ui->chartEnergy->replot();
    ui->chartPitch->replot();
    ui->chartSpectrogram->replot();
}






void MainWindow::process(const QAudioBuffer& buffer){
    _timer->restart();
    const qint16 *data = buffer.data<qint16>();
    const qreal max = getPeakValue(buffer.format());

#ifdef Q_OS_WIN
        for (int i=0;i <buffer.frameCount();i++)
           _tempBuffer[i] = data[i]/max;
#endif
#ifdef Q_OS_UNIX
        //Buffer size of 10ms, required 40 -> Lets store it
        logo_vector temp = new_vec(buffer.frameCount());
        for (int i=0;i <buffer.frameCount();i++) {
           temp[i] = data[i]/max;
        }
        _tempBuffer += (temp);
        if (_tempBufferCount == 3) {
            _tempBufferCount = 0;
        } else {
            _tempBufferCount++;
            return;
        }
#endif


        logo_apply_buffer_slicer(_slicer, _tempBuffer, _data.temp_frames);
        shortTimeAnalysis();
        frequencyAnalysis();
        _elapsedTime = _timer->elapsed();
        _progressBar->setValue((_elapsedTime > BUFFER_TIME_MS) ? BUFFER_TIME_MS : _elapsedTime );
        showStatusMessage(tr("%1 ms of 40 ms").arg(_elapsedTime));
#ifdef DEBUG
        _framesRecord.append(_tempBuffer);
        _rifBufferFrames.append(_slicer.ring_buffer);
        _windowSlicer.append(_data.temp_frames);
#endif

#ifdef Q_OS_UNIX
        _tempBuffer.clear();
#endif
}


logo_vector smooth_savitzky_golay_filter(const logo_vector& frame){
    logo_real result = 0;
    logo_vector smooth = new_vec(frame.size());
    for (int i=0; i< frame.length();i++) {
        result = 17.0 * frame[i];
        if (i-2 > 0) result -= 3.0*frame[i-2];
        if (i-1 > 0) result -= 12.0*frame[i-1];
        if (i+1 < frame.length()) result += 12*frame[i+1];
        if (i+2 < frame.length()) result += 12*frame[i+2];
        smooth[i] = 1/35.0 * result;
    }
    return smooth;
}



void MainWindow::shortTimeAnalysis() {

    if (ui->actionShowEnergy->isChecked()) {
        switch (_procSettingsDialog->shortTimeMethod()) {
        case 0:
            logo_frames_energy_db_spl(_data.temp_frames, _data.temp_result);
            break;
        case 1:
            logo_frames_zero_crossing_rate(_data.temp_frames, _data.temp_result);
            break;
        }
        logo_plot_concatenate(_data.time_plot, _data.temp_result);
        _energyPlot->setData(_data.time_plot.x,_data.time_plot.y);
        ui->chartEnergy->yAxis->setRange(QCPRange(_data.time_plot.min, _data.time_plot.max));
        ui->chartEnergy->replot();
    }

    if (ui->actionShowPitch->isChecked()){
        switch (_procSettingsDialog->pitchMethod()) {
        case 0:
            logo_frames_pitch_tracker_acf(_pitch, _data.temp_frames, _data.temp_result);
            break;
        case 1:
            logo_frames_pitch_tracker_amdf(_pitch, _data.temp_frames, _data.temp_result);
            break;
        case 2:
            logo_frames_pitch_tracker_cepstrum(_freq.fft, _pitch, _data.temp_frames, _data.temp_result);
            break;
        }
        logo_plot_concatenate(_data.pitch_plot, _data.temp_result);
        _pitchPlot->setData(_data.pitch_plot.x, smooth_savitzky_golay_filter(_data.pitch_plot.y));
        ui->chartPitch->yAxis->setRange(QCPRange(_data.pitch_plot.min, _data.pitch_plot.max));
        ui->chartPitch->replot();
    }
}

void MainWindow::frequencyAnalysis(){
    if (ui->actionShowSpectrogram->isChecked()) {
        const int fftMethod = _procSettingsDialog->fftMethod();
        switch (fftMethod) {
        case 0: default:
            logo_frames_fft_psd(_freq, _data.temp_frames, _data.temp_spectrum);
            logo_concatenate_matrix(_data.spectrogram_plot, _data.temp_spectrum);
            break;
        case 1:
            logo_frames_fft_lpc(_freq, _data.temp_frames, _data.temp_spectrum);
            logo_concatenate_matrix(_data.spectrogram_plot, _data.temp_spectrum);
            break;
        case 2:
            logo_frames_fft_ceps(_freq, _data.temp_frames, _data.temp_spectrum);
            logo_concatenate_matrix(_data.spectrogram_plot, _data.temp_spectrum);
            break;
        case 3:
            logo_frames_fft_filterbank(_freq,_data.temp_frames, _data.temp_fb_spectrum);
            logo_concatenate_matrix(_data.filterbank_plot, _data.temp_fb_spectrum);
            break;
        }
        matrixToData();
        ui->chartSpectrogram->replot();
    }

}




// GUI SLOTS
void MainWindow::estimateLevels() {
    if (_recorder->state() == QAudioRecorder::RecordingState)
        record();

    LevelThreSholdDetector* _detector = new LevelThreSholdDetector(this);
    int res = _detector->exec();
    if (res != QDialog::Accepted)
        return;
    ui->folderWidget->controlWidget()->toleranceDial()->setValue(_detector->getTempAZCR()*100);
    ui->folderWidget->controlWidget()->levelDial()->setValue(_detector->getTempEnergy());
}



void MainWindow::aboutApp(){
    _aboutAppDialog->exec();
}

void MainWindow::toggleFullScreen(){
    setWindowState(windowState() ^ Qt::WindowFullScreen);
}



void MainWindow::save(){

}


void MainWindow::open(){

}

void MainWindow::overView() {
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowFlags ( Qt::Window );
    connect(&preview, SIGNAL(paintRequested(QPrinter *)),this, SLOT(preview(QPrinter*)));

    preview.exec();
}

void MainWindow::preview(QPrinter* printer) {
#ifndef QT_NO_PRINTER
    ui->chartEnergy->savePng(QDir::currentPath()+"/energy.png",  0, 0, 1.0, -1);
    ui->chartPitch->savePng(QDir::currentPath()+"/pitch.png",  0, 0, 1.0, -1);
    ui->chartSpectrogram->savePng(QDir::currentPath()+"/spectrum.png",  0, 0, 1.0, -1);
    QTextDocument doc;
    doc.setHtml(
             "<p><img height=\"200\" width=\"700\" src=\"energy.png\" alt=\"Smiley face\" ></p>"
             "<p><img height=\"200\" width=\"700\" src=\"pitch.png\" alt=\"Smiley face\" ></p>"
             "<p><img  height=\"200\" width=\"700\" src=\"spectrum.png\" alt=\"Smiley face\" ></p>"
             );
    doc.print(printer);
    printer->newPage();
#endif
}



void MainWindow::print(){
    QPrinter printer;
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;
    ui->chartEnergy->savePng(QDir::currentPath()+"/energy.png",  0, 0, 1.0, -1);
    ui->chartPitch->savePng(QDir::currentPath()+"/pitch.png",  0, 0, 1.0, -1);
    ui->chartSpectrogram->savePng(QDir::currentPath()+"/spectrum.png",  0, 0, 1.0, -1);
    QTextDocument doc;
    doc.setHtml(
                "<p><font size=+2>" + tr("Audio Settings")+ "</font></p>"
                "<p><b>" + tr("Device: ")+ "</b>" + _recorder->device()+
                "</p>"
                "<p><b>" + tr("Codec: ") +  "</b>" + _recorder->codec() +
                "</p>"
                "<p><b>" + tr("Samplerate:")+ "</b>" + QString::number(_freq.sample_rate)+
                "  Hz</p>"
                "<p><b>" + tr("Container: ")+ "</b> WAV "
                "</p>"
                "<p><font size=+2>" + tr("Processing Settings") + "</font></p>"
                "<p><font size=+1>" + tr("Short Time Analysis") + "</font></p>"
                "<p><b>" + tr("Window Time: ") +  "</b>" + QString::number(_slicer.window_time)+
                " ms</p>"
                "<p><b>" + tr("Hope Time: ") +  "</b>" + QString::number(_slicer.hope_time)+
                " ms</p>"
                "<p><b>" + tr("Buffer Time: ") +  "</b>" + QString::number(_procSettingsDialog->bufferTime())+
                " s</p>"

                "<p><font size=+1>" + tr("Frequency Analysis") + "</font></p>"
                "<p><b>" + tr("LPC order: ") +  "</b>" + QString::number(_freq.lpc_order)+
                "</p>"
                "<p><b>" + tr("Cepstrum order: ") +  "</b>" + QString::number(_freq.ceps_order)+
                "</p>"
                "<p><b>" + tr("Filter Banks: ") +  "</b>" + QString::number(_freq.fbank_order)+
                "</p>"
                "<p><font size=+3>" + tr("Results") + "</font></p>"
                "<p><img height=\"200\" width=\"700\" src=\"energy.png\" alt=\"Smiley face\" ></p>"
                "<p><img height=\"200\" width=\"700\" src=\"pitch.png\" alt=\"Smiley face\" ></p>"
                "<p><img  height=\"200\" width=\"700\" src=\"spectrum.png\" alt=\"Smiley face\" ></p>"
                );
     doc.print(&printer);
}


void MainWindow::exit(){
    close();
}



void MainWindow::changeView(){
    QAction *sender =  qobject_cast<QAction *>(QObject::sender());
    sender->triggered(!sender->isChecked());
    if (sender == ui->actionShowTerminal)
        ui->infoPanel->setVisible(sender->isChecked());
}



void MainWindow::closeEvent(QCloseEvent *event){
    int state = _recorder->state();
    if (state == QAudioRecorder::RecordingState)
        record();

    saveSettings();
    ui->folderWidget->saveSettings();
    event->accept();
}


void MainWindow::showStatusMessage(const QString& mess){
   _folderName->setText(mess);
}

#ifdef DEBUG
void MainWindow::exportDebugData() {
    qDebug() << "Exporting debug data";
    std::ofstream out("framesRecorded.txt");
    foreach (const logo_vector vect, _framesRecord) {
        foreach (const logo_real value, vect) {
            out << value << " ";
        }
        out << std::endl;
    }
    out.close();

    std::ofstream out_2("rifBuffer.txt");
    foreach (const logo_vector vect, _rifBufferFrames) {
        foreach (const logo_real value, vect) {
            out_2 << value << " ";
        }
        out_2 << std::endl;
    }
    out_2.close();

    std::ofstream out_3("window.txt");
    foreach (const logo_matrix matrix, _windowSlicer) {
        foreach (const logo_vector vector, matrix) {
            foreach (const logo_real value, vector) {
                out_3 << value << " ";

            }
            out_3 << std::endl;
        }
    }
    out_3.close();
}
#endif



void MainWindow::showChartContexMenu(const QPoint& pos){

   QCustomPlot* sender = static_cast<QCustomPlot*>(QObject::sender());
   QPoint globalPos = sender->mapToGlobal(pos);

   QMenu* menu = new QMenu(this);
   QAction* copyChart = menu->addAction(tr("Copy"));
   copyChart->setIcon(QIcon(":resources/icons/copy.png"));
   QAction* exportChart = menu->addAction(tr("Export"));
   exportChart->setIcon(QIcon(":resources/icons/save.png"));
   QAction* actionClicked = menu->exec(globalPos);
   if (!actionClicked)
       return;

    if (actionClicked == copyChart) {
        sender->savePng(QDir::currentPath()+"/temp.png",  0, 0, 1.0, -1);
        QApplication::clipboard()->setPixmap(QPixmap(QDir::currentPath()+"/temp.png"));
    } else {
       QString fileName=QFileDialog::getSaveFileName(this,tr("Export Chart As File"),".",tr("JPEG (*.jpg)\nBMP (*.bmp)\nPNG (*.png)"));
       if(!fileName.isEmpty()){
           QFileInfo f(fileName);
           QString sufix=f.suffix();

           if(sufix=="jpg"){
                sender->saveJpg(fileName,  0, 0, 1.0, -1);
           }else if(sufix=="png"){
                sender->savePng(fileName,  0, 0, 1.0, -1);
           }else if(sufix=="bmp"){
                sender->saveBmp(fileName,  0, 0, 1.0);
           }
       }
    }
}










