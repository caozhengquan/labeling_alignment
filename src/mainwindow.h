#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "labelingscreen.h"
#include "showscreen.h"

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QFileDialog>

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

    void refresh();

protected:
    void wheelEvent(QWheelEvent *e);

private:
    Ui::MainWindow *ui;

    QGridLayout *mainLayout;
    LabelingScreen *labelscreen;
    vector<ShowScreen *> showscreen;

    QImage *image;

    GlobalInfo *gf;


};

#endif // MAINWINDOW_H
