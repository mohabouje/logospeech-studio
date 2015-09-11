#ifndef CHARTSETTINGSDIALOG_H
#define CHARTSETTINGSDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QPalette>
#include <QSlider>

#define KEY_ENERGY_CURRENT_TAB "chartSettings/currentTab"


#define KEY_ENERGY_CHART_LINEWIDTH "chartSettings/energyLineWidth"
#define KEY_ENERGY_CHART_LINECOLOR "chartSettings/energyLineColor"
#define KEY_ENERGY_CHART_X_AXIS "chartSettings/energyXAxis"
#define KEY_ENERGY_CHART_X_GRID "chartSettings/energyXGrid"
#define KEY_ENERGY_CHART_Y_AXIS "chartSettings/energyYAxis"
#define KEY_ENERGY_CHART_Y_GRID "chartSettings/energyYGrid"

#define ENERGY_CHART_LINEWIDTH 1
#define ENERGY_CHART_LINECOLOR "#000000"
#define ENERGY_CHART_X_AXIS 0
#define ENERGY_CHART_X_GRID 1
#define ENERGY_CHART_Y_AXIS 0
#define ENERGY_CHART_Y_GRID 1




#define KEY_PITCH_CHART_LINEWIDTH "chartSettings/pitchLineWidth"
#define KEY_PITCH_CHART_LINECOLOR "chartSettings/pitchLineColor"
#define KEY_PITCH_CHART_X_AXIS "chartSettings/pitchXAxis"
#define KEY_PITCH_CHART_X_GRID "chartSettings/pitchXGrid"
#define KEY_PITCH_CHART_Y_AXIS "chartSettings/pitchYAxis"
#define KEY_PITCH_CHART_Y_GRID "chartSettings/pitchYGrid"

#define PITCH_CHART_LINEWIDTH 1
#define PITCH_CHART_LINECOLOR "#000000"
#define PITCH_CHART_X_AXIS 0
#define PITCH_CHART_X_GRID 1
#define PITCH_CHART_Y_AXIS 0
#define PITCH_CHART_Y_GRID 1




#define KEY_SPECTROGRAM_CHART_COLORMAP "chartSettings/spectrogramColorMap"
#define KEY_SPECTROGRAM_CHART_X_AXIS "chartSettings/spectrogramXAxis"
#define KEY_SPECTROGRAM_CHART_Y_AXIS "chartSettings/spectrogramYAxis"
#define KEY_SPECTROGRAM_MU_LAW "chartSettings/muLaw"


#define SPECTROGRAM_CHART_COLORMAP "Spectral"
#define SPECTROGRAM_CHART_X_AXIS 1
#define SPECTROGRAM_CHART_Y_AXIS 1
namespace Ui {
class ChartSettingsDialog;
}

class ChartSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChartSettingsDialog(QWidget *parent = 0);
    ~ChartSettingsDialog();



    int energy_LineWidth() const;
    QString energy_LineColor() const;
    bool energy_X_Axis() const;
    bool energy_Y_Axis() const;
    bool energy_X_Grid() const;
    bool energy_Y_Grid() const;

    int pitch_LineWidth() const;
    QString pitch_LineColor() const;
    bool pitch_X_Axis() const;
    bool pitch_Y_Axis() const;
    bool pitch_X_Grid() const;
    bool pitch_Y_Grid() const;

    int spectrogram_colormap() const;
    bool spectrogram_Y_Axis() const;
    bool spectrogram_X_Axis() const;


    int muLawValue() const;

    QSlider *slider();
private slots:
    void restore();
    void save();
    void changeColor();
    void showColorMap(const int index);
    void muLawValueChanged(const int &currentValue);
private:
    Ui::ChartSettingsDialog *ui;
    void loadFromSettings();


    int                 _energy_LineWidth;
    QString             _energy_LineColor;
    bool                _energy_X_Axis;
    bool                _energy_Y_Axis;
    bool                _energy_X_Grid;
    bool                _energy_Y_Grid;

    int                 _pitch_LineWidth;
    QString             _pitch_LineColor;
    bool                _pitch_X_Axis;
    bool                _pitch_Y_Axis;
    bool                _pitch_X_Grid;
    bool                _pitch_Y_Grid;

    int                 _spectrogram_colormap;
    bool                _spectrogram_X_Axis;
    bool                _spectrogram_Y_Axis;

    int                 _muLawValue;



};

#endif // CHARTSETTINGSDIALOG_H
