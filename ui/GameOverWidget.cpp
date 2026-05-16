#include "GameOverWidget.h"
#include <QVBoxLayout>

GameOverWidget::GameOverWidget(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: #1a1a2e;");

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    resultLabel = new QLabel("", this);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet(
        "font-size: 48px; font-weight: bold; padding: 20px;");

    auto* infoLabel = new QLabel("点击下方按钮重新开始", this);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("font-size: 16px; color: #888; padding-bottom: 30px;");

    restartButton = new QPushButton("重新开始", this);
    restartButton->setFixedSize(220, 55);
    restartButton->setStyleSheet(
        "QPushButton { font-size: 20px; background-color: #4CAF50; color: white;"
        "border-radius: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45a049; }");

    menuButton = new QPushButton("返回主菜单", this);
    menuButton->setFixedSize(220, 55);
    menuButton->setStyleSheet(
        "QPushButton { font-size: 20px; background-color: #2196F3; color: white;"
        "border-radius: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1976D2; }");

    layout->addWidget(resultLabel);
    layout->addWidget(infoLabel);
    layout->addWidget(restartButton, 0, Qt::AlignCenter);
    layout->addSpacing(15);
    layout->addWidget(menuButton, 0, Qt::AlignCenter);

    connect(restartButton, &QPushButton::clicked, this, &GameOverWidget::restartClicked);
    connect(menuButton, &QPushButton::clicked, this, &GameOverWidget::mainMenuClicked);
}

void GameOverWidget::setResult(bool win)
{
    if (win) {
        resultLabel->setText("胜利!");
        resultLabel->setStyleSheet(
            "font-size: 48px; font-weight: bold; color: #4CAF50; padding: 20px;");
    } else {
        resultLabel->setText("失败!");
        resultLabel->setStyleSheet(
            "font-size: 48px; font-weight: bold; color: #f44336; padding: 20px;");
    }
}
