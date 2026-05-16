#include "SunDisplay.h"

SunDisplay::SunDisplay(QWidget *parent)
    : QLabel(parent)
{
    setFixedHeight(30);
    setStyleSheet("font-size: 16px; font-weight: bold; color: #FFD700;");
    setSunValue(150);
}

void SunDisplay::setSunValue(int value)
{
    setText(QString("Sun: %1").arg(value));
}
