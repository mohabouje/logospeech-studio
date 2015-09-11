#include "folderwidget.h"
#include "ui_folderwidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QDateTime>
#include <QAudioProbe>
#include <QAudio>
#include <QMessageBox>
#include <settings/appsettings.h>
#include <ui_mainwindow.h>



FolderWidget::FolderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FolderWidget),
    _dirModel(new QFileSystemModel(parent)),
    _audioFileReader(new WavReader(parent)),
    _player(new QMediaPlayer(0, QMediaPlayer::StreamPlayback))

{
    ui->setupUi(this);

    _dirModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files );
    QStringList filters;
    filters << "*.wav";
    filters << "*.WAV";
    _dirModel->setNameFilters(filters);
    _dirModel->setNameFilterDisables(false);
    ui->folderListView->setModel(_dirModel);
    ui->detailWidget->setEnabled(false);
    connect(ui->folderListView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(directoryChanged(QModelIndex)));
    connect(ui->folderListView, SIGNAL(clicked(QModelIndex)), this, SLOT(fileSelected(QModelIndex)));
    connect(ui->folderListView,SIGNAL(entered(QModelIndex)),this, SLOT(fileSelected(QModelIndex)));
    connect(ui->goLeftButton, SIGNAL(clicked()), this, SLOT(goBack()));
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openFolder()));
    connect(_audioFileReader,SIGNAL(processedUSecs(qint64)), this, SLOT(positionChanged(qint64)));
    connect(_audioFileReader,SIGNAL(readAllData(QVector<double>)),this, SLOT(readAllData(QVector<double>)));
    connect(_audioFileReader,SIGNAL(stateChangeg(QAudio::State)),this, SLOT(stateChanged(QAudio::State)));

    loadFromSettings();
}

void FolderWidget::stateChanged(const QAudio::State& state){
    switch (state) {
    case QAudio::IdleState:
        _parent->actionPlay->setIcon(QIcon(":resources/icons/refresh.png"));
        emit idleState();
        break;
    default:
        break;
    }
}
void FolderWidget::setParent(Ui::MainWindow *parent)
{
    _parent = parent;
}


ControlWidget* FolderWidget::controlWidget() {
    return ui->controlWidget;
}


void FolderWidget::readAllData(const logo_vector& data){

    _loadedFilePlot->clearData();
    int sampleRate = _audioFileReader->format().sampleRate();
    double duration = ((double)(data.size())/(double)(sampleRate));
    logo_plot plot = new_vecplot(data.size(),0, sampleRate,duration);
    plot.y = data;
    logo_pair minmax;
    logo_minmax_vec(plot.y, minmax);
    _plot->yAxis->setRange(minmax.first, minmax.second);
    _plot->xAxis->setRange(0, plot.x[data.size()-1]);
    _loadedFilePlot->setData(plot.x, plot.y);
    _plot->replot();

}




WavReader *FolderWidget::wavReader(){
    return this->_audioFileReader;
}


void FolderWidget::restartWavReader(){
    if (_audioFileReader->state() == QAudio::ActiveState )
        _audioFileReader->stop();

    _audioFileReader->release();
    int state =  _audioFileReader->setSource(_audioFileReader->fileName());
    if (state == Succes) {
        _audioFileReader->setNotifyInterval(40);
        _audioFileReader->readAll();
        _audioFileReader->processedUSecs(0);
        emit showStatusMesasge(tr("Refresh %1").arg(_fileInfo->baseName()));
        emit restartPlot();
        emit enablePlayButton(true);
    } else {
        emit showStatusMesasge(tr("Error refreshing file %1").arg(_fileInfo->baseName()));
    }
}

void FolderWidget::attachQwtPlot(QCustomPlot* plot){
    const QColor backgroundColor = this->palette().background().color();
    _plot = plot;
    _plot->setBackground(backgroundColor);
    _plot->axisRect()->setBackground(backgroundColor);
    _plot->axisRect()->setAutoMargins(QCP::msNone);
    _plot->axisRect()->setMargins(QMargins(0,0,0,0));


    _plot->xAxis->setTicks(false);
    _plot->yAxis->setTicks(false);
    _plot->xAxis->grid()->setVisible(false);
    _plot->yAxis->grid()->setVisible(false);


    _positionPlot = plot->addGraph();
    _loadedFilePlot = plot->addGraph();

}

void FolderWidget::positionChanged(const qint64& position){

    double pos = (double)(position);
    QVector<double> data_X(2,0);
    data_X[0] = pos/1e6;
    data_X[1] = pos/1e6;
    QVector<double> data_Y(2,0);
    data_Y[0] = -32768;
    data_Y[1] = 32768;
    _positionPlot->setData(data_X,data_Y);
    _plot->replot();
}


void FolderWidget::fileSelected(const QModelIndex& index){
    ui->detailWidget->setEnabled(true);

    QString currentFolderSelected = _dirModel->data(index, QFileSystemModel::FilePathRole).toString();
    QIcon currentIcon = _dirModel->data(index, QFileSystemModel::FileIconRole).value<QIcon>();


    _fileInfo = new QFileInfo(currentFolderSelected);
    ui->iconItem->setIcon(currentIcon);
    ui->nameItem->setText(_fileInfo->baseName());
    ui->labelDateCreated->setText(_fileInfo->created().toString("dd/MM/yyyy"));
    ui->labelDateModified->setText(_fileInfo->lastModified().toString("dd/MM/yyyy"));
    ui->labelSize->setText(QString::number(_fileInfo->size())+" B");

    if (_fileInfo->isFile()) {
        ui->labelFormat->setText(_fileInfo->completeSuffix());
    } else {
        ui->labelFormat->setText(" ");
        if (_audioFileReader->audioOutput() && _audioFileReader->state() != QAudio::ActiveState)
                emit enablePlayButton(false);
    }

}


bool couldReturn(QString folder){
    int pos = folder.lastIndexOf('/');
    if (pos == -1)
        return false;
    else
        return true;
}

void FolderWidget::goBack(){
    ui->detailWidget->setEnabled(false);
    if (couldReturn(_currentFolder))
        ui->goLeftButton->setEnabled(true);
    else
        ui->goLeftButton->setEnabled(false);
    _currentFolder.truncate(_currentFolder.lastIndexOf('/'));
    directoryChanged(_currentFolder);
}

void FolderWidget::directoryChanged(const QString& directory){
    _currentFolder = directory;
    _dirModel->setRootPath(_currentFolder);
    ui->folderListView->setRootIndex(_dirModel->index(_currentFolder));
    emit showStatusMesasge(_currentFolder);
}



void FolderWidget::directoryChanged(const QModelIndex& index){
    QString currentFolderSelected = _dirModel->data(index, QFileSystemModel::FilePathRole).toString();
    _fileInfo = new QFileInfo(currentFolderSelected);

    if (_fileInfo->isFile()){

        if (wavReader()->state() != QAudio::IdleState) {
            emit restartPlot();
            restartWavReader();
        }

        int state =  _audioFileReader->setSource(currentFolderSelected);
        if (state == Succes) {
            _audioFileReader->setNotifyInterval(40);
            _audioFileReader->readAll();
            _audioFileReader->processedUSecs(0);
            _parent->actionPlay->setIcon(QIcon(":/resources/icons/play.png"));
            emit showStatusMesasge(tr("File %1 loaded").arg(_fileInfo->baseName()));
            emit enablePlayButton(true);

        } else {
            QMessageBox::critical(
                 this,
                 tr("Error while loading file %1").arg(_fileInfo->baseName()),
                 state == FileFormatNotSupported ? tr("File format not supported. Only mnocanal") : tr("Error reading file") );
            emit showStatusMesasge(tr("Error while loading file %1").arg(_fileInfo->baseName()));
        }
    } else {
        _currentFolder = currentFolderSelected;
        if (couldReturn(_currentFolder))
            ui->goLeftButton->setEnabled(true);
        else
            ui->goLeftButton->setEnabled(false);

        _dirModel->setRootPath(_currentFolder);
        ui->folderListView->setRootIndex(index);

        emit showStatusMesasge(_currentFolder);

    }
}



void FolderWidget::loadFromSettings(){
    AppSettings *settings =  new AppSettings();
    QString directory = settings->currentSettings()->value(KEY_DEFAULT_PATH,QDir::currentPath()).toString();
    directoryChanged(directory);
    if (couldReturn(_currentFolder))
        ui->goLeftButton->setEnabled(true);
    else
        ui->goLeftButton->setEnabled(false);
}


void FolderWidget::saveSettings(){
    AppSettings *settings =  new AppSettings();
    settings->currentSettings()->setValue(KEY_DEFAULT_PATH,_currentFolder);
}

FolderWidget::~FolderWidget()
{
    delete ui;
}



void FolderWidget::openFolder(){
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    _currentFolder,
                                                    QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty() || dir.isNull())
        return;

    directoryChanged(dir);

    if (couldReturn(_currentFolder))
        ui->goLeftButton->setEnabled(true);
    else
        ui->goLeftButton->setEnabled(false);
}




