#ifndef DETECT_TEST_H
#define DETECT_TEST_H

#include <iostream>
#include <thread>
#include "opencv2/dnn.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudawarping.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d.hpp"
#include "telnet.h"
#include <math.h>
#include <mutex>
#include <future>

class detect_test
{
public:
    detect_test(cv::dnn::Net getnet,cv::VideoCapture getcapture,bool isdetect=true,bool isGetRect=false);
    ~detect_test();

    void run() ;
    //Mat mattopixmap;
    bool autoControl;
    bool quitTheThread;
    static double command[2];//send command
    float confidenceThreshold = 0.22;//置信度
    static cv::Mat frame;
    static cv::Mat temp;
    bool _isGetRect;
    int theHolesNumber;
    std::vector<cv::Point2f> theTempCentre;
    cv::Point2f selectPoint;
    int hasClosePoint=0;
    std::mutex mtx;

    static int addIndex_1;
    static int index_1 ;

protected:
    void Detect();
    void pmacsendmassage();
    void objectPosition(std::vector<cv::Point2f> &center_p);
    void setBottleAndSift(std::vector<cv::Point2f> &center_p,cv::Mat &frame);
    double  distance_(cv::Point2f point1,cv::Point2f point2);
    void  getConfidenceCentre(std::vector< std::vector<cv::Point2f> > tens,cv::Mat &frame);
    void  caclDistanceandTheta(std::vector<cv::Point2f> theLastCentre, cv::Mat &frame,int elemet=0);
    void  luminositySift();


private:
    cv::dnn::Net net;
    cv::VideoCapture capture;
    int width;
    int height;
    bool startdetect;
    std::vector< std::vector<cv::Point2f> > tens;

    //-------------------------------全局变量的定义-------------------
    const int inwidth = 600;
    const int inheight = 600;
    const char* classNames[2] = { "holes","stone" };
    const bool get_val_mp4 = true;
};


#endif // DETECT_H
