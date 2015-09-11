#-------------------------------------------------
#
# Project created by QtCreator 2015-06-28T16:14:34
#
#-------------------------------------------------

QT          +=  core gui
QT          +=  multimedia
QT          +=  printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TFG
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    settings/appsettings.cpp \
    settings/chartsettingsdialog.cpp \
    settings/processsettingsdialog.cpp \
    settings/recordersettingsdialog.cpp \
    capturedata/recorder.cpp \
    capturedata/wavfile.cpp \
    capturedata/wavreader.cpp \
    customwidget/folderlistview.cpp \
    customwidget/folderwidget.cpp \
    logo/logo_preprocessing.cpp \
    logo/logo_sta.cpp \
    logo/logo_util.cpp \
    logo/logo_structs.cpp \
    customwidget/aboutapp.cpp \
    customwidget/controlwidget.cpp \
    settings/levelthresholddetector.cpp \
    capturedata/util.cpp


HEADERS  += mainwindow.h \
    qcustomplot.h \
    settings/appsettings.h \
    settings/chartsettingsdialog.h \
    settings/processsettingsdialog.h \
    settings/recordersettingsdialog.h \
    capturedata/recorder.h \
    capturedata/wavfile.h \
    capturedata/wavreader.h \
    customwidget/folderlistview.h \
    customwidget/folderwidget.h \
    logo/logo_preprocessing.h \
    logo/logo_sta.h \
    logo/logo_util.h \
    logo/logo_structs.h \
    customwidget/aboutapp.h \
    customwidget/controlwidget.h \
    settings/levelthresholddetector.h \
    capturedata/util.h


FORMS    += mainwindow.ui \
    customwidget/folderlistview.ui \
    customwidget/folderwidget.ui \
    settings/chartsettingsdialog.ui \
    settings/processsettingsdialog.ui \
    settings/recordersettingsdialog.ui \
    customwidget/aboutapp.ui \
    customwidget/controlwidget.ui \
    settings/levelthresholddetector.ui


RESOURCES += \
    icons.qrc


win32 {
    LIBS += -L$$PWD/../../../../FFTW/ -lfftw3-3
    INCLUDEPATH += $$PWD/../../../../FFTW
    DEPENDPATH += $$PWD/../../../../FFTW
}

unix {
    LIBS += -lfftw3
}
