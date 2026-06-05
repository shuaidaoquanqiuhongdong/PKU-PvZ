#include "PlantCardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    , cooldownRemaining(0)
{
    setFixedSize(70, 90);
    setCursor(Qt::PointingHandCursor);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(2);

    QPixmap tempIcon(iconPath);
    if (!tempIcon.isNull()) {
        icon = tempIcon;
    } else {
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
    iconLabel->setFixedSize(60, 55);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(icon.scaled(55, 55, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    mainLayout->addWidget(iconLabel, 0, Qt::AlignCenter);

    auto* nameLabel = new QLabel(displayName, this);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("font-size: 9px; color: white; font-weight: bold;");
    mainLayout->addWidget(nameLabel);

    auto* costLayout = new QHBoxLayout();
    costLayout->setSpacing(2);
    costLayout->addStretch();

    auto* sunIconLabel = new QLabel(this);
    QPixmap sunIcon(20, 20);
    sunIcon.fill(Qt::transparent);
    QPainter sp(&sunIcon);
    sp.setRenderHint(QPainter::Antialiasing);
    sp.setBrush(QColor("#FFD700"));
    sp.setPen(QPen(QColor("#FFA500"), 1));
    sp.drawEllipse(2, 2, 16, 16);
    sunIconLabel->setPixmap(sunIcon.scaled(12, 12, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    costLayout->addWidget(sunIconLabel);

    auto* costLabel = new QLabel(QString::number(cost), this);
    costLabel->setAlignment(Qt::AlignCenter);
    costLabel->setStyleSheet("font-size: 10px; color: #FFD700; font-weight: bold;");
    costLayout->addWidget(costLabel);

    costLayout->addStretch();
    mainLayout->addLayout(costLayout);

    setStyleSheet("background-color: #3a3a3a; border: 1px solid #555555; border-radius: 5px;");
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

void PlantCardWidget::setCooldown(int cooldownMs)
{
    cooldownRemaining = cooldownMs;
    update();
}

void PlantCardWidget::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (affordable && cooldownRemaining <= 0) {
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

    if (cooldownRemaining > 0) {
        int cdSeconds = (cooldownRemaining + 999) / 1000;
        painter.fillRect(rect(), QColor(0, 0, 0, 160));
        
        QFont font = painter.font();
        font.setPointSize(16);
        font.setBold(true);
        painter.setFont(font);
        painter.setPen(QColor("#FFFFFF"));
        painter.drawText(rect(), Qt::AlignCenter, QString::number(cdSeconds));
    }
}
