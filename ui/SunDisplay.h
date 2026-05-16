#ifndef SUNDISPLAY_H
#define SUNDISPLAY_H

#include <QLabel>

class SunDisplay : public QLabel
{
    Q_OBJECT
public:
    explicit SunDisplay(QWidget *parent = nullptr);
    void setSunValue(int value);
};

#endif
