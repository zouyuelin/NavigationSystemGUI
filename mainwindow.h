#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <auto_ptr.h>
#include <mutex>
#include "telnet.h"
#include "detect.h"
#include "vtkrender.h"
#include <QString>
#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class vtkRender;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow(); 
    void initNetCapture();
    void vtkplotThread(Ui::MainWindow *ui, vtkRender *vplot);
    void ininTheUIsetting();
    void ChangePushbuttonStyle();
    void LineEditInit();

private slots:
    void on_pushButton_4_clicked();


    void getTheframe(cv::Mat mat);
    void cameraThread(cv::Mat frame);

private:
    Ui::MainWindow *ui;
    detect *det;
    cv::dnn::Net net;
    cv::VideoCapture capture;
    vtkRender *vplot;
    std::shared_ptr<std::thread> vtkthread;
    QGraphicsScene*  Scene;
    QPixmap pimap_background;
    QColor color;


    QString millimeter;
    QString degree;
    QString force;

};
#endif // MAINWINDOW_H
