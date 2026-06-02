#include "CardPanel.h"
#include <QHBoxLayout>

CardPanel::CardPanel(QWidget *parent)
    : QWidget(parent)
    , plantMode(true)
{
    setFixedHeight(105);
    setStyleSheet("background-color: #3a3a3a;");
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(8);
    layout->addStretch();
}

void CardPanel::addPlantCard(const QString& plantType, const QString& displayName,
                             int cost, const QString& iconPath)
{
    plantMode = true;
    auto* card = new PlantCardWidget(plantType, displayName, cost, iconPath, this);
    cards.append(card);
    layout->insertWidget(layout->count() - 1, card);

    connect(card, &PlantCardWidget::clicked, this, [this, plantType](const QString&) {
        if (currentSelectedType == plantType) {
            clearSelection();
        } else {
            clearSelection();
            currentSelectedType = plantType;
            for (auto* c : cards) {
                if (c->getPlantType() == plantType) {
                    c->setSelected(true);
                    break;
                }
            }
        }
        emit cardSelected(currentSelectedType, true);
    });
}

void CardPanel::addZombieCard(const QString& zombieType, const QString& displayName,
                              int cost, const QString& iconPath)
{
    plantMode = false;
    auto* card = new PlantCardWidget(zombieType, displayName, cost, iconPath, this);
    cards.append(card);
    layout->insertWidget(layout->count() - 1, card);

    connect(card, &PlantCardWidget::clicked, this, [this, zombieType](const QString&) {
        if (currentSelectedType == zombieType) {
            clearSelection();
        } else {
            clearSelection();
            currentSelectedType = zombieType;
            for (auto* c : cards) {
                if (c->getPlantType() == zombieType) {
                    c->setSelected(true);
                    break;
                }
            }
        }
        emit cardSelected(currentSelectedType, false);
    });
}

QString CardPanel::selectedCardType() const
{
    return currentSelectedType;
}

bool CardPanel::isPlantMode() const
{
    return plantMode;
}

void CardPanel::clearSelection()
{
    currentSelectedType.clear();
    for (auto* card : cards) {
        card->setSelected(false);
    }
}

void CardPanel::updateCardState(int currentSun)
{
    for (auto* card : cards) {
        card->setAffordable(currentSun >= card->getCost());
    }
}

void CardPanel::clearAllCards()
{
    clearSelection();
    for (auto* card : cards) {
        layout->removeWidget(card);
        card->deleteLater();
    }
    cards.clear();
}
