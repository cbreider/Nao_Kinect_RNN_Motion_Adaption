
#ifndef KINECT_H
#define KINECT_H


#include "UserSkeleton.hpp"
#include <iostream>
#include "ObjectTracker.hpp"
#include "NiTE.h"
using namespace nite;
class Kinect
{
public:
	Kinect()
	{
	};
	int InitKinect();
	UserSkeleton *GetUser();
	void CleanUpAndClose();
	SkeletonState UpdateAll();

private:
	UserSkeleton _userSkeleton;
	ObjectTracker _objectTracker;
	openni::Device device;
	openni::VideoStream depth, color;
	openni::VideoStream** stream;
	openni::VideoFrameRef depthFrame, colorFrame;
	float _objectX = 0;
	float _objectY = 0;
	float _objectZ = 0;
};


#endif
