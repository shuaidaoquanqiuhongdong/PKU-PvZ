#ifndef PLANTCARDWIDGET_H
#define PLANTCARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>

class PlantCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlantCardWidget(const QString& plantType, const QString& displayName,
                             int cost, const QString& iconPath,
                             QWidget *parent = nullptr);
    QString getPlantType() const;
    int getCost() const;
    void setSelected(bool selected);
    void setAffordable(bool affordable);
    void setCooldown(int cooldownMs);
signals:
    void clicked(const QString& plantType);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
private:
    QString plantType;
    QString displayName;
    int cost;
    QPixmap icon;
    bool selected;
    bool affordable;
    int cooldownRemaining;
};

#endif
