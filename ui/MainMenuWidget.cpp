#include "MainMenuWidget.h"
#include <QVBoxLayout>

MainMenuWidget::MainMenuWidget(QWidget *parent)
    : QWidget(parent)
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

    startButton = new QPushButton("开始游戏", this);
    startButton->setFixedSize(220, 55);
    startButton->setStyleSheet(
        "QPushButton { font-size: 20px; background-color: #4CAF50; color: white;"
        "border-radius: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45a049; }");

    exitButton = new QPushButton("退出游戏", this);
    exitButton->setFixedSize(220, 55);
    exitButton->setStyleSheet(
        "QPushButton { font-size: 20px; background-color: #f44336; color: white;"
        "border-radius: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #da190b; }");

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addWidget(startButton, 0, Qt::AlignCenter);
    layout->addSpacing(15);
    layout->addWidget(exitButton, 0, Qt::AlignCenter);

    connect(startButton, &QPushButton::clicked, this, &MainMenuWidget::startClicked);
    connect(exitButton, &QPushButton::clicked, this, &MainMenuWidget::exitClicked);
}
