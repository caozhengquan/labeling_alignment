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
    void delete_label();
    void update_linestate();

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
    Line_Sate line_state;

    QPointF toGlobal(QPointF p);
    bool inRect(QRectF f, QRectF s);
    void my_crop(QImage input_image, QRectF rect, QImage &output_image);

signals:
    void changed();
};

#endif // LABELINGSCREEN_H
