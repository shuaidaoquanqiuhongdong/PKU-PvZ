#include "CardPanel.h"
#include <QHBoxLayout>

CardPanel::CardPanel(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(90);
    setStyleSheet("background-color: #3a3a3a;");
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(8);
    layout->addStretch();
}

void CardPanel::addPlantCard(const QString& plantType, const QString& displayName,
                              int cost, const QString& iconPath)
{
    auto* card = new PlantCardWidget(plantType, displayName, cost, iconPath, this);
    cards.append(card);

    // Insert before the trailing stretch
    layout->insertWidget(layout->count() - 1, card);

    connect(card, &PlantCardWidget::clicked, this, [this](const QString& type) {
        if (currentSelectedPlantType == type) {
            clearSelection();
        } else {
            clearSelection();
            currentSelectedPlantType = type;
            for (auto* c : cards) {
                if (c->getPlantType() == type) {
                    c->setSelected(true);
                    break;
                }
            }
        }
        emit plantSelected(currentSelectedPlantType);
    });
}

QString CardPanel::selectedPlantType() const
{
    return currentSelectedPlantType;
}

void CardPanel::clearSelection()
{
    currentSelectedPlantType.clear();
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
