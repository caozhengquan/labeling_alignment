#include "labelingscreen.h"

LabelingScreen::LabelingScreen(GlobalInfo *ginfo, QWidget *parent):QFrame(parent),gf(ginfo)
{
    setFrameStyle(WinPanel | Sunken);
    setLineWidth(0);
    setFocusPolicy(Qt::NoFocus);

    image = new QImage;

    setMouseTracking(true);

    line_sate = Empty;

    connect(this, SIGNAL(changed()), parent, SLOT(refresh()));
}

void LabelingScreen::paintEvent(QPaintEvent *e)
{
    QFrame::paintEvent(e);
    QPainter painter(this);

    qreal scale = gf->lens.width()/width()*4;

    painter.drawImage(QRectF(QPointF(),size()), *image, gf->lens);
    painter.setWindow(gf->lens.toRect());

    painter.setPen(QPen(Qt::black, scale*2));
    painter.drawRect(gf->lens.toRect());

    painter.setPen(QPen(QColor(255, 0, 0, 100), scale));
    for(int i = 0; i < gf->label.size(); i++)
        painter.drawEllipse(gf->label[i],scale/2, scale/2);

    painter.setPen(QPen(QColor(255, 0, 0, 100), scale/2));

    if(gf->label.size() == 2)
        painter.drawLine(gf->label[0], gf->label[1]);

    if(gf->label.size() > 2)
    {
        for(int j = 0; j < gf->func.size(); j++)
        {
            double delta = gf->func[j].h / 100.0;
            for(int k = 0; k < 100; k++)
            {
                double t1 = delta*k;
                double t2 = delta*(k+1);
                double x1 = gf->func[j].a_x + gf->func[j].b_x*t1 + gf->func[j].c_x*pow(t1,2) + gf->func[j].d_x*pow(t1,3);
                double y1 = gf->func[j].a_y + gf->func[j].b_y*t1 + gf->func[j].c_y*pow(t1,2) + gf->func[j].d_y*pow(t1,3);
                double x2 = gf->func[j].a_x + gf->func[j].b_x*t2 + gf->func[j].c_x*pow(t2,2) + gf->func[j].d_x*pow(t2,3);
                double y2 = gf->func[j].a_y + gf->func[j].b_y*t2 + gf->func[j].c_y*pow(t2,2) + gf->func[j].d_y*pow(t2,3);

                painter.drawLine(QPointF(x1,y1), QPointF(x2, y2));
            }
        }
    }
}

void LabelingScreen::setImage(QImage* a)
{
    image = a;
    if(qreal(image->width()) / image->height() > qreal(width()) / height())
    {
        gf->lens.setX(0);
        gf->lens.setY((image->height() - image->width()*height()/width())/2);
        gf->lens.setWidth(image->width());
        gf->lens.setHeight(image->width()*height()/width());
    }
    else
    {
        gf->lens.setX((image->width() - image->height()*width()/height())/2);
        gf->lens.setY(0);
        gf->lens.setWidth(image->height()*width()/height());
        gf->lens.setHeight(image->height());
    }
    lens_bk = gf->lens;

    gf->label.clear();
    gf->func.clear();
    line_sate = Empty;

    emit changed();
}

void LabelingScreen::setLabel(vector<QPointF> p)
{
    gf->label.clear();
    for(int i = 0; i < p.size(); i++)
    {
        gf->label.push_back(p[i]);
    }
    line_sate = Over;
    if(gf->label.size() > 2)
    {
        gf->func = vector<Func>(gf->label.size() - 1);
        cal_fun(gf->label, gf->func);
    }
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
    if (f < 1 && w < 16 && h < 16)
        return;
    if (f > 1 && w > lens_bk.width() && h > lens_bk.height())
        return;
    //lens = QRectF(x, y, w, h);
    gf->lens.setX(x);
    gf->lens.setY(y);
    gf->lens.setWidth(w);
    gf->lens.setHeight(h);
    if(!inRect(lens_bk, gf->lens))
    {
        if(gf->lens.left() < lens_bk.left())
            gf->lens.setX(gf->lens.x() + lens_bk.left() - gf->lens.left());
        if(gf->lens.right() > lens_bk.right())
           gf->lens.setX(gf->lens.x() + lens_bk.right() - gf->lens.right());
        if(gf->lens.top() < lens_bk.top())
            gf->lens.setY(gf->lens.y() + lens_bk.top() - gf->lens.top());
        if(gf->lens.bottom() > lens_bk.bottom())
            gf->lens.setY(gf->lens.y() + lens_bk.bottom() - gf->lens.bottom());
        gf->lens.setWidth(w);
        gf->lens.setHeight(h);
    }

    //update();
    emit changed();
}

void LabelingScreen::mouseMoveEvent(QMouseEvent *e)
{
    mousepos = e->localPos();

    if(line_sate == On)
    {
        int point_num = gf->label.size();
        qreal scale = gf->lens.width()/width()*4;
        if(point_num > 3 && cal_dis(gf->label[0], toGlobal(e->localPos())) < scale*2)
            gf->label[point_num - 1] = gf->label[0];
        else
            gf->label[point_num - 1] = toGlobal(e->localPos());
        if (point_num > 2)
        {
            gf->func = vector<Func>(gf->label.size() - 1);
            cal_fun(gf->label, gf->func);
        }
        emit changed();
    }
}

void LabelingScreen::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        if(line_sate == Over)
            return;
        if(line_sate == Empty)
        {
            gf->label.push_back(toGlobal(e->localPos()));
            line_sate = On;
        }
        int point_num = gf->label.size();
        qreal scale = gf->lens.width()/width()*4;
        if(gf->label.size() > 1)
        {
            if( cal_dis(gf->label[point_num - 2], toGlobal(e->localPos())) <  scale/2)
                    return;
        }
        if(gf->label.size() > 3 && cal_dis(gf->label[0], toGlobal(e->localPos())) < scale*2)
        {
            line_sate = Over;
        }
        else
        {
            gf->label.push_back(gf->label[point_num - 1]);
        }
    }
    else
    {
        if(gf->label.size() > 1)
        {
            line_sate = Over;
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

void LabelingScreen::cal_fun(vector<QPointF> point_v, vector<Func> &func_v)
{
    int n = point_v.size();
    vector<double> Mx(n);
    vector<double> My(n);
    vector<double> A(n-2);
    vector<double> B(n-2);
    vector<double> C(n-2);
    vector<double> Dx(n-2);
    vector<double> Dy(n-2);
    vector<double> h(n-1);
    //vector<func> func_v(n-1);

    for(int i = 0; i < n-1; i++)
    {
        h[i] = sqrt(pow(point_v[i+1].x() - point_v[i].x(), 2) + pow(point_v[i+1].y() - point_v[i].y(), 2));
    }

    for(int i = 0; i < n-2; i++)
    {
        A[i] = h[i];
        B[i] = 2*(h[i]+h[i+1]);
        C[i] = h[i+1];

        Dx[i] =  6*( (point_v[i+2].x() - point_v[i+1].x())/h[i+1] - (point_v[i+1].x() - point_v[i].x())/h[i] );
        Dy[i] =  6*( (point_v[i+2].y() - point_v[i+1].y())/h[i+1] - (point_v[i+1].y() - point_v[i].y())/h[i] );
    }

    //TDMA
    C[0] = C[0] / B[0];
    Dx[0] = Dx[0] / B[0];
    Dy[0] = Dy[0] / B[0];
    for(int i = 1; i < n-2; i++)
    {
        double tmp = B[i] - A[i]*C[i-1];
        C[i] = C[i] / tmp;
        Dx[i] = (Dx[i] - A[i]*Dx[i-1]) / tmp;
        Dy[i] = (Dy[i] - A[i]*Dy[i-1]) / tmp;
    }
    Mx[n-2] = Dx[n-3];
    My[n-2] = Dy[n-3];
    for(int i = n-4; i >= 0; i--)
    {
        Mx[i+1] = Dx[i] - C[i]*Mx[i+2];
        My[i+1] = Dy[i] - C[i]*My[i+2];
    }

    Mx[0] = 0;
    Mx[n-1] = 0;
    My[0] = 0;
    My[n-1] = 0;

    for(int i = 0; i < n-1; i++)
    {
        func_v[i].a_x = point_v[i].x();
        func_v[i].b_x = (point_v[i+1].x() - point_v[i].x())/h[i] - (2*h[i]*Mx[i] + h[i]*Mx[i+1]) / 6;
        func_v[i].c_x = Mx[i]/2;
        func_v[i].d_x = (Mx[i+1] - Mx[i]) / (6*h[i]);

        func_v[i].a_y = point_v[i].y();
        func_v[i].b_y = (point_v[i+1].y() - point_v[i].y())/h[i] - (2*h[i]*My[i] + h[i]*My[i+1]) / 6;
        func_v[i].c_y = My[i]/2;
        func_v[i].d_y = (My[i+1] - My[i]) / (6*h[i]);

        func_v[i].h = h[i];
    }

}

bool LabelingScreen::inRect(QRectF f, QRectF s)
{
    if(f.left() < s.left() && f.top() < s.top() && f.right() > s.right() && f.bottom() > s.bottom())
        return true;
    return false;
}

double LabelingScreen::cal_dis(QPointF a, QPointF b)
{
    return sqrt(pow(a.x() - b.x(),2) +pow(a.y() - b.y(),2));
}


void LabelingScreen::delete_label()
{
    if(line_sate == Over)
    {
        gf->label.clear();
        gf->func.clear();
        line_sate = Empty;
        emit changed();
    }
}

int LabelingScreen::getLabel(vector<QPointF> &p)
{
    if(line_sate != Over)
        return 0;
    p.clear();
    for(int i = 0; i < gf->label.size(); i++)
    {
        p.push_back(gf->label[i]);
    }
    return 1;
}
