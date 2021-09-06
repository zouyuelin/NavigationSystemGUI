#include "mainwindow.h"
#include "./ui_mainwindow.h"

using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),color(QColor(28,134,238))
    , ui(new Ui::MainWindow)
{
    qRegisterMetaType<Mat>("cv::Mat");
    ui->setupUi(this);

    //初始化神经网络模型和camera
    initNetCapture();

    //创建目标检测线程
    det = new detect(net,capture,true);

    //QObject::connect(det,SIGNAL(setTable(cv::Mat)),this,SLOT(getTheframe(cv::Mat)),Qt::QueuedConnection);//QueuedConnection means asynchronous
    QObject::connect(det,&detect::setTable,this,&MainWindow::getTheframe,Qt::QueuedConnection);
    QObject::connect(det,&detect::rawFrame,this,&MainWindow::cameraThread,Qt::QueuedConnection);
    det->start();

    ininTheUIsetting();

}

MainWindow::~MainWindow()
{
    //delete det;
    //delete vplot;
    det->quitTheThread = true;
    det->wait();
    det->quit();
    delete ui;
}

void MainWindow::getTheframe(Mat mat)
{
    //利用cuda加速处理cvtColor函数
    //getTheObjectPosition->frame = detect->frame.clone();
    //for(int i=0;i<2;i++)
            //pmacstart->command[i]=detect->command[i];
    cuda::GpuMat Mat1(mat);
    cuda::GpuMat gpu_frame,resize_frame;
    cuda::cvtColor(Mat1,gpu_frame,COLOR_BGR2RGB);
    cuda::resize(gpu_frame,resize_frame,cv::Size(200,200));
    resize_frame.download(mat);
    //将图像数据映射到QImage中
    QImage disImage = QImage((const unsigned char*)(mat.data),mat.cols,
                             mat.rows,QImage::Format_RGB888);
    QPixmap temp_pix = QPixmap::fromImage(disImage);

    ui->label->setPixmap(temp_pix);
    ui->label->show();
}

void MainWindow::initNetCapture()
{
    String pb_path="/home/zyl/qtWorkPath/AutoControlSystem/faster-rcnn/frozen_inference_graph.pb";
    String pbtxt_path="/home/zyl/qtWorkPath/AutoControlSystem/faster-rcnn/frozen_inference_graph.pbtxt";
    net=dnn::readNetFromTensorflow(pb_path,pbtxt_path);

    //开启GPU前向传播运算
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    if(!net.empty()){
        std::cout<<"Load the pb and pbtxt successful!\n";
    }
    else {
        std::cout<<"Load the pb and pbtxt faild!\n";
        return ;
    }

    capture=VideoCapture(0,CAP_V4L);

    if (capture.isOpened())
    {
        std::cout<<"Open the camera successful\n";
    }
    else {
        std::cout<<"Open the camera faild\n";
        return ;
    }
}

void MainWindow::vtkplotThread(Ui::MainWindow *ui,vtkRender *vplot)
{
    //set the function that plot the VTK
    vplot->vtkplot(ui->qvtkWidget);
}

void MainWindow::cameraThread(cv::Mat frame)
{
    Mat temp = frame.clone();
    cvtColor(temp,temp,COLOR_BGR2RGB);
    QImage disImage = QImage((const unsigned char*)(temp.data),temp.cols,
                                         temp.rows,QImage::Format_RGB888);
    QPixmap temp_pix = QPixmap::fromImage(disImage);

    ui->label_2->setPixmap(temp_pix);
    ui->label_2->show();
}

void MainWindow::on_pushButton_4_clicked()
{
    vtkthread = std::make_shared<std::thread>(&MainWindow::vtkplotThread,this,ui,vplot);
    vtkthread->detach();
}

void MainWindow::ininTheUIsetting()
{
    millimeter=QString(" mm");
    degree=QString(" °");
    force=QString(" N");


    ui->label->setAlignment(Qt::AlignCenter);
    //初始化VTK维
    ui->qvtkWidget->GetInteractor()->Initialize();

    //初始化plot
    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom,QString::fromLocal8Bit("时间/s"));
    ui->qwtPlot->setAxisTitle(QwtPlot::yLeft,QString::fromLocal8Bit("幅值/s"));
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft,-20.0,20.0,5.0);

    //机器人状态信息
    pimap_background = QPixmap(ui->graphicsView_5->size());
    pimap_background.fill(color);
    ui->label_14->setPixmap(pimap_background);

    //按钮美化
    ChangePushbuttonStyle();


    //lineEdit setting
    LineEditInit();

}

void MainWindow::ChangePushbuttonStyle()
{
    ui->pushButton->setStyleSheet("QPushButton{background-color:rgb(14 , 150 , 254);color: white;"
                                  "border-radius: 10px; width:100px;border: 3px groove gray;\
                                   border-style: outset;}"

                                  "QPushButton:hover{background-color:rgb(44 , 137 , 255)}"

                                  "QPushButton:pressed{background-color:rgb(14 , 135 , 228);padding-left:3px;padding-top:3px;}"

                                              );
    ui->pushButton_2->setStyleSheet("QPushButton{background-color:rgb(14 , 150 , 254);color: white;"
                              "border-radius: 10px; width:100px;border: 3px groove gray;\
                               border-style: outset;}"

                              "QPushButton:hover{background-color:rgb(44 , 137 , 255)}"

                              "QPushButton:pressed{background-color:rgb(14 , 135 , 228);padding-left:3px;padding-top:3px;}"

                                          );
    ui->pushButton_3->setStyleSheet("QPushButton{background-color:rgb(14 , 150 , 254);color: white;"
                                  "border-radius: 10px; width:100px;border: 3px groove gray;\
                                   border-style: outset;}"

                                  "QPushButton:hover{background-color:rgb(44 , 137 , 255)}"

                                  "QPushButton:pressed{background-color:rgb(14 , 135 , 228);padding-left:3px;padding-top:3px;}"

                                              );
    ui->pushButton_4->setStyleSheet("QPushButton{background-color:rgb(14 , 150 , 254);color: white;"
                                  "border-radius: 10px; width:100px;border: 3px groove gray;\
                                   border-style: outset;}"

                                  "QPushButton:hover{background-color:rgb(44 , 137 , 255)}"

                                  "QPushButton:pressed{background-color:rgb(14 , 135 , 228);padding-left:3px;padding-top:3px;}"

                                              );
    ui->pushButton_5->setStyleSheet("QPushButton{background-color:rgb(14 , 150 , 254);color: white;"
                                  "border-radius: 10px; width:100px;border: 3px groove gray;\
                                   border-style: outset;}"

                                  "QPushButton:hover{background-color:rgb(44 , 137 , 255)}"

                                  "QPushButton:pressed{background-color:rgb(14 , 135 , 228);padding-left:3px;padding-top:3px;}"

                                              );
    ui->pushButton_6->setStyleSheet("QPushButton{background-color:rgb(14 , 150 , 254);color: white;"
                                  "border-radius: 10px; width:100px;border: 3px groove gray;\
                                   border-style: outset;}"

                                  "QPushButton:hover{background-color:rgb(44 , 137 , 255)}"

                                  "QPushButton:pressed{background-color:rgb(14 , 135 , 228);padding-left:3px;padding-top:3px;}"

                                              );
    ui->pushButton_7->setStyleSheet("QPushButton{background-color:rgb(14 , 150 , 254);color: white;"
                                  "border-radius: 10px; width:100px;border: 3px groove gray;\
                                   border-style: outset;}"

                                  "QPushButton:hover{background-color:rgb(44 , 137 , 255)}"

                                  "QPushButton:pressed{background-color:rgb(14 , 135 , 228);padding-left:3px;padding-top:3px;}"

                                              );
}

void MainWindow::LineEditInit()
{
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit_2->setReadOnly(true);
    ui->lineEdit_3->setReadOnly(true);
    ui->lineEdit_4->setReadOnly(true);
    ui->lineEdit_5->setReadOnly(true);

    ui->lineEdit_6->setReadOnly(true);
    ui->lineEdit_7->setReadOnly(true);
    ui->lineEdit_8->setReadOnly(true);
    ui->lineEdit_9->setReadOnly(true);
    ui->lineEdit_10->setReadOnly(true);
    ui->lineEdit_11->setReadOnly(true);
    ui->lineEdit_6->setAlignment(Qt::AlignCenter);
    ui->lineEdit_7->setAlignment(Qt::AlignCenter);
    ui->lineEdit_8->setAlignment(Qt::AlignCenter);
    ui->lineEdit_9->setAlignment(Qt::AlignCenter);
    ui->lineEdit_10->setAlignment(Qt::AlignCenter);
    ui->lineEdit_11->setAlignment(Qt::AlignCenter);
    ui->lineEdit_6->setStyleSheet("color:red");

    ui->lineEdit_6->setText(QString("0")+millimeter);
    ui->lineEdit_7->setText(QString("0")+degree);
    ui->lineEdit_8->setText(QString("0")+degree);
    ui->lineEdit_9->setText(QString("3:1"));

    ui->lineEdit_10->setStyleSheet("color:green");
    ui->lineEdit_10->setText(QString::fromLocal8Bit("正常"));
    ui->lineEdit_11->setText(QString("0")+force);
}
