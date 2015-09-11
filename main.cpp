#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QThread>
#include <QObject>
#include <capturedata/recorder.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap pixmap(":/resources/splahscreen.jpg");
    QSplashScreen splash(pixmap);
    splash.showMessage("Loading libraries...");
    splash.show();

    //checkAudioDevice();

    a.processEvents();
    MainWindow w;
    splash.finish(&w);
    w.show();
    return a.exec();
}
