#include "appsettings.h"
#include <QDir>
AppSettings::AppSettings():
    _appSettings(new QSettings(COMPANY_NAME, APP_NAME))
{
    if(!_appSettings->contains(KEY_FIRST_RUN))
        firstRunSettings();
}



void AppSettings::firstRunSettings() const{

    qDebug() << "Define first run configuration";
    _appSettings->setValue(KEY_FIRST_RUN,true);
    _appSettings->setValue(KEY_SHOW_ENERGY_CHART,true);
    _appSettings->setValue(KEY_SHOW_PITCH_CHART,true);
    _appSettings->setValue(KEY_SHOW_SPECTROGRAM,true);
    _appSettings->setValue(KEY_SHOW_TERMINAL,true);
    _appSettings->setValue(KEY_DEFAULT_PATH,QDir::currentPath());

}


QSettings* AppSettings::currentSettings() const{
    return _appSettings;
}
