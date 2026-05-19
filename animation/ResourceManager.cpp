#include "ResourceManager.h"

#include <QPainter>
#include <QPen>
#include <QFont>
#include <QDebug>

QPixmap ResourceManager::loadPixmap(const QString& path)
{
    QPixmap pixmap;

    if (!pixmap.load(path)) {
        qWarning() << "ResourceManager failed to load pixmap:" << path;
        return createPlaceholderPixmap(QSize(80, 80), QColor(180, 180, 180), "Missing");
    }

    return pixmap;
}

QVector<QPixmap> ResourceManager::loadFrames(
    const QString& folder,
    const QString& prefix,
    int count
) {
    QVector<QPixmap> frames;
    frames.reserve(count);

    QString normalizedFolder = folder;
    if (normalizedFolder.endsWith('/')) {
        normalizedFolder.chop(1);
    }

    for (int i = 0; i < count; ++i) {
        const QString path =
            QString("%1/%2_%3.png")
            .arg(normalizedFolder)
            .arg(prefix)
            .arg(i);

        frames.push_back(loadPixmap(path));
    }

    return frames;
}

QPixmap ResourceManager::createPlaceholderPixmap(
    QSize size,
    QColor color,
    const QString& text
) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setBrush(color);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRoundedRect(QRect(QPoint(0, 0), size - QSize(1, 1)), 8, 8);

    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(10);
    font.setBold(true);
    painter.setFont(font);

    painter.drawText(
        QRect(QPoint(0, 0), size),
        Qt::AlignCenter | Qt::TextWordWrap,
        text
    );

    return pixmap;
}
