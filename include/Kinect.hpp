
#ifndef KINECT_H
#define KINECT_H

#include "OpenNI.h"
#include "UserSkeleton.hpp"

class Kinect
{
public:
	Kinect()
	{
	};
	int InitKinect();
	UserSkeleton *GetUser();
	void CleanUpAndClose();

private:
	UserSkeleton _userSkeleton;
	openni::Device device;
	openni::VideoStream depth, color;
	openni::VideoStream** stream;
	openni::VideoFrameRef depthFrame, colorFrame;
};


#endif
