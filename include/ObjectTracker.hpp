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
  /*  void createTrackbars()
    {
    		namedWindow(trackbarWindowName,0);

    		char TrackbarName[100];
    		printf(TrackbarName, "Hue_MIN", H_MIN);
    		printf(TrackbarName, "Hue_MAX", H_MAX);
    		printf(TrackbarName, "Saturation_MIN", S_MIN);
    		printf(TrackbarName, "Saturation_MAX", S_MAX);
    		printf(TrackbarName, "Value_MIN", V_MIN);
    		printf(TrackbarName, "Value_MAX", V_MAX);


    		createTrackbar("Hue_MIN", trackbarWindowName, &H_MIN, 256, on_trackbar);
    		createTrackbar("Hue_MAX", trackbarWindowName, &H_MAX, 256, on_trackbar);
    		createTrackbar("Saturation_MIN", trackbarWindowName, &S_MIN, 256, on_trackbar);
    		createTrackbar("Saturation_MAX", trackbarWindowName, &S_MAX, 256, on_trackbar);
    		createTrackbar("Value_MIN", trackbarWindowName, &V_MIN, 256, on_trackbar);
    		createTrackbar("Value_MAX", trackbarWindowName, &V_MAX, 256, on_trackbar);
    }*/
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
};

#endif
