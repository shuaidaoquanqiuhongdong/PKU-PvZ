#include "PlantCardWidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>

PlantCardWidget::PlantCardWidget(const QString& plantType, const QString& displayName,
                                 int cost, const QString& iconPath,
                                 QWidget *parent)
    : QWidget(parent)
    , plantType(plantType)
    , displayName(displayName)
    , cost(cost)
    , selected(false)
    , affordable(true)
{
    setFixedSize(70, 80);
    setCursor(Qt::PointingHandCursor);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);

    // Try to load icon, create colored placeholder on failure
    icon = QPixmap(iconPath);
    if (icon.isNull()) {
        icon = QPixmap(50, 50);
        icon.fill(Qt::transparent);
        QPainter p(&icon);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor("#4CAF50"));
        p.setPen(QPen(Qt::white, 2));
        p.drawRoundedRect(1, 1, 48, 48, 6, 6);
        p.setPen(Qt::white);
        QFont f = p.font();
        f.setPointSize(14);
        f.setBold(true);
        p.setFont(f);
        p.drawText(QRect(0, 0, 50, 50), Qt::AlignCenter, displayName.left(2));
    }

    auto* iconLabel = new QLabel(this);
    iconLabel->setFixedSize(60, 50);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(icon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    layout->addWidget(iconLabel, 0, Qt::AlignCenter);

    auto* nameLabel = new QLabel(displayName, this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-size: 9px; color: white;");
    layout->addWidget(nameLabel);

    auto* costLabel = new QLabel(QString::number(cost), this);
    costLabel->setAlignment(Qt::AlignCenter);
    costLabel->setStyleSheet("font-size: 9px; color: #FFD700;");
    layout->addWidget(costLabel);

    setStyleSheet("background-color: #4a4a4a; border-radius: 5px;");
}

QString PlantCardWidget::getPlantType() const { return plantType; }
int PlantCardWidget::getCost() const { return cost; }

void PlantCardWidget::setSelected(bool sel)
{
    selected = sel;
    update();
}

void PlantCardWidget::setAffordable(bool aff)
{
    affordable = aff;
    update();
}

void PlantCardWidget::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (affordable) {
        emit clicked(plantType);
    }
}

void PlantCardWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!affordable) {
        painter.fillRect(rect(), QColor(0, 0, 0, 120));
    }

    if (selected) {
        QPen pen(QColor("#FFD700"), 3);
        painter.setPen(pen);
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 5, 5);
    }
}
