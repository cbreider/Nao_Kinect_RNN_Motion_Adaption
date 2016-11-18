#include "../include/Kinect.hpp"
#include "OpenNI.h"
#include <iostream>

using namespace openni;
using namespace std;

int Kinect::InitKinect()
{
    std::cout << " \n \nKinect initialization..." << endl;
	openni::Status rc = OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		//return 1;
	}
	//rc = device.open(openni::ANY_DEVICE);
	if (rc != openni::STATUS_OK)
	{
		printf("Kinect not found !\n%s\n", OpenNI::getExtendedError());
		openni::OpenNI::getExtendedError();
		std::cout << "Press ESC to exit" << endl;
		return 1;
	}

	//device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
	//device.setDepthColorSyncEnabled(true);

	std::cout << "Kinect opened" << endl;

	/*color.create(device, SENSOR_COLOR);
	color.start();
	std::cout << "Camera ok" << endl;
	depth.create(device, SENSOR_DEPTH);
	depth.start();
	std::cout << "Depth sensor ok" << endl;
	openni::VideoMode video;
	video.setResolution(640, 480);
	video.setFps(30);
	video.setPixelFormat(PIXEL_FORMAT_DEPTH_100_UM);
	depth.setVideoMode(video);
	video.setPixelFormat(PIXEL_FORMAT_RGB888);
	color.setVideoMode(video);*/

	int status = _userSkeleton.Init(device);
	if(status != 0)
	{
		return 1;
	}
	std::cout << "Kinect successfully initialized! \n \n \n " << endl;
	return 0;
}

UserSkeleton *Kinect::GetUser()
{
	return &_userSkeleton;
}

void Kinect::CleanUpAndClose()
{
	this->_userSkeleton.Close();
	//this->depth.stop();
	//this->depth.destroy();
	//this->color.destroy();
    //this->device.close();

	//openni::OpenNI::shutdown();
}
