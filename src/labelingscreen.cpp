#include "labelingscreen.h"

LabelingScreen::LabelingScreen(GlobalInfo *ginfo, QWidget *parent):QFrame(parent),gf(ginfo)
{
    setFrameStyle(WinPanel | Sunken);
    setLineWidth(0);
    setFocusPolicy(Qt::NoFocus);

    image = new QImage;

    setMouseTracking(true);

    line_state = Empty;

    connect(this, SIGNAL(changed()), parent, SLOT(refresh()));
}

void LabelingScreen::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
    QPainter painter(this);

    qreal scale = gf->lens.width()/width()*4;

    painter.drawImage(QRectF(QPointF(),size()), *image, gf->lens.toRect());
    painter.setWindow(gf->lens.toRect());

    painter.setPen(QPen(Qt::black, scale/3));
    QPointF glopos = toGlobal(mousepos);
    painter.drawLine(QPointF(0, glopos.y()), QPointF(image->width(), glopos.y()));
    painter.drawLine(QPointF(glopos.x(), 0), QPointF(glopos.x(), image->height()));

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
            if(i == int(LeftEyebrow) || i == int(RightEyebrow))
                continue;

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

void LabelingScreen::setImage(QImage* a)
{
    image = a;
    if(gf->label[gf->image_no][int(gf->aligntpye)].size() > 0)
        line_state = Over;
    else
        line_state = Empty;
    emit changed();
}

void LabelingScreen::wheelEvent(QWheelEvent *e)
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

void LabelingScreen::mouseMoveEvent(QMouseEvent *e)
{
    mousepos = e->localPos();

    if(line_state == On)
    {
        if(gf->aligntpye == LeftEyebrow || gf->aligntpye == RightEyebrow)
        {
            //do nothing
        }
        else
        {
            int point_num = gf->label[gf->image_no][int(gf->aligntpye)].size();
            qreal scale = gf->lens.width()/width()*4;
            if(point_num > 3 && cal_dis(gf->label[gf->image_no][int(gf->aligntpye)][0], toGlobal(e->localPos())) < scale*2)
                gf->label[gf->image_no][int(gf->aligntpye)][point_num - 1] = gf->label[gf->image_no][int(gf->aligntpye)][0];
            else
                gf->label[gf->image_no][int(gf->aligntpye)][point_num - 1] = toGlobal(e->localPos());

            if(gf->pair_type != -1 && gf->label[gf->image_no][gf->pair_type].size() > 0)
            {
                if(point_num > 3 && cal_dis(gf->label[gf->image_no][gf->pair_type][0], toGlobal(e->localPos())) < scale*2)
                    gf->label[gf->image_no][int(gf->aligntpye)][point_num - 1] = gf->label[gf->image_no][gf->pair_type][0];
                int size_tmp = gf->label[gf->image_no][gf->pair_type].size();
                if(point_num > 3 && cal_dis(gf->label[gf->image_no][gf->pair_type][size_tmp - 1], toGlobal(e->localPos())) < scale*2)
                    gf->label[gf->image_no][int(gf->aligntpye)][point_num - 1] = gf->label[gf->image_no][gf->pair_type][size_tmp - 1];
            }

            if (point_num > 2)
            {
                cal_fun(gf->label[gf->image_no][int(gf->aligntpye)], gf->func[gf->image_no][int(gf->aligntpye)]);
            }
        }

    }

    emit changed();
}

void LabelingScreen::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        if(gf->aligntpye == LeftEyebrow || gf->aligntpye == RightEyebrow)
        {
            if(line_state == Empty)
            {
                gf->label[gf->image_no][int(gf->aligntpye)].push_back(toGlobal(e->localPos()));
                line_state = Over;
            }
            gf->label[gf->image_no][int(gf->aligntpye)].push_back(toGlobal(e->localPos()));
        }
        else
        {
            qreal scale = gf->lens.width()/width()*4;
            if(line_state == Over)
                return;
            if(line_state == Empty)
            {
                gf->label[gf->image_no][int(gf->aligntpye)].push_back(toGlobal(e->localPos()));
                line_state = On;
                if(gf->pair_type != -1 && gf->label[gf->image_no][gf->pair_type].size() > 0)
                {
                    if(cal_dis(gf->label[gf->image_no][gf->pair_type][0], toGlobal(e->localPos())) < scale*2)
                        gf->label[gf->image_no][int(gf->aligntpye)][0] = gf->label[gf->image_no][gf->pair_type][0];
                    int size_tmp = gf->label[gf->image_no][gf->pair_type].size();
                    if(cal_dis(gf->label[gf->image_no][gf->pair_type][size_tmp - 1], toGlobal(e->localPos())) < scale*2)
                        gf->label[gf->image_no][int(gf->aligntpye)][0] = gf->label[gf->image_no][gf->pair_type][size_tmp - 1];
                }
            }
            int point_num = gf->label[gf->image_no][int(gf->aligntpye)].size();
            if(point_num > 1)
            {
                if( cal_dis(gf->label[gf->image_no][int(gf->aligntpye)][point_num - 2], toGlobal(e->localPos())) <  scale/2)
                        return;
            }

            if(gf->pair_type != -1 && gf->label[gf->image_no][gf->pair_type].size() > 0)
            {
                if(point_num > 3 && cal_dis(gf->label[gf->image_no][gf->pair_type][0], toGlobal(e->localPos())) < scale*2)
                    line_state = Over;
                int size_tmp = gf->label[gf->image_no][gf->pair_type].size();
                if(point_num > 3 && cal_dis(gf->label[gf->image_no][gf->pair_type][size_tmp - 1], toGlobal(e->localPos())) < scale*2)
                    line_state = Over;
            }

            if(point_num > 3 && cal_dis(gf->label[gf->image_no][int(gf->aligntpye)][0], toGlobal(e->localPos())) < scale*2)
            {
                line_state = Over;
            }
            else if(line_state != Over)
            {
                if(point_num > 2)
                    cal_fun(gf->label[gf->image_no][int(gf->aligntpye)], gf->func[gf->image_no][int(gf->aligntpye)]);
                gf->label[gf->image_no][int(gf->aligntpye)].push_back(gf->label[gf->image_no][int(gf->aligntpye)][point_num - 1]);
            }
        }

    }
    else
    {
        if(gf->label[gf->image_no][int(gf->aligntpye)].size() > 1)
        {
//            gf->label[gf->line_no].pop_back();
//            gf->func[gf->line_no] = vector<Func>(gf->label[gf->line_no].size() - 1);
//            cal_fun(gf->label[gf->line_no], gf->func[gf->line_no]);
            line_state = Over;
        }
    }
    emit changed();
}

QPointF LabelingScreen::toGlobal(QPointF p)
{
    qreal x = p.x() / width() * gf->lens.width() + gf->lens.x();
    qreal y = p.y() / height() * gf->lens.height() + gf->lens.y();
    return QPointF(x, y);
}


bool LabelingScreen::inRect(QRectF f, QRectF s)
{
    if(f.left() < s.left() && f.top() < s.top() && f.right() > s.right() && f.bottom() > s.bottom())
        return true;
    return false;
}


void LabelingScreen::delete_label()
{
    if(line_state == Over)
    {
        gf->label[gf->image_no][int(gf->aligntpye)].clear();
        gf->func[gf->image_no][int(gf->aligntpye)].clear();
        line_state = Empty;
        emit changed();
    }
}

void LabelingScreen::update_linestate()
{
    if(gf->label[gf->image_no][int(gf->aligntpye)].size() > 0)
        line_state = Over;
    else
        line_state = Empty;
}

void LabelingScreen::set_linestate(Line_State a)
{
    line_state = a;
}

