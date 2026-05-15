#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QPixmap>
#include <QVector>
#include <QString>
#include <QSize>
#include <QColor>

class ResourceManager {
public:
    static QPixmap loadPixmap(const QString& path);

    static QVector<QPixmap> loadFrames(
        const QString& folder,
        const QString& prefix,
        int count
    );

    static QPixmap createPlaceholderPixmap(
        QSize size,
        QColor color,
        const QString& text
    );
};

#endif // RESOURCEMANAGER_H
