#ifndef SHOWSCREEN_H
#define SHOWSCREEN_H

#include "common.h"

using namespace std;

class ShowScreen: public QFrame
{
    Q_OBJECT

public:
    ShowScreen(GlobalInfo *ginfo, QWidget *parent);

    void setImage(QImage *a);
    int getLabel(vector<QPointF> &p);

    bool choosed;

protected:
    void paintEvent(QPaintEvent *e);
    void wheelEvent(QWheelEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    QImage *image;
    QRectF lens_bk;
    QPointF mousepos;

    GlobalInfo *gf;

    enum Line_Sate {Empty, On, Over};
    Line_Sate line_sate;

    QPointF toGlobal(QPointF p);
    bool inRect(QRectF f, QRectF s);

signals:
    void changed();
    void choice_change();
    void change_image();
};

#endif // SHOWSCREEN_H
