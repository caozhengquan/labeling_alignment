#ifndef COMMON_H
#define COMMON_H

#include <QImage>
#include <QFrame>
#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QtCore>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define BasePtNum 21
#define SIZE_I 2000
#define LineNum 13

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

enum AlignType {LeftEyeUp, LeftEyeDown, RightEyeUp, RightEyeDown, LeftEyebrow, RightEyebrow,
                NoseOut, NoseIn, MouseOutUp, MouseOutDown, MouseInUp, MouseInDown, Face};


struct GlobalInfo{
    QRectF lens;
    QRectF lens_bk;
    vector<vector<vector<QPointF> > > label;
    vector<vector<vector<Func> > > func;
    int image_no;
    AlignType aligntpye;
    int pair_type;
};

QImage mat2Qimg(Mat a);
Mat Qimg2mat(QImage a);
QRectF toQrect(cv::Rect_<float> rect);
cv::Rect_<float> toCVrect(QRectF rect);
double cal_dis(QPointF a, QPointF b);
void cal_fun(vector<QPointF> point_v, vector<Func> &func_v);
void CalcAffineMatByPose(const vector<QPointF> &source, const vector<QPointF> &target, Mat &affine_mat);
void InvAffinePose(const Mat &affine_mat, vector<QPointF> &pose);
void InvAffinePose(const Mat &affine_mat, vector<QPointF> pose, vector<QPointF> &pose_new);
void AffinePose(const Mat &affine_mat, vector<QPointF> pose, vector<QPointF> &pose_new);
void AffinePose(const Mat &affine_mat, vector<QPointF> &pose);

void my_laplace(cv::Mat& srcImg, cv::Mat& dstImg);
void my_Sobel(cv::Mat& srcImg, cv::Mat& dstImg);
void my_canny(cv::Mat& srcImg, cv::Mat& dstImg);

#endif // COMMON_H
