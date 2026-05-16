#ifndef GAMEPAGEWIDGET_H
#define GAMEPAGEWIDGET_H

#include <QWidget>
#include "TopBarWidget.h"
#include "CardPanel.h"
#include "GameView.h"

class GamePageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GamePageWidget(QWidget *parent = nullptr);
    GameView* getGameView() const;
    CardPanel* getCardPanel() const;
    TopBarWidget* getTopBarWidget() const;
private:
    TopBarWidget* topBarWidget;
    CardPanel* cardPanel;
    GameView* gameView;
};

#endif
