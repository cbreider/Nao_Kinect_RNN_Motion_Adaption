#ifndef OBJECTTRACKER_HPP
#define OBJECTTRACKER_HPP

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include "OpenNI2/OpenNI.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>



//class Connectom;
using namespace std;
using namespace cv;

class ObjectTracker
{

  public:
    vector<float> Update(openni::VideoFrameRef depthFrame, openni::VideoFrameRef colorFrame);
    int Init();
    u_int16_t depth;

  private:
    void Objecttracking();

        void drawObject(int x, int y, Mat &frame);
    void on_trackbar(int, void*);

    float posX;
    float posY;
    Mat cameraFeed{640, 480, CV_8UC3, NULL};
   Mat HSV{640, 480, CV_8UC3, NULL};
   Mat thresholdMat{640, 480, CV_8UC1, NULL};
   cv::Mat depthcv1{640, 480, CV_16UC1, NULL};
   IplImage*  Contours = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
   IplImage*  drawingIpl = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    Mat coloredDepth;
     int H_MIN;
     int H_MAX;
     int S_MIN;
     int S_MAX;
     int V_MIN;
     int V_MAX;
     string trackbarWindowName;
     int FRAME_WIDTH;
     int FRAME_HEIGHT;
    float x, y, z;
    Mat show{640, 480, CV_8UC3, NULL};

};

#endif
