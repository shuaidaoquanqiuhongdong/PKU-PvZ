#ifndef CARDPANEL_H
#define CARDPANEL_H

#include <QWidget>
#include <QList>
#include <QHBoxLayout>
#include "PlantCardWidget.h"

class CardPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CardPanel(QWidget *parent = nullptr);
    void addPlantCard(const QString& plantType, const QString& displayName,
                      int cost, const QString& iconPath);
    void addZombieCard(const QString& zombieType, const QString& displayName,
                       int cost, const QString& iconPath);
    QString selectedCardType() const;
    bool isPlantMode() const;
    void clearSelection();
    void updateCardState(int currentSun);
    void updatePlantCooldown(const QString& plantType, int cooldownMs);
    void clearAllCards();
signals:
    void cardSelected(const QString& cardType, bool isPlant);
private:
    QString currentSelectedType;
    bool plantMode;
    QList<PlantCardWidget*> cards;
    QHBoxLayout* layout;
};

#endif
