#ifndef HELPERSCREEN_H
#define HELPERSCREEN_H
#include "common.h"

class HelperScreen: public QFrame
{
    Q_OBJECT

public:
    HelperScreen(QWidget *parent);
    void setImage(QImage a);
private:
    QImage image;
protected:
    void paintEvent(QPaintEvent *e);
};

#endif // HELPERSCREEN_H

