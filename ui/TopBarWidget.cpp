#include "TopBarWidget.h"
#include <QHBoxLayout>

TopBarWidget::TopBarWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(50);
    setStyleSheet("background-color: #2c2c2c;");

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);

    sunDisplay = new SunDisplay(this);
    layout->addWidget(sunDisplay);

    layout->addStretch();

    waveLabel = new QLabel("Wave: 1/5", this);
    waveLabel->setStyleSheet("font-size: 14px; color: white;");
    layout->addWidget(waveLabel);

    layout->addStretch();

    pauseButton = new QPushButton("暂停", this);
    pauseButton->setFixedSize(60, 30);
    pauseButton->setStyleSheet("font-size: 14px;");
    layout->addWidget(pauseButton);

    connect(pauseButton, &QPushButton::clicked, this, [this]() {
        emit pauseClicked();
    });
}

void TopBarWidget::setPaused(bool paused)
{
    isPaused = paused;
    pauseButton->setText(paused ? "继续" : "暂停");
}

void TopBarWidget::setSunValue(int value)
{
    sunDisplay->setSunValue(value);
}

void TopBarWidget::setWaveInfo(int currentWave, int totalWave)
{
    waveLabel->setText(QString("Wave: %1/%2").arg(currentWave).arg(totalWave));
}
