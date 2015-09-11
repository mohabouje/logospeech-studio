#ifndef LEVELTHRESHOLDDETECTOR_H
#define LEVELTHRESHOLDDETECTOR_H


#include <QDialog>

#include <capturedata/recorder.h>
#include <logo/logo_structs.h>
#include <logo/logo_preprocessing.h>
#include <logo/logo_sta.h>
#include "processsettingsdialog.h"

namespace Ui {
class LevelThreSholdDetector;
}

class LevelThreSholdDetector : public QDialog
{
    Q_OBJECT

public:
    explicit LevelThreSholdDetector(QWidget *parent = 0);
    ~LevelThreSholdDetector();

    logo_real getTempEnergy() const;

    logo_real getTempAZCR() const;

private slots:
    void process(const QAudioBuffer &buffer);
    void play();
    void getResult();
private:
    Ui::LevelThreSholdDetector *ui;

    Recorder*                   _recorder;
    logo_vector                 _temp;
    logo_real                   _tempEnergy;
    logo_real                   _tempAZCR;
    int                         _iteration;

};

#endif // LEVELTHRESHOLDDETECTOR_H
