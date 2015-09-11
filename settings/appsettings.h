#ifndef APPSETTINGS_H
#define APPSETTINGS_H
#include <QSettings>
#include <QDebug>

#define COMPANY_NAME "mohabouje"
#define APP_NAME    "tfg"
#define KEY_FIRST_RUN "app/firstrun"



// View Configuration
#define KEY_SHOW_ENERGY_CHART "view/showEnergyChart"
#define KEY_SHOW_PITCH_CHART "view/showPitchChart"
#define KEY_SHOW_SPECTROGRAM "view/showSpectrogram"
#define KEY_SHOW_TERMINAL "view/showTerminal"
#define KEY_DEFAULT_PATH "view/defaultpath"

// MainWindow Configuration
#define KEY_MAINWINDOW_SIZE "mainwindow/size"
#define KEY_MAINWINDOW_POS "mainwindow/pos"
#define KEY_MAINWINDOW_FULLSCRENN "mainwindow/fullscreen"
#define KEY_DEFAULT_LANGUGE "mainwindow/language"



class AppSettings
{
public:
    AppSettings();

    void firstRunSettings() const;
    QSettings *currentSettings() const;

private:
    QSettings*  _appSettings;
};

#endif // APPSETTINGS_H
