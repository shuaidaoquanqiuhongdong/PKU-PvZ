#include "GamePageWidget.h"
#include <QVBoxLayout>

GamePageWidget::GamePageWidget(QWidget *parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    topBarWidget = new TopBarWidget(this);
    layout->addWidget(topBarWidget);

    cardPanel = new CardPanel(this);
    layout->addWidget(cardPanel);

    gameView = new GameView(this);
    layout->addWidget(gameView, 1);
}

GameView* GamePageWidget::getGameView() const { return gameView; }
CardPanel* GamePageWidget::getCardPanel() const { return cardPanel; }
TopBarWidget* GamePageWidget::getTopBarWidget() const { return topBarWidget; }
