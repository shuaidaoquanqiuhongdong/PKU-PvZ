#ifndef MAINMENUWIDGET_H
#define MAINMENUWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class MainMenuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenuWidget(QWidget *parent = nullptr);
signals:
    void startClicked();
    void zombieModeClicked();
    void guideClicked();
    void exitClicked();
private slots:
    void onStartClicked();
    void onZombieModeClicked();
    void onGuideClicked();
private:
    QPushButton* classicModeBtn;
    QPushButton* zombieModeBtn;
    QPushButton* guideButton;
    QPushButton* exitButton;
    QLabel* modePanel;
    void showModePanel();
    void hideModePanel();
};

#endif
