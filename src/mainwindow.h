#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "labelingscreen.h"
#include "showscreen.h"
#include "helperscreen.h"

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include <QSlider>
#include <QComboBox>
#include <QTextCodec>
#include <QPushButton>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_slider_valueChanged(int);
    void on_combobox_valueChanged(int);
    void on_deleteButton_clicked();

    void refresh();
    void refresh_choice();
    void refresh_image();

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    Ui::MainWindow *ui;

    QGridLayout *mainLayout;
    QGridLayout *controlLayout;
    LabelingScreen *labelscreen;
    vector<ShowScreen *> showscreen;
    QPushButton *deleteButton;

    QSlider *slider;
    QComboBox *combobox;
    QLabel *sliderLabel;
    HelperScreen *helperLabel;

    enum State {Empty, Normal, Begin};
    State state;

    vector<QString> image_list;
    vector<vector<QPointF> > label21pt;
    vector<QRect> labelfacerect;
    vector<int> labelrotate;
    QString folderpath;
    QString image_folderpath;
    vector<QPointF> mean_pose;
    vector<Mat> affine_mats;

    QImage *image;
    QImage *image_sobel;
    QImage *image_laplace;
    QImage *image_canny;
    vector<QImage> helper_image;
    GlobalInfo *gf;

    void update_lens();
    void init_label();
};

#endif // MAINWINDOW_H
