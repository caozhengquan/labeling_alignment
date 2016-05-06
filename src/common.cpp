#include "common.h"


QImage mat2Qimg(Mat a)
{
   QImage qimg(a.cols,a.rows, QImage::Format_RGB888 );

   if(a.channels() == 3)
   {
       Mat a_rgb = a;
       cvtColor(a, a_rgb, CV_BGR2RGB);
       memcpy(qimg.bits(), (const unsigned char*)a_rgb.data, a_rgb.cols*a_rgb.rows*a_rgb.channels());
   }
   else
   {
       Mat a_gray;
       cvtColor(a, a_gray, CV_BGR2GRAY);
       memcpy(qimg.bits(), (const unsigned char*)a_gray.data, a_gray.cols*a_gray.rows*a_gray.channels());
   }
   return qimg;
}

Mat Qimg2mat(QImage qimg)
{

    if(qimg.format() == QImage::Format_Grayscale8)
    {
        Mat a(qimg.height(), qimg.width(), CV_8UC1);
        memcpy((uchar*)a.data, qimg.bits(), a.cols*a.rows*a.channels());
        return a;
    }

    Mat a(qimg.height(), qimg.width(), CV_8UC3);
    memcpy((uchar*)a.data, qimg.bits(), a.cols*a.rows*a.channels());
    return a;
}

QRectF toQrect(cv::Rect_<float> rect)
{
    return QRectF(qreal(rect.x), qreal(rect.y), qreal(rect.width), qreal(rect.height));
}

cv::Rect_<float> toCVrect(QRectF rect)
{
    return cv::Rect_<float>(rect.x(), rect.y(), rect.width(), rect.height());
}


void cal_fun(vector<QPointF> point_v, vector<Func> &func_v)
{
    if(point_v.size() <= 2)
    {
        func_v.clear();
        return;
    }

    int n = point_v.size();
    func_v.clear();
    func_v.resize(n-1);


    vector<double> Mx(n);
    vector<double> My(n);
    vector<double> A(n-2);
    vector<double> B(n-2);
    vector<double> C(n-2);
    vector<double> Dx(n-2);
    vector<double> Dy(n-2);
    vector<double> h(n-1);

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

void CalcAffineMatByPose(const vector<QPointF> &source, const vector<QPointF> &target, Mat &affine_mat)
{
    int point_num=source.size();

    Mat X(2*point_num, 4, CV_32F);
    Mat U(2*point_num, 1, CV_32F);
    for (unsigned int i=0;i<point_num;i++) {
        X.at<float>(i, 0)=source[i].x();
        X.at<float>(i+point_num, 0)=source[i].y();
        X.at<float>(i, 1)=source[i].y();
        X.at<float>(i+point_num, 1)=-1*source[i].x();
        X.at<float>(i, 2)=1;
        X.at<float>(i+point_num, 3)=1;
        X.at<float>(i, 3)=0;
        X.at<float>(i+point_num, 2)=0;

        U.at<float>(i, 0)=target[i].x();
        U.at<float>(i+point_num, 0)=target[i].y();
    }
    Mat result=X.inv(DECOMP_SVD)*U;

    affine_mat.create(2, 3, CV_32F);
    affine_mat.at<float>(0, 0)=result.at<float>(0, 0);
    affine_mat.at<float>(0, 1)=result.at<float>(1, 0);
    affine_mat.at<float>(0, 2)=result.at<float>(2, 0);
    affine_mat.at<float>(1, 0)=-1*result.at<float>(1, 0);
    affine_mat.at<float>(1, 1)=result.at<float>(0, 0);
    affine_mat.at<float>(1, 2)=result.at<float>(3, 0);
}

void InvAffinePose(const Mat &affine_mat, vector<QPointF> &pose)
{
    for(size_t i=0;i<pose.size();i++) {
        pose[i].setX(pose[i].x() - affine_mat.at<float>(0, 2));
        pose[i].setY(pose[i].y() - affine_mat.at<float>(1, 2));
    }

    float scale=affine_mat.at<float>(0, 0)*affine_mat.at<float>(0, 0)+affine_mat.at<float>(0, 1)*affine_mat.at<float>(0, 1);
    float inv_00=affine_mat.at<float>(0, 0)/scale;
    float inv_01=-1*affine_mat.at<float>(0, 1)/scale;
    float inv_10=-1*inv_01;
    float inv_11=inv_00;

    for(size_t i=0;i<pose.size();i++) {
        QPointF inv_point;
        inv_point.setX(inv_00*pose[i].x()+inv_01*pose[i].y());
        inv_point.setY(inv_10*pose[i].x()+inv_11*pose[i].y());
        pose[i]=inv_point;
    }
}

void InvAffinePose(const Mat &affine_mat, vector<QPointF> pose, vector<QPointF> &pose_new)
{
    pose_new.resize(pose.size());
    for(size_t i=0;i<pose.size();i++) {
        pose_new[i].setX(pose[i].x() - affine_mat.at<float>(0, 2));
        pose_new[i].setY(pose[i].y() - affine_mat.at<float>(1, 2));
    }

    float scale=affine_mat.at<float>(0, 0)*affine_mat.at<float>(0, 0)+affine_mat.at<float>(0, 1)*affine_mat.at<float>(0, 1);
    float inv_00=affine_mat.at<float>(0, 0)/scale;
    float inv_01=-1*affine_mat.at<float>(0, 1)/scale;
    float inv_10=-1*inv_01;
    float inv_11=inv_00;

    for(size_t i=0;i<pose.size();i++) {
        QPointF inv_point;
        inv_point.setX(inv_00*pose_new[i].x()+inv_01*pose_new[i].y());
        inv_point.setY(inv_10*pose_new[i].x()+inv_11*pose_new[i].y());
        pose_new[i]=inv_point;
    }
}

void AffinePose(const Mat &affine_mat, vector<QPointF> pose, vector<QPointF> &pose_new)
{
    pose_new.resize(pose.size());
    for(size_t i=0;i<pose.size();i++) {
        qreal x = pose[i].x();
        qreal y = pose[i].y();
        pose_new[i].setX(x*affine_mat.at<float>(0, 0) + y*affine_mat.at<float>(0, 1) + affine_mat.at<float>(0, 2));
        pose_new[i].setY(x*affine_mat.at<float>(1, 0) + y*affine_mat.at<float>(1, 1) + affine_mat.at<float>(1, 2));
    }
}

void AffinePose(const Mat &affine_mat, vector<QPointF> &pose)
{
    for(size_t i=0;i<pose.size();i++) {
        qreal x = pose[i].x();
        qreal y = pose[i].y();
        pose[i].setX(x*affine_mat.at<float>(0, 0) + y*affine_mat.at<float>(0, 1) + affine_mat.at<float>(0, 2));
        pose[i].setY(x*affine_mat.at<float>(1, 0) + y*affine_mat.at<float>(1, 1) + affine_mat.at<float>(1, 2));
    }
}

void my_laplace(cv::Mat& srcImg, cv::Mat& dstImg)
{
    Mat kernel(3,3,CV_32F,Scalar(-1));
    kernel.at<float>(1,1) = 8.9;
    filter2D(srcImg,dstImg,srcImg.depth(),kernel);
    //cvtColor(dstImg, dstImg, CV_RGB2BGR);
}

void my_Sobel(cv::Mat& srcImg, cv::Mat& dstImg)
{
    Mat dst_x, dst_y;
    Sobel(srcImg, dst_x, srcImg.depth(), 1, 0, 3);
    Sobel(srcImg, dst_y, srcImg.depth(), 0, 1, 3);
    convertScaleAbs(dst_x, dst_x);
    convertScaleAbs(dst_y, dst_y);
    addWeighted( dst_x, 0.5, dst_y, 0.5, 0, dstImg);

    if(dstImg.channels() == 3)
    {
        cvtColor(dstImg, dstImg, CV_BGR2GRAY);
        cvtColor(dstImg, dstImg, CV_GRAY2BGR);
    }

}

void my_canny(cv::Mat& srcImg, cv::Mat& dstImg)
{
    if(srcImg.channels() == 3)
        cvtColor(srcImg, srcImg, CV_BGR2GRAY);
    Canny(srcImg, dstImg, 50, 80);
    if(dstImg.channels() == 1)
    {
        cvtColor(dstImg, dstImg, CV_GRAY2BGR);
    }
}

double cal_dis(QPointF a, QPointF b)
{
    return sqrt(pow(a.x() - b.x(),2) +pow(a.y() - b.y(),2));
}

