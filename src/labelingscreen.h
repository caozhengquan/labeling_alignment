#ifndef LABELINGSCREEN_H
#define LABELINGSCREEN_H

#include "common.h"

using namespace std;

class LabelingScreen: public QFrame
{
    Q_OBJECT

public:
    LabelingScreen(GlobalInfo *ginfo, QWidget *parent);

    void setImage(QImage *a);
    void setLabel(vector<QPointF> p);
    void delete_label();
    int getLabel(vector<QPointF> &p);

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
    void cal_fun(vector<QPointF> point_v,  vector<Func> &func_v);
    bool inRect(QRectF f, QRectF s);
    double cal_dis(QPointF a, QPointF b);

signals:
    void changed();
};

#endif // LABELINGSCREEN_H
