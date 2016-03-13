#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget*widget = new QWidget(this) ;
    this->setCentralWidget(widget) ;

    mainLayout = new QGridLayout();
    mainLayout->setColumnStretch(0,2);
    mainLayout->setColumnStretch(1,2);
    mainLayout->setColumnStretch(2,4);
    mainLayout->setColumnStretch(3,1);
    mainLayout->setRowStretch(0,3);
    mainLayout->setRowStretch(1,3);
    mainLayout->setRowStretch(2,1);
    mainLayout->setMargin(30);
    mainLayout->setSpacing(20);

    gf = new GlobalInfo;

    showscreen = vector<ShowScreen*>(4);
    for(int i = 0; i < 4; i++)
    {
        showscreen[i] = new ShowScreen(gf, this);
    }
    labelscreen = new LabelingScreen(gf, this);

    mainLayout->addWidget(showscreen[0], 0, 0);
    mainLayout->addWidget(showscreen[1], 0, 1);
    mainLayout->addWidget(showscreen[2], 1, 0);
    mainLayout->addWidget(showscreen[3], 1, 1);
    mainLayout->addWidget(labelscreen, 0, 2, 2, 1);

    widget->setLayout(mainLayout);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString path;
    path = QFileDialog::getOpenFileName(this, "Open", "", " image file.(*.jpg *.png *.)");
    if (path.isEmpty())
        return;
    path = QFileInfo(path).absoluteFilePath();
    QString filename = QFileInfo(path).fileName();

    image = new QImage;
    image->load(path);


    labelscreen->setImage(image);
    for(int i = 0; i < 4; i++)
    {
        showscreen[i]->setImage(image);
    }

}

void MainWindow::on_actionSave_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{

}

void MainWindow::wheelEvent(QWheelEvent *e)
{
}

void MainWindow::refresh()
{
    labelscreen->update();
    for(int i = 0; i < 4; i++)
    {
        showscreen[i]->update();
    }
}
