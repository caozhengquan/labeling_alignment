#include "helperscreen.h"

HelperScreen::HelperScreen(QWidget *parent):QFrame(parent)
{
    setFrameStyle(WinPanel | Sunken);
    setLineWidth(0);
    setFocusPolicy(Qt::NoFocus);
}

void HelperScreen::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
    QPainter painter(this);

    QRect lens = QRect(0,0,image.width(), image.height());
    painter.drawImage(QRectF(QPointF(),size()), image, lens);
    painter.setWindow(lens);

}

void HelperScreen::setImage(QImage a)
{
    image = a;
    update();
}
