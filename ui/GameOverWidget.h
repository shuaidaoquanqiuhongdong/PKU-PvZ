#ifndef GAMEOVERWIDGET_H
#define GAMEOVERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class GameOverWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameOverWidget(QWidget *parent = nullptr);
    void setResult(bool win);
signals:
    void restartClicked();
    void mainMenuClicked();
private:
    QLabel* resultLabel;
    QPushButton* restartButton;
    QPushButton* menuButton;
};

#endif
