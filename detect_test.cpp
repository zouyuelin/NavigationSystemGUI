﻿#include "detect_test.h"

int detect_test::addIndex_1 = 0;
int detect_test::index_1 = 0;
double detect_test::command[2]={0};
cv::Mat detect_test::frame = cv::Mat::zeros(600,600,CV_8UC3);
cv::Mat detect_test::temp = cv::Mat::zeros(600,600,CV_8UC3);

using namespace cv;
using namespace std;

detect_test::detect_test(dnn::Net getnet,VideoCapture getcapture,bool isdetect,bool isGetRect )
    :net(getnet),capture(getcapture),autoControl(false),quitTheThread(true),theHolesNumber(0)       //子对象初始化
{
    startdetect = isdetect;
    if(isdetect)
    {
        //double rate=capture.get(CAP_PROP_FPS);
        int position=0;
        //long totalFrameNumber = capture.get(CAP_PROP_FRAME_COUNT);
        capture.set(CAP_PROP_POS_FRAMES,position);
        capture.set(CAP_PROP_FRAME_WIDTH,1920);//1920*1080
        capture.set(CAP_PROP_FRAME_HEIGHT,1080);
        width=capture.get(CAP_PROP_FRAME_WIDTH);
        height=capture.get(CAP_PROP_FRAME_HEIGHT);
    }
}

detect_test::~detect_test()
{

}
void detect_test::run()
{
    //目标检测
    if(startdetect)
    {
        Detect();
    }

}

void detect_test::Detect()
{

    // 相机内参
    Mat cameraMatrix = Mat::zeros(3,3,CV_64F);
    cameraMatrix.at<double>(0,0) = 399.23;
    cameraMatrix.at<double>(1,1) = 396.99;
    cameraMatrix.at<double>(0,2) = 301.175;
    cameraMatrix.at<double>(1,2) = 306.12;
    cameraMatrix.at<double>(2,2) = 1.00;

    Mat distCoeffs = Mat::zeros(5,1,CV_64F);
    distCoeffs.at<double>(0,0) = -0.0876;
    distCoeffs.at<double>(0,1) = -0.1972;
    distCoeffs.at<double>(0,4) = 0.1358;

        //--------------处理识别图像数据-----------------
        while(capture.isOpened() && quitTheThread)
        {
            mtx.lock();
            cv::TickMeter meter;
            meter.start();
            capture>>frame;
            if (frame.empty())
            {
                capture.release();
                break;
            }

            Rect rect(width/2-355,height/2-355,750,750);
            frame=frame(rect);
            //----------------resize 处理-------------------
                    cuda::GpuMat Mat1(frame);
                    cuda::GpuMat gpu_frame;
                    cuda::resize(Mat1,gpu_frame,Size(frame.rows*0.8,frame.cols*0.8));
                    gpu_frame.download(frame);


            vector<Point2f> center;
            objectPosition(center);
            setBottleAndSift(center,frame);//set big vector
            //imshow("image", frame);
            //waitKey(20);
            center.clear();
            meter.stop();

            //repat the image-------------------消除畸变-----------------------
            Mat output;
            undistort(frame ,output ,cameraMatrix,distCoeffs);
            frame = output.clone();
            //repat the image-------------------消除畸变结束-----------------

            putText(frame, format("FPS: %.2f ; time: %.2f ms; number: %d ", 1000.f / meter.getTimeMilli(), meter.getTimeMilli(),theHolesNumber),Point(10, 20), 0, 0.5, Scalar(0, 0, 255),1);

            temp = frame.clone();
            mtx.unlock();
            usleep(1);

        }
}

void inline detect_test::objectPosition(vector<Point2f> &center_p)
{
    Mat inputblob = dnn::blobFromImage(frame,
                                   1.,
                                   Size(inwidth, inheight),false,true);
    net.setInput(inputblob);
    Mat output = net.forward();

    Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());


//------------------循环遍历显示检测框--------------------------
    for (int i = 0; i < detectionMat.rows; i++)
        {
            float confidence = detectionMat.at<float>(i, 2);
            size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));

            if (confidence > confidenceThreshold && objectClass == 0)
            {
                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                if(xRightTop - xLeftBottom>400||yRightTop - yLeftBottom>400)
                    continue;

                //cacl the theta and length
                 Point center_C;
                 center_C.x = (xLeftBottom+xRightTop)/2.F;
                 center_C.y = (yLeftBottom+yRightTop)/2.F;

                 center_p.push_back(center_C);

            }
        }
            //show the centre of the camera
            circle(frame,Point(frame.cols/2,frame.rows/2),2,Scalar(0,50,255),2);
}

//the big vector
void inline detect_test::setBottleAndSift(vector<Point2f> &center_p,Mat &frame)
{
    if(tens.size()<10)
    {
        tens.push_back(center_p);
        return;
    }
    tens.erase(tens.begin());
    tens.push_back(center_p);

    getConfidenceCentre(tens,frame);
}

//get confident centre
void inline detect_test::getConfidenceCentre(vector< vector<Point2f> > tens,Mat &frame)
{
    vector<Point2f> theLastCentre;

    for(vector< vector<Point2f> >::iterator i=tens.begin();i !=tens.end()-5;i++)
    {
        Point2f theLastTemp;
        int count = 0;
        for(auto k = (*i).begin();k != (*i).end() ;k++)
        {
                for(vector< vector<Point2f> >::iterator j = i+1 ; j !=tens.end();j++)
                {
                    //compare the distance
                    for(auto h = (*j).begin(); (h != (*j).end()) && (*j).size() != 0;h++)
                    {
                        if( distance_(*k,*h)<20)
                        {
                            count++;
                            theLastTemp = *h;
                            h=(*j).erase(h);
                            h--;
                        }
                    }
                    if(count>3)
                    {
                        count = 0;
                        theLastCentre.push_back(theLastTemp);
                        //break;
                    }
                }
        }
    }

    //sift the single point in the vector again, which making the result looks better
    //set the threshold as 40, but you can debug the threshold if the envirnoment has chaged.
    for(auto i = theLastCentre.begin(); (i != theLastCentre.end() && theLastCentre.size() != 0);i++)
    {
        for(auto j = i+1;(j != theLastCentre.end() && theLastCentre.size() != 0);j++)
        {
            if(distance_(*j,*i)<40)
            {
                theLastCentre.erase(j);
                j--;
            }
        }
    }

    //cal theta and distance
    if(theLastCentre.size()>0)
        theTempCentre = theLastCentre;

    //sift the points whose  luminosity doesn't meet the requirement
     luminositySift();

    for(int i = 0; i<theTempCentre.size();i++)
    {
        circle(frame,theTempCentre[i],3,Scalar(0,255,255),2);
        circle(frame,theTempCentre[i],20,Scalar(0,255,0),1);
    }
}

//the distance of the points
double inline detect_test::distance_(Point2f point1,Point2f point2)
{
    return sqrt(pow(point1.x-point2.x,2)+pow(point1.y-point2.y,2));
}

//sift the points whose thread doesn't meet the requirement
void inline detect_test:: luminositySift()
{
    int averThread=0;
    int size = 7;
    for(int i = 0;i<theTempCentre.size();i++)
    {
            for(int k = -1*(size/2);k<size-size/2;k++)
                for(int h = -1*(size/2);h<size-size/2;h++)
                    averThread += frame.at<Vec3b>(theTempCentre[i].x+k,theTempCentre[i].y+h)[0];
            averThread /= pow(size,2);
            if(averThread>200)
            {
                theTempCentre.erase(i+theTempCentre.begin());
                i--;
            }
     }
}

//select the holes to get in

//cal theta and distance
void inline  detect_test::caclDistanceandTheta(vector<Point2f> theLastCentre,Mat &frame,int elemet)
{
    vector<double> theta;
    vector<double> distance;
    for(int i = 0;i<theLastCentre.size();i++)
    {
            double K =(frame.rows/2.-theLastCentre[i].y)/(frame.cols/2.-theLastCentre[i].x);
            double theta_val = 0.0;
            if(frame.rows/2.>theLastCentre[i].y&&frame.cols/2.<theLastCentre[i].x)
            theta_val = atan(-K);
            else if (frame.rows/2.>theLastCentre[i].y&&frame.cols/2.>theLastCentre[i].x) {
            theta_val = 3.14 - atan(K);
            }
            else if (frame.rows/2.<theLastCentre[i].y&&frame.cols/2.>theLastCentre[i].x) {
            theta_val =atan(-K)+3.14;
            }
            else if (frame.rows/2.<theLastCentre[i].y&&frame.cols/2.<theLastCentre[i].x) {
            theta_val = 6.28 - atan(K);
            }
            //save the theta(angle)
            theta.push_back(theta_val/6.28);
            distance.push_back(sqrt(pow((frame.cols/2-theLastCentre[i].x),2)
                  +pow((frame.rows/2-theLastCentre[i].y),2))/530.3);

            //cast the data to the powerpmac
            theta[i] = theta[i];
            distance[i] = distance[i]/1.F;
    }
    //telnet send data
    if(distance.size()!=0)
    {
        //draw the camera centre to the object centre with a green line
        if(theLastCentre.size()>0)
            line(frame,theLastCentre[elemet],Point(frame.cols/2,frame.rows/2),Scalar(255,200,0));

        //发送偏转角度，已经映射0~360°到0~1；
        command[0] = theta[elemet];
        //发送孔中心距视觉中心距离，已经映射0~530.3像素值到0~1；
        command[1] = distance[elemet];
    }
}
