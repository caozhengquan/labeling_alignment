#include "showscreen.h"

ShowScreen::ShowScreen(GlobalInfo *ginfo, QWidget *parent):QFrame(parent),gf(ginfo)
{
    setFrameStyle(WinPanel | Sunken);
    setLineWidth(0);
    setFocusPolicy(Qt::NoFocus);

    image = new QImage;

    setMouseTracking(true);

    line_sate = Empty;
    choosed = false;

    connect(this, SIGNAL(changed()), parent, SLOT(refresh()));
    connect(this, SIGNAL(choice_change()), parent, SLOT(refresh_choice()));
    connect(this, SIGNAL(change_image()), parent, SLOT(refresh_image()));
}

void ShowScreen::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
    QPainter painter(this);

    qreal scale = gf->lens.width()/width()*4;

    painter.drawImage(QRectF(QPointF(),size()), *image, gf->lens.toRect());
    painter.setWindow(gf->lens.toRect());

    if(choosed)
        painter.setPen(QPen(Qt::red, scale*3));
    else
        painter.setPen(QPen(Qt::black, scale*2));
    painter.drawRect(gf->lens.toRect());

    if(gf->label.size() > gf->image_no)
    {
        painter.setPen(QPen(QColor(255, 0, 0, 100), scale));
        for(int i = 0; i < gf->label[gf->image_no].size(); i++)
            for(int j = 0; j < gf->label[gf->image_no][i].size(); j++)
                painter.drawEllipse(gf->label[gf->image_no][i][j],scale/2, scale/2);

        painter.setPen(QPen(QColor(255, 0, 0, 100), scale/2));

        for(int i = 0; i < gf->label[gf->image_no].size(); i++)
        {
            if(gf->label[gf->image_no][i].size() == 2)
            painter.drawLine(gf->label[gf->image_no][i][0], gf->label[gf->image_no][i][1]);

            if(gf->label[gf->image_no][i].size() > 2)
            {
                for(int j = 0; j < gf->func[gf->image_no][i].size(); j++)
                {
                    double delta = gf->func[gf->image_no][i][j].h / 100.0;
                    for(int k = 0; k < 100; k++)
                    {
                        double t1 = delta*k;
                        double t2 = delta*(k+1);
                        double x1 = gf->func[gf->image_no][i][j].a_x + gf->func[gf->image_no][i][j].b_x*t1 + gf->func[gf->image_no][i][j].c_x*pow(t1,2) + gf->func[gf->image_no][i][j].d_x*pow(t1,3);
                        double y1 = gf->func[gf->image_no][i][j].a_y + gf->func[gf->image_no][i][j].b_y*t1 + gf->func[gf->image_no][i][j].c_y*pow(t1,2) + gf->func[gf->image_no][i][j].d_y*pow(t1,3);
                        double x2 = gf->func[gf->image_no][i][j].a_x + gf->func[gf->image_no][i][j].b_x*t2 + gf->func[gf->image_no][i][j].c_x*pow(t2,2) + gf->func[gf->image_no][i][j].d_x*pow(t2,3);
                        double y2 = gf->func[gf->image_no][i][j].a_y + gf->func[gf->image_no][i][j].b_y*t2 + gf->func[gf->image_no][i][j].c_y*pow(t2,2) + gf->func[gf->image_no][i][j].d_y*pow(t2,3);

                        painter.drawLine(QPointF(x1,y1), QPointF(x2, y2));
                    }
                }
            }
        }
    }

}

void ShowScreen::mousePressEvent(QMouseEvent *e)
{
    emit choice_change();
    choosed = true;
    update();
    emit change_image();
}

void ShowScreen::setImage(QImage* a)
{
    image = a;
    line_sate = Empty;

    update();
}


void ShowScreen::wheelEvent(QWheelEvent *e)
{
    qreal f = qPow(1.05, -e->angleDelta().y() / qreal(120));

    qreal ratiox = mousepos.x()/width();
    qreal ratioy = mousepos.y()/height();

    qreal x = gf->lens.x() - gf->lens.width() * (f - 1) * ratiox;
    qreal y = gf->lens.y() - gf->lens.height() * (f - 1) * ratioy;
    qreal w = gf->lens.width() * f, h = gf->lens.height() * f;
    if (f < 1 && w < 64 && h < 64)
        return;
    if (f > 1 && w > gf->lens_bk.width() && h > gf->lens_bk.height())
        return;
    //lens = QRectF(x, y, w, h);
    gf->lens.setX(x);
    gf->lens.setY(y);
    gf->lens.setWidth(w);
    gf->lens.setHeight(h);
    if(!inRect(gf->lens_bk, gf->lens))
    {
        if(gf->lens.left() < gf->lens_bk.left())
            gf->lens.setX(gf->lens.x() + gf->lens_bk.left() - gf->lens.left());
        if(gf->lens.right() > gf->lens_bk.right())
           gf->lens.setX(gf->lens.x() + gf->lens_bk.right() - gf->lens.right());
        if(gf->lens.top() < gf->lens_bk.top())
            gf->lens.setY(gf->lens.y() + gf->lens_bk.top() - gf->lens.top());
        if(gf->lens.bottom() > gf->lens_bk.bottom())
            gf->lens.setY(gf->lens.y() + gf->lens_bk.bottom() - gf->lens.bottom());
        gf->lens.setWidth(w);
        gf->lens.setHeight(h);
    }

    //update();
    emit changed();
}

void ShowScreen::mouseMoveEvent(QMouseEvent *e)
{
    mousepos = e->localPos();
}


QPointF ShowScreen::toGlobal(QPointF p)
{
    qreal x = p.x() / width() * gf->lens.width() + gf->lens.x();
    qreal y = p.y() / height() * gf->lens.height() + gf->lens.y();
    return QPointF(x, y);
}


bool ShowScreen::inRect(QRectF f, QRectF s)
{
    if(f.left() < s.left() && f.top() < s.top() && f.right() > s.right() && f.bottom() > s.bottom())
        return true;
    return false;
}

double ShowScreen::cal_dis(QPointF a, QPointF b)
{
    return sqrt(pow(a.x() - b.x(),2) +pow(a.y() - b.y(),2));
}

