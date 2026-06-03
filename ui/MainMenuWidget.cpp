#include "MainMenuWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

MainMenuWidget::MainMenuWidget(QWidget *parent)
    : QWidget(parent)
    , modePanel(nullptr)
{
    setStyleSheet("background-color: #1a3a1a;");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto* titleLabel = new QLabel("植物大战僵尸", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 42px; font-weight: bold; color: #4CAF50; padding: 20px;");

    auto* subtitleLabel = new QLabel("PvZLiteQt", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 18px; color: #8BC34A; padding-bottom: 40px;");

    classicModeBtn = new QPushButton("🌱 经典模式", this);
    classicModeBtn->setFixedSize(280, 65);
    classicModeBtn->setStyleSheet(
        "QPushButton { font-size: 22px; background-color: #4CAF50; color: white;"
        "border-radius: 10px; font-weight: bold; border: 3px solid #45a049; }"
        "QPushButton:hover { background-color: #66BB6A; border-color: #81C784; }"
        "QPushButton:pressed { background-color: #388E3C; }");

    zombieModeBtn = new QPushButton("🧟 操控僵尸模式", this);
    zombieModeBtn->setFixedSize(280, 65);
    zombieModeBtn->setStyleSheet(
        "QPushButton { font-size: 22px; background-color: #FF7043; color: white;"
        "border-radius: 10px; font-weight: bold; border: 3px solid #E64A19; }"
        "QPushButton:hover { background-color: #FF8A65; border-color: #FFAB91; }"
        "QPushButton:pressed { background-color: #E64A19; }");

    guideButton = new QPushButton("📖 图鉴", this);
    guideButton->setFixedSize(280, 65);
    guideButton->setStyleSheet(
        "QPushButton { font-size: 22px; background-color: #2196F3; color: white;"
        "border-radius: 10px; font-weight: bold; border: 3px solid #1976D2; }"
        "QPushButton:hover { background-color: #42A5F5; border-color: #64B5F6; }"
        "QPushButton:pressed { background-color: #1565C0; }");

    exitButton = new QPushButton("退出游戏", this);
    exitButton->setFixedSize(220, 55);
    exitButton->setStyleSheet(
        "QPushButton { font-size: 20px; background-color: #f44336; color: white;"
        "border-radius: 8px; font-weight: bold; border: 2px solid #da190b; }"
        "QPushButton:hover { background-color: #EF5350; border-color: #ff7043; }"
        "QPushButton:pressed { background-color: #C62828; }");

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addWidget(classicModeBtn, 0, Qt::AlignCenter);
    layout->addSpacing(15);
    layout->addWidget(zombieModeBtn, 0, Qt::AlignCenter);
    layout->addSpacing(15);
    layout->addWidget(guideButton, 0, Qt::AlignCenter);
    layout->addSpacing(30);
    layout->addWidget(exitButton, 0, Qt::AlignCenter);

    connect(classicModeBtn, &QPushButton::clicked, this, &MainMenuWidget::onStartClicked);
    connect(zombieModeBtn, &QPushButton::clicked, this, &MainMenuWidget::onZombieModeClicked);
    connect(guideButton, &QPushButton::clicked, this, &MainMenuWidget::onGuideClicked);
    connect(exitButton, &QPushButton::clicked, this, &MainMenuWidget::exitClicked);
}

void MainMenuWidget::onStartClicked()
{
    emit startClicked();
}

void MainMenuWidget::onZombieModeClicked()
{
    emit zombieModeClicked();
}

void MainMenuWidget::onGuideClicked()
{
    emit guideClicked();
}
