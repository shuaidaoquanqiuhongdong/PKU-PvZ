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

    shovelButton = new QPushButton("⛏️ 铲子", this);
    shovelButton->setObjectName("shovelButton");
    shovelButton->setFixedSize(70, 30);
    shovelButton->setStyleSheet(
        "QPushButton { font-size: 14px; background-color: #607D8B; color: white; border-radius: 5px; }"
        "QPushButton:hover { background-color: #78909C; }"
        "QPushButton:checked { background-color: #FF7043; }"
    );
    shovelButton->setCheckable(true);
    layout->addWidget(shovelButton);

    pauseButton = new QPushButton("暂停", this);
    pauseButton->setFixedSize(60, 30);
    pauseButton->setStyleSheet("font-size: 14px;");
    layout->addWidget(pauseButton);

    connect(pauseButton, &QPushButton::clicked, this, [this]() {
        emit pauseClicked();
    });

    connect(shovelButton, &QPushButton::toggled, this, [this](bool checked) {
        shovelMode = checked;
        emit shovelModeChanged(checked);
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
