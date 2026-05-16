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
    QString selectedPlantType() const;
    void clearSelection();
    void updateCardState(int currentSun);
signals:
    void plantSelected(const QString& plantType);
private:
    QString currentSelectedPlantType;
    QList<PlantCardWidget*> cards;
    QHBoxLayout* layout;
};

#endif
