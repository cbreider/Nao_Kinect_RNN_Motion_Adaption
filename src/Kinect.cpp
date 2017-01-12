#include "../include/Kinect.hpp"
#include "OpenNI.h"
#include <iostream>
#include "NiTE.h"
#include "Utilities.hpp"

using namespace nite;
using namespace openni;
using namespace std;

int Kinect::InitKinect(bool sample)
{
    Utilities::WriteMessage("Kinect initialization...", Utilities::NewProcedure);
    _userSkeleton.Sample = sample;
    int status = _userSkeleton.Init(device);
    if(status == 0)
    {
        openni::Status rc = OpenNI::initialize();
        status = rc;
        if (rc != openni::STATUS_OK)
        {
            Utilities::WriteError("OpenNI initialisation failed!:", OpenNI::getExtendedError());
        }
        if(status == 0)
        {
            rc = device.open(openni::ANY_DEVICE);
            if (rc != openni::STATUS_OK)
            {
                Utilities::WriteError("Kinect not found!:", OpenNI::getExtendedError());
                return 1;
            }

            //device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);
            //device.setDepthColorSyncEnabled(true);
            Utilities::WriteMessage("Kinect opened!", Utilities::OK);
            color.create(device, SENSOR_COLOR); color.start();
            Utilities::WriteMessage("Camera ok!", Utilities::OK);
            depth.create(device, SENSOR_DEPTH); depth.start();
            Utilities::WriteMessage("Camera ok!", Utilities::OK);
            Utilities::WriteMessage("Depth sensor  ok!", Utilities::OK);
            openni::VideoMode video;
            video.setResolution(640, 480); video.setFps(30);
            video.setPixelFormat(PIXEL_FORMAT_DEPTH_100_UM);
            depth.setVideoMode(video);
            video.setPixelFormat(PIXEL_FORMAT_RGB888);
            color.setVideoMode(video);
        }
    }
    Utilities::WriteBlankLine();
    if(status == 0)
	{
        _objectTracker.Init();
      Utilities::WriteMessage("Kinect successfully initialized!", Utilities::OK);
	}
    else
    {
         Utilities::WriteMessage("Failed to initialize Kinect", Utilities::Error);
    }

    Utilities::WriteBlankLine();
    Utilities::WriteBlankLine();
    return status;
}

SkeletonState Kinect::Update(bool object, bool user)
{
    if(object)
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
    }
    if(user)
    {
        return _userSkeleton.Update();
    }
    return nite::SKELETON_NONE;
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
