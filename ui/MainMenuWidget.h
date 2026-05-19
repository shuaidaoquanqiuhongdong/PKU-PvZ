#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class MainMenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenuWidget(QWidget *parent = nullptr);
signals:
    void startClicked();
    void exitClicked();
private:
    QPushButton* startButton;
    QPushButton* exitButton;
};

#endif
