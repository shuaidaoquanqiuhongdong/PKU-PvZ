#ifndef TOPBARWIDGET_H
#define TOPBARWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "SunDisplay.h"

class TopBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TopBarWidget(QWidget *parent = nullptr);
    void setSunValue(int value);
    void setWaveInfo(int currentWave, int totalWave);
    void setPaused(bool paused);
signals:
    void pauseClicked();
    void shovelModeChanged(bool enabled);
private:
    SunDisplay* sunDisplay;
    QPushButton* pauseButton;
    QPushButton* shovelButton;
    QLabel* waveLabel;
    bool isPaused = false;
    bool shovelMode = false;
};

#endif
