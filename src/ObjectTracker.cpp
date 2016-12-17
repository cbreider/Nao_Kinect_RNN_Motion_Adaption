#include "ObjectTracker.hpp"
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

#include "OpenNI.h"

using namespace std;
using namespace cv;
using namespace openni;

void ObjectTracker::on_trackbar(int, void*)
{//This function gets called whenever a
	// trackbar position is changed



}


int ObjectTracker::Init()
{
      posX = 0;
      posY = 0;

      H_MIN = 4;
      H_MAX = 17;
      S_MIN = 163;
      S_MAX = 244;
      V_MIN = 141;
      V_MAX =256;
      trackbarWindowName = "HSV ColorPicker";
      FRAME_WIDTH = 640;
      FRAME_HEIGHT = 480;
      //createTrackbars();
      return 1;
}


vector<float> ObjectTracker::Update(openni::VideoFrameRef depthFrame, openni::VideoFrameRef colorFrame)
{
    cameraFeed.data = (uchar*)colorFrame.getData();
    cv::Mat depthImage(depthFrame.getHeight(), depthFrame.getWidth(), CV_16U);
    depthImage.data = (uchar*)depthFrame.getData();
    Mat depthImage2;
    depthImage.convertTo(depthImage2, CV_8U, 0.1);
    cvtColor(cameraFeed, cameraFeed, CV_BGR2RGB);
    cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
    inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), thresholdMat);
    cvtColor(depthImage2, coloredDepth, CV_GRAY2RGB);
    Objecttracking();


    float posZ = ((float)400 * ((float)depthImage2.at<uchar>(posY, posX)) / (float)255) - 40;

    //imshow("RGB", cameraFeed);
    cv::imshow("depth", coloredDepth);
    //cv::imshow("Threshold", thresholdMat);
    std::vector<float> pos(3);
    pos[0] = posX;
    pos[1] = posY;
    pos[2] = posZ;
    return pos;
}

void ObjectTracker::Objecttracking()
{
		Mat temp;
		cvSet(drawingIpl, cvScalar(0));
		CvMoments *colorMoment = (CvMoments*)malloc(sizeof(CvMoments));
		static const int thickness = 3;
		static const int lineType = 8;
		Scalar           color = CV_RGB(255, 255, 255);

		temp = thresholdMat.clone();
		Contours->imageData = (char*)temp.data;


		CvMemStorage*   storage = cvCreateMemStorage(0);
		CvSeq*          contours = 0;
		CvSeq*			biggestContour = 0;
		int             numCont = 0;
		int             contAthresh = 45;

		cvFindContours(Contours, storage, &contours, sizeof(CvContour),
			CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

		int largest_area = 0;

		for (; contours != 0; contours = contours->h_next)
		{
			double a = cvContourArea(contours, CV_WHOLE_SEQ);

			if (a > largest_area){

				largest_area = a;

				biggestContour = contours;
			}


		}

		if (biggestContour != 0)
		{
			cvDrawContours(drawingIpl, biggestContour, color, color, -1, thickness, lineType, cvPoint(0, 0));
		}

			cvMoments(drawingIpl, colorMoment, 1);
			if (true)
			{

		double moment10 = cvGetSpatialMoment(colorMoment, 1, 0);
		double moment01 = cvGetSpatialMoment(colorMoment, 0, 1);
		double area = cvGetCentralMoment(colorMoment, 0, 0);
		if (area > 30)
		{

			posX = (moment10 / area);
			posY = moment01 / area;
            float xd = posX -320;
            float yd = posY -240;
            float xd2, yd2;
            if(xd >= 0)
            {
                xd2 = posX + ((xd * xd) * 0.0008);
            }
            else
            {
                //xd2 = posX;
                xd2 = posX - ((xd * xd) * 0.0004);
            }
            if(yd >= 0)
            {
                yd2 = posY + ((yd * yd) * 0.0008);
            }
            else
            {
                //xd2 = posX;
                yd2 = posY - ((yd * yd) * 0.0004);
            }
            if(xd2 >= 0 && xd2<= 640 && yd2 >= 0 && yd2 <= 480)
            {
                drawObject(posX, posY, coloredDepth);
                posX = xd2;
                 posY = yd2;
            }
            else
            {
                posX = 0;
                posY = 0;
            }
            //drawObject(posX, posY, cameraFeed);
		}
		else
		{
			posX = 0;
			posY = 0;
			putText(cameraFeed, "No Object", Point(30, 450), 1, 1, Scalar(0, 255, 0), 2);
		}
	}
    else
    {
        posX = 0;
        posY = 0;
    }
}

void ObjectTracker::drawObject(int x, int y, Mat &frame)
{

	 circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	   if (y - 25>0)
		  line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	  else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	   if (y + 25<FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25<FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);


}
