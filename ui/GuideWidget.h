#ifndef GUIDEWIDGET_H
#define GUIDEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QScrollArea>

class GuideWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GuideWidget(QWidget *parent = nullptr);
signals:
    void backToMenuClicked();
private slots:
    void onBackClicked();
    void onTabPlantClicked();
    void onTabZombieClicked();
private:
    QPushButton* backButton;
    QPushButton* plantTabBtn;
    QPushButton* zombieTabBtn;
    QStackedWidget* contentStack;
    void createPlantPage();
    void createZombiePage();
};

#endif