
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
    int InitKinect(bool sample);
	UserSkeleton *GetUser();
	void CleanUpAndClose();
    SkeletonState Update(bool object, bool user);
    float _objectX = 0;
    float _objectY = 0;
    float _objectZ = 0;

private:
	UserSkeleton _userSkeleton;
	ObjectTracker _objectTracker;
	openni::Device device;
	openni::VideoStream depth, color;
	openni::VideoStream** stream;
	openni::VideoFrameRef depthFrame, colorFrame;

    int counter = 0;
};


#endif
