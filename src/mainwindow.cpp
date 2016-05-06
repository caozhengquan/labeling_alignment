#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget*widget = new QWidget(this) ;
    this->setCentralWidget(widget) ;

    for(int i = 0; i < LineNum; i++)
    {
        QImage a;
        a.load("./helper_image/" + QString::number(i,10) + ".jpg");
        helper_image.push_back(a);
    }

    QFile file("./mean_pose.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line;
    for(int i = 0; i < BasePtNum; i++)
    {
        QPointF point_tmp;
        line = in.readLine();
        point_tmp.setX(line.toDouble()/256*SIZE_I);
        line = in.readLine();
        point_tmp.setY(line.toDouble()/256*SIZE_I);
        mean_pose.push_back(point_tmp);
    }

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

    controlLayout = new QGridLayout(this);
    controlLayout->setRowStretch(0,1);
    controlLayout->setRowStretch(1,1);
    controlLayout->setRowStretch(2,1);
    controlLayout->setRowStretch(3,1);
    controlLayout->setRowStretch(4,1);

    labelLayout = new QGridLayout(this);
    labelLayout->setColumnStretch(0,1);
    labelLayout->setColumnStretch(1,1);

    gf = new GlobalInfo;
    gf->aligntpye = LeftEyeUp;
    gf->image_no = 0;
    gf->pair_type = int(LeftEyeDown);

    showscreen = vector<ShowScreen*>(4);
    for(int i = 0; i < 4; i++)
    {
        showscreen[i] = new ShowScreen(gf, this);
    }
    labelscreen = new LabelingScreen(gf, this);
    sliderEdit = new QLineEdit(this);
    slider = new QSlider(Qt::Horizontal,this);
    sliderLabel = new QLabel(this);
    combobox = new QComboBox(this);
    helperLabel = new HelperScreen(this);
    QStringList typelist;
    typelist.push_back("左眼上沿");
    typelist.push_back("左眼下沿");
    typelist.push_back("右眼上沿");
    typelist.push_back("右眼下沿");
    typelist.push_back("左眉");
    typelist.push_back("右眉");
    typelist.push_back("鼻子");
    typelist.push_back("鼻梁");
    typelist.push_back("上唇上沿");
    typelist.push_back("上唇下沿");
    typelist.push_back("下唇上沿");
    typelist.push_back("下唇下沿");
    typelist.push_back("脸庞");

    combobox->addItems(typelist);
    QFont font("Microsoft YaHei",12,QFont::Bold);
    combobox->setFont(font);
    combobox->setMinimumHeight(30);
    combobox->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    deleteButton = new QPushButton(this);
    QFont font2("Microsoft YaHei",12,QFont::Bold);
    deleteButton->setText("删除");
    deleteButton->setFont(font2);
    deleteButton->setMinimumHeight(40);

    sliderLabel->setText("/ A");
    sliderLabel->setAlignment(Qt::AlignLeft);
    sliderLabel->setFont(font);
    sliderEdit->setText("N");
    sliderEdit->setAlignment(Qt::AlignRight);
    sliderEdit->setFont(font);
    sliderEdit->setEnabled(false);
    sliderEdit->setValidator(new QIntValidator(1, 9999, this));


    mainLayout->addWidget(showscreen[0], 0, 0);
    mainLayout->addWidget(showscreen[1], 0, 1);
    mainLayout->addWidget(showscreen[2], 1, 0);
    mainLayout->addWidget(showscreen[3], 1, 1);
    mainLayout->addWidget(labelscreen, 0, 2, 2, 1);
    mainLayout->addWidget(slider, 2, 0, 1, 3);
    controlLayout->addWidget(combobox, 0, 0);
    controlLayout->addWidget(helperLabel,1,0);
    controlLayout->addWidget(deleteButton, 2, 0);
    labelLayout->addWidget(sliderEdit, 0, 0);
    labelLayout->addWidget(sliderLabel,0,1);
    mainLayout->addLayout(controlLayout, 0, 3, 3, 1);
    mainLayout->addLayout(labelLayout, 2, 3);

    widget->setLayout(mainLayout);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_slider_valueChanged(int)));
    connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_combobox_valueChanged(int)));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(on_deleteButton_clicked()));
    connect(sliderEdit, SIGNAL(returnPressed()), this, SLOT(on_silderEdit_valueChanged()));


    state = Empty;
    helperLabel->setImage(helper_image[int(gf->aligntpye)]);
    helperLabel->update();

    image = new QImage;
    image_sobel = new QImage;
    image_laplace = new QImage;
    image_canny = new QImage;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    if(state == Normal || state == Begin)
    {
//        QMessageBox::StandardButton reply;
//        reply = QMessageBox::question(this,"Warning",tr("Save result or not?"),
//                QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok);
//        if (reply == QMessageBox::Ok)
            on_actionSave_triggered();
    }

    QString path;
    path = QFileDialog::getOpenFileName(this, "Open", "", " list file.(*.txt)");
    if (path.isEmpty())
        return;
    QString list_filename = QFileInfo(path).absoluteFilePath();
    folderpath = QFileInfo(path).absoluteDir().absolutePath();
    folderpath += "/";
    image_folderpath = folderpath + "Image/";

    QFile file(list_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    image_list.clear();
    label21pt.clear();
    labelfacerect.clear();
    labelrotate.clear();

    QTextStream in(&file);
    vector<QPointF> point_v(BasePtNum);
    QRect tmp_rect;
    QString line = in.readLine();
    while (!line.isNull())
    {
        QStringList line_list = line.split("\t");
        if(line_list.size() == 4)
        {
            image_list.push_back(line_list[0]);

            QStringList rect_list = line_list[1].split(" ");
            tmp_rect.setLeft(rect_list[0].toInt());
            tmp_rect.setTop(rect_list[1].toInt());
            tmp_rect.setRight(rect_list[2].toInt());
            tmp_rect.setBottom(rect_list[3].toInt());
            labelfacerect.push_back(tmp_rect);

            labelrotate.push_back(line_list[2].toInt());

            QStringList point_list = line_list[3].split(" ");
            for(int i = 0; i < BasePtNum; i++)
            {
             point_v[i].setX(point_list[2*i].toDouble());
             point_v[i].setY(point_list[2*i + 1].toDouble());
            }
            label21pt.push_back(point_v);
        }
        line = in.readLine();
    }
    in.flush();
    file.close();

    affine_mats.clear();
    affine_mats.resize(image_list.size());
    for(int i = 0; i < image_list.size(); i++)
    {
        CalcAffineMatByPose(label21pt[i], mean_pose, affine_mats[i]);
    }

    state = Begin;
    gf->image_no = 0;
    init_label();
    showscreen[0]->choosed = true;

    sliderEdit->setEnabled(true);

    slider->setMaximum(image_list.size());
    slider->setMinimum(1);
    slider->setSingleStep(1);
    slider->setValue(2);
    slider->setValue(1);

}

void MainWindow::on_actionSave_triggered()
{
    if(state == Normal || state == Begin)
    {
        QFile file(folderpath + "res.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        for(int i = 0; i < image_list.size(); i++)
        {
            out << image_list[i] << "\t" << labelrotate[i] << "\n";
            for(int j = 0; j < LineNum; j++)
            {
                if(gf->label[i][j].size() > 0)
                {
                    out << "1\t";
                    vector<QPointF> save_label;
                    InvAffinePose(affine_mats[i], gf->label[i][j], save_label);
                    for(int k = 0; k < save_label.size(); k++)
                    {
                        out << save_label[k].x() << "\t" << save_label[k].y() << "\t";
                    }
                    out << "\n";
                }
                else
                {
                    out << "0\n";
                }
            }
        }
        //out.flush();
        file.close();
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::refresh()
{
    labelscreen->update();
    for(int i = 0; i < 4; i++)
    {
        showscreen[i]->update();
    }
}

void MainWindow::refresh_choice()
{
    for(int i = 0; i < 4; i++)
        showscreen[i]->choosed = false;
    refresh();
}

void MainWindow::refresh_image()
{
    for(int i = 0; i < 4; i++)
    {
        if(showscreen[i]->choosed == true)
        {
            switch (i) {
            case 1:
                labelscreen->setImage(image_laplace);
                break;
            case 2:
                labelscreen->setImage(image_sobel);
                break;
            case 3:
                labelscreen->setImage(image_canny);
                break;
            default:
                labelscreen->setImage(image);
                break;
            }
            break;
        }
    }
    refresh();
}

void MainWindow::on_slider_valueChanged(int value)
{
    if(state == Normal || state == Begin)
    {   
        gf->image_no = value - 1;
        Mat a = imread(image_folderpath.toStdString() + image_list[gf->image_no].toStdString());

        labelrotate[gf->image_no] = (labelrotate[gf->image_no] + 360) % 360;
        if(labelrotate[gf->image_no] == 90)
            flip(a.t(), a, 1);
        if(labelrotate[gf->image_no] == 270)
            flip(a.t(), a, 0);
        if(labelrotate[gf->image_no] == 180)
            flip(a, a, -1);

        Mat b;
        warpAffine(a, b, affine_mats[gf->image_no], Size(SIZE_I*1.5, SIZE_I*1.5));
        //image = new QImage(mat2Qimg(b));
        image->swap(mat2Qimg(b));
        //labelscreen->setImage(image);

        showscreen[0]->setImage(image);

        Mat a_laplace, b_laplace;
        my_laplace(a, a_laplace);
        warpAffine(a_laplace, b_laplace, affine_mats[gf->image_no], Size(SIZE_I*1.5, SIZE_I*1.5));
        //image_laplace = new QImage(mat2Qimg(b_laplace));
        image_laplace->swap(mat2Qimg(b_laplace));
        showscreen[1]->setImage(image_laplace);

        Mat a_sobel, b_sobel;
        my_Sobel(a, a_sobel);
        warpAffine(a_sobel, b_sobel, affine_mats[gf->image_no], Size(SIZE_I*1.5, SIZE_I*1.5));
        //image_sobel = new QImage(mat2Qimg(b_sobel));
        image_sobel->swap(mat2Qimg(b_sobel));
        showscreen[2]->setImage(image_sobel);

        Mat a_canny, b_canny;
        my_canny(a, a_canny);
        warpAffine(a_canny, b_canny, affine_mats[gf->image_no], Size(SIZE_I*1.5, SIZE_I*1.5));
        //image_canny = new QImage(mat2Qimg(b_canny));
        image_canny->swap(mat2Qimg(b_canny));
        showscreen[3]->setImage(image_canny);

        refresh_image();

        //set lens_bk
        if(qreal(image->width()) / image->height() > qreal(labelscreen->width()) / labelscreen->height())
        {
            gf->lens_bk.setX(0);
            gf->lens_bk.setY((image->height() - image->width()*labelscreen->height()/labelscreen->width())/2);
            gf->lens_bk.setWidth(image->width());
            gf->lens_bk.setHeight(image->width()*labelscreen->height()/labelscreen->width());
        }
        else
        {
            gf->lens_bk.setX((image->width() - image->height()*labelscreen->width()/labelscreen->height())/2);
            gf->lens_bk.setY(0);
            gf->lens_bk.setWidth(image->height()*labelscreen->width()/labelscreen->height());
            gf->lens_bk.setHeight(image->height());
        }
        gf->lens = gf->lens_bk;
        update_lens();

        sliderLabel->setText("/" + QString::number(image_list.size()));
        sliderEdit->setText(QString::number(value));

//        if(value == 1)
//        {
//            state = Begin;
//            combobox->setEnabled(true);
//        }
//        else
//        {
//            state = Normal;
//            combobox->setEnabled(false);
//        }
    }
}

void MainWindow::on_combobox_valueChanged(int value)
{
    gf->aligntpye = AlignType(value);
    switch(gf->aligntpye)
    {
    case LeftEyeUp:
        gf->pair_type = int(LeftEyeDown);
        break;
    case LeftEyeDown:
        gf->pair_type = int(LeftEyeUp);
        break;
    case RightEyeUp:
        gf->pair_type = int(RightEyeDown);
        break;
    case RightEyeDown:
        gf->pair_type = int(RightEyeUp);
        break;
    case MouseOutUp:
        gf->pair_type = int(MouseOutDown);
        break;
    case MouseInUp:
        gf->pair_type = int(MouseInDown);
        break;
    case MouseInDown:
        gf->pair_type = int(MouseInUp);
        break;
    case MouseOutDown:
        gf->pair_type = int(MouseOutUp);
        break;
    default:
        gf->pair_type = -1;
        break;
    }
    helperLabel->setImage(helper_image[int(gf->aligntpye)]);
    helperLabel->update();
    if(state == Normal || state == Begin)
    {
        labelscreen->update_linestate();
        update_lens();
        refresh();
    }
}

void MainWindow::update_lens()
{
    //set lens
    vector<QPointF> label21pt_affine;
    AffinePose(affine_mats[gf->image_no], label21pt[gf->image_no], label21pt_affine);
    QPointF center;
    float width;
    switch (gf->aligntpye) {
    case LeftEyeUp:
    case LeftEyeDown:
        center = label21pt_affine[17];
        width = std::max(center.x() - label21pt_affine[8].x(), center.x() - label21pt_affine[9].x())*3;
        break;
    case RightEyeUp:
    case RightEyeDown:
        center = label21pt_affine[16];
        width = std::max(center.x() - label21pt_affine[6].x() , center.x() - label21pt_affine[7].x())*3;
        break;
    case LeftEyebrow:
        center = label21pt_affine[4];
        width = std::max(center.x() - label21pt_affine[3].x(), center.x() - label21pt_affine[5].x())*3;
        break;
    case RightEyebrow:
        center = label21pt_affine[1];
        width = std::max(center.x() - label21pt_affine[0].x(), center.x() - label21pt_affine[2].x())*3;
        break;
    case NoseOut:
    case NoseIn:
        center = label21pt_affine[18];
        width = std::max(center.x() - label21pt_affine[10].x(), center.x() - label21pt_affine[12].x())*4;
        center.setY(center.y() - width/4);
        break;
    case MouseOutUp:
    case MouseOutDown:
    case MouseInUp:
    case MouseInDown:
        center = label21pt_affine[14];
        width = std::max(center.x() - label21pt_affine[19].x(), center.x() - label21pt_affine[20].x())*4;
        break;
    default:
        vector<QPointF> face_rect;
        face_rect.push_back(QPointF(labelfacerect[gf->image_no].x(), labelfacerect[gf->image_no].y()));
        face_rect.push_back(QPointF(labelfacerect[gf->image_no].x() + labelfacerect[gf->image_no].width(), labelfacerect[gf->image_no].y() + labelfacerect[gf->image_no].height()));
        AffinePose(affine_mats[gf->image_no], face_rect);
        center.setX( (face_rect[0].x() + face_rect[1].x())/2 );
        center.setY( (face_rect[0].y() + face_rect[1].y())/2 );
        width = (face_rect[1].x() - face_rect[0].x())*1.5;
        break;
    }
    float height = width * gf->lens_bk.height()/gf->lens_bk.width();
    gf->lens.setX(center.x() - width/2);
    gf->lens.setY(center.y() - height/2);
    gf->lens.setWidth(width);
    gf->lens.setHeight(height);

}

void MainWindow::init_label()
{
    for(int i = 0; i < gf->label.size(); i++)
    {
        for(int j = 0; j < gf->label[i].size(); j++)
        {
            gf->label[i][j].clear();
            gf->func[i][j].clear();
        }
//        gf->label[i].clear();
//        gf->func[i].clear();
    }
    gf->label.resize(image_list.size(), vector<vector<QPointF> >(13, vector<QPointF>()));
    gf->func.resize(image_list.size(), vector<vector<Func> >(13, vector<Func>()));

    QFile file(folderpath + "res.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "warning", "failed to open res.txt");
        return;
    }

    QTextStream in(&file);
    QString line;
    for(int i = 0; i < image_list.size(); i++)
    {
        if(i == 43)
        {
            int a = 0;
        }
        line = in.readLine();
        QStringList line_list = line.split("\t");
        if(line_list[0] != image_list[i])
        {
            QMessageBox::warning(this, "warning", "images does not match when loading res.txt.");
            return;
        }
        for(int j = 0; j < LineNum; j++)
        {
            line = in.readLine();
            QStringList line_list = line.split("\t");
            if(line_list.size() > 1 && line_list[0] != "0")
            {
                for(int k = 0; k < (line_list.size()-1)/2; k++)
                {
                    QPointF tmp_point;
                    tmp_point.setX(line_list[2*k+1].toDouble());
                    tmp_point.setY(line_list[2*k+2].toDouble());
                    if(k==0 ||  k > 0 && cal_dis(tmp_point, gf->label[i][j][k-1]) > 1)
                        gf->label[i][j].push_back(tmp_point);
                }
                AffinePose(affine_mats[i],gf->label[i][j]);
                if(j != int(LeftEyebrow) && j != int(RightEyebrow))
                    cal_fun(gf->label[i][j], gf->func[i][j]);
            }
        }
    }
    in.flush();
    file.close();
}

void MainWindow::on_deleteButton_clicked()
{
    if(state == Normal || state == Begin)
    {
        labelscreen->delete_label();
        refresh();
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if(state == Normal || state == Begin)
    {
//        QMessageBox::StandardButton reply;
//        reply = QMessageBox::question(this,"Warning",tr("Save result or not?"),
//                QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok);
//        if (reply == QMessageBox::Ok)
            on_actionSave_triggered();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(state == Normal || state == Begin)
    {
        switch (e->key())
        {
        case Qt::Key_A:
            if(gf->image_no - 1 >= 0)
            {
                slider->setValue(gf->image_no);
            }
            break;
        case Qt::Key_D:
            if(gf->image_no + 1 < image_list.size())
            {
                slider->setValue(gf->image_no + 2);
            }
            break;
        case Qt::Key_W:
            if(gf->image_no - 1 >= 0)
            {
                slider->setValue(gf->image_no);
            }
            break;
        case Qt::Key_S:
            if(gf->image_no + 1 < image_list.size())
            {
                slider->setValue(gf->image_no + 2);
            }
            break;
        case Qt::Key_Escape:
            if(gf->label[gf->image_no][int(gf->aligntpye)].size() > 0)
            {
                gf->label[gf->image_no][int(gf->aligntpye)].pop_back();
                if(gf->label[gf->image_no][int(gf->aligntpye)].size() == 0)
                    labelscreen->set_linestate(Line_State::Empty);
                if(gf->aligntpye != LeftEyebrow && gf->aligntpye != RightEyebrow && gf->label[gf->image_no][int(gf->aligntpye)].size() > 2)
                {
                    cal_fun(gf->label[gf->image_no][int(gf->aligntpye)], gf->func[gf->image_no][int(gf->aligntpye)]);
                    labelscreen->set_linestate(Line_State::On);
                }
                refresh();
            }
        }
    }
}

void MainWindow::on_silderEdit_valueChanged()
{
    QString text = sliderEdit->text();
    int value = text.toInt();
    if(value > 0 && value <= image_list.size())
        on_slider_valueChanged(value);
}
