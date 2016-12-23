#include "../include/Kinect.hpp"
#include "OpenNI.h"
#include <iostream>
#include "ObjectTracker.hpp"
#include "NiTE.h"
#include "UserInterface.hpp"

using namespace nite;
using namespace openni;
using namespace std;

int Kinect::InitKinect()
{
    UserMessage::WriteMessage("Kinect initialization...", UserMessage::NewProcedure);
    int status = _userSkeleton.Init(device);
    if(status == 0)
    {
        openni::Status rc = OpenNI::initialize();
        status = rc;
        if (rc != openni::STATUS_OK)
        {
            UserMessage::WriteError("OpenNI initialisation failed!:", OpenNI::getExtendedError());
        }
        if(status == 0)
        {
            rc = device.open(openni::ANY_DEVICE);
            if (rc != openni::STATUS_OK)
            {
                UserMessage::WriteError("Kinect not found!:", OpenNI::getExtendedError());
                return 1;
            }

            //device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
            //device.setDepthColorSyncEnabled(true);
            UserMessage::WriteMessage("Kinect opened!", UserMessage::OK);
            color.create(device, SENSOR_COLOR); color.start();
            UserMessage::WriteMessage("Camera ok!", UserMessage::OK);
            depth.create(device, SENSOR_DEPTH); depth.start();
            UserMessage::WriteMessage("Camera ok!", UserMessage::OK);
            UserMessage::WriteMessage("Depth sensor  ok!", UserMessage::OK);
            openni::VideoMode video;
            video.setResolution(640, 480); video.setFps(30);
            video.setPixelFormat(PIXEL_FORMAT_DEPTH_100_UM);
            depth.setVideoMode(video);
            video.setPixelFormat(PIXEL_FORMAT_RGB888);
            color.setVideoMode(video);
        }
    }
    UserMessage::WriteBlankLine();
    if(status == 0)
	{
        _objectTracker.Init();
      UserMessage::WriteMessage("Kinect successfully initialized!", UserMessage::OK);
	}
    else
    {
         UserMessage::WriteMessage("Failed to initialize Kinect", UserMessage::Error);
    }

    UserMessage::WriteBlankLine();
    UserMessage::WriteBlankLine();
    return status;
}

SkeletonState Kinect::UpdateAll()
{
  color.readFrame(&colorFrame);
  depth.readFrame(&depthFrame);

  vector<float> pos = _objectTracker.Update(depthFrame, colorFrame);

  int a = CoordinateConverter::convertDepthToWorld(depth, pos[0], pos[1], pos[2] * 10, &_objectX, &_objectY, &_objectZ);
  vector<float> pos2(3);
  pos2[0] = _objectX;
  pos2[1] = _objectY;
  pos2[2] = _objectZ;
  _userSkeleton.object = pos2;
  return _userSkeleton.Update();
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
