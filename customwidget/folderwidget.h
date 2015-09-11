#ifndef FOLDERWIDGET_H
#define FOLDERWIDGET_H

#include <QWidget>
#include <QDir>
#include <QFileSystemModel>
#include <QMediaPlayer>

#include <capturedata/wavreader.h>
#include "mainwindow.h"
#include "folderlistview.h"
#include "controlwidget.h"

namespace Ui {
class FolderWidget;
}

class FolderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FolderWidget(QWidget *parent = 0);
    ~FolderWidget();

    void loadFromSettings();
    void saveSettings();
    void attachQwtPlot(QCustomPlot *plot);
    WavReader *wavReader();


    void setParent(Ui::MainWindow *parent);
    ControlWidget *controlWidget();

public slots:
    void positionChanged(const qint64 &position);
    void restartWavReader();
    void openFolder();
private slots:
    void goBack();
    void directoryChanged(const QModelIndex &index);
    void directoryChanged(const QString &directory);
    void fileSelected(const QModelIndex &index);
    void readAllData(const QVector<double> &data);
    void stateChanged(const QAudio::State &state);
signals:
    void showStatusMesasge(const QString folder);
    void restartPlot();
    void enablePlayButton(const bool& state);
    void idleState();

private:
    Ui::FolderWidget*       ui;
    QString                 _currentFolder;
    QString                 _settingsFolder;
    QList<QString>          _registerFolder;
    QFileSystemModel*       _dirModel;
    QFileInfo*              _fileInfo;
    WavReader*              _audioFileReader;
    QMediaPlayer*           _player;
    const QIODevice*        _mediaStream;
    QCustomPlot*            _plot;
    QCPGraph*               _loadedFilePlot;
    QCPGraph*               _positionPlot;
    Ui::MainWindow*         _parent;





};

#endif // FOLDERWIDGET_H
