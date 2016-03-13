#ifndef COMMON_H
#define COMMON_H

#include <QImage>
#include <QFrame>
#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QtCore>

using namespace std;

struct Func{
    double a_x;
    double b_x;
    double c_x;
    double d_x;
    double a_y;
    double b_y;
    double c_y;
    double d_y;
    double h;
};


struct GlobalInfo{
    QRectF lens;
    vector<QPointF> label;
    vector<Func> func;
};

#endif // COMMON_H
