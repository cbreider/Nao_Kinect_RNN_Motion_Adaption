#include "../include/UserSkeleton.hpp"
#include "NiTE.h"
#include "OpenNI.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "Utilities.hpp"
using namespace nite;
using namespace std;

int UserSkeleton::Init(openni::Device &dev)
{
    Utilities::WriteMessage("Initialize NiTE...", Utilities::NewProcedure);
	niteRc = NiTE::initialize();
	if (niteRc != nite::STATUS_OK)
	{
        Utilities::WriteMessage("nitializing NiTE failed!", Utilities::Error);
		return 1;
	}

    Utilities::WriteMessage("Creating UserTracker...", Utilities::NewProcedure);
    niteRc = userTracker.create();
	if (niteRc != nite::STATUS_OK)
	{
        Utilities::WriteMessage("Couldn't create user tracker!", Utilities::Error);
		return 1;
	}
    Utilities::WriteMessage("UserTracker successfully created!", Utilities::OK);

	g_visibleUsers = false;
	g_skeletonStates = nite::SKELETON_NONE;
    _shouldCapture = false;
    sampler.Init();

	return 0;
}

std::vector<float> UserSkeleton::GetRightArmAngles()
{
    std::vector<float> ang = calculateArmAngles(1, &rShoulder, &rElbow, &rHand, &torso);

    return ang;
}

std::vector<float> UserSkeleton::GetLeftArmAngles()
{
    return calculateArmAngles(-1, &lShoulder, &lElbow, &lHand, &torso);
}

float UserSkeleton::GetRightArmPositionConfidence()
{
	return min(rShoulder.getPositionConfidence(), min(rElbow.getPositionConfidence(), rHand.getPositionConfidence()));
}

float UserSkeleton::GetLeftArmPositionConfidence()
{
	return min(lShoulder.getPositionConfidence(), min(lElbow.getPositionConfidence(), lHand.getPositionConfidence()));
}

nite::SkeletonState UserSkeleton::Update()
{
	niteRc = userTracker.readFrame(&userTrackerFrame);
	if (niteRc != nite::STATUS_OK)
	{
		printf("Get next frame failed\n");
		return SKELETON_NONE;
	}

    const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();

    nite::SkeletonState state = SKELETON_NONE;
    unsigned long long ts = userTrackerFrame.getTimestamp();
    if (users.getSize() > 0)
    {
        const nite::UserData& user = users[0];
        for (int i = 0; i < users.getSize(); ++i)
        {
            const nite::UserData& user = users[i];
            if(users[i].getId() ==1)
            {



                state = updateUserState(user,userTrackerFrame.getTimestamp());

                if (user.isNew())
                {
                    userTracker.startSkeletonTracking(user.getId());
                }
                else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
                {
                    // NiTE2.2 x64 under Linux seems to have a bug. Right and left joints are interchanged. So here left = right and vise versa
                    this->calculateUserOrientation(user.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER), user.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER), user.getSkeleton().getJoint(nite::JOINT_TORSO));

                    lShoulder = user.getSkeleton().getJoint(nite::JOINT_RIGHT_SHOULDER);
                    lElbow = user.getSkeleton().getJoint(nite::JOINT_RIGHT_ELBOW);
                    lHand = user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
                    //printf("%0.3f, %0.3f \n", lHand.getPosition().x, lHand.getPosition().y);
                    rShoulder = user.getSkeleton().getJoint(nite::JOINT_LEFT_SHOULDER);
                    rElbow = user.getSkeleton().getJoint(nite::JOINT_LEFT_ELBOW);
                    rHand = user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);

                    head = user.getSkeleton().getJoint(nite::JOINT_HEAD);
                    torso = user.getSkeleton().getJoint(nite::JOINT_TORSO);
                }
            }
        }
    }
	return state;
}

std::vector<float> UserSkeleton::calculateArmAngles(int rightOrLeft, SkeletonJoint *shoulder, SkeletonJoint *elbow, SkeletonJoint *hand, SkeletonJoint *torso)
{
	vector<float> upperArm = getVector(elbow, shoulder);
	vector<float> lowerArm = getVector(hand, elbow);

    float shoulderPitch =  -asin(upperArm[0] * _userOrientationY[0] + upperArm[1] * _userOrientationY[1] + upperArm[2] *  _userOrientationY[2]);
    float shoulderRoll =  asin(upperArm[0] *  _userOrientationX[0] + upperArm[1] *  _userOrientationX[1] + upperArm[2] *  _userOrientationX[2]);
	float elbowRoll = rightOrLeft * acos(upperArm[0] * lowerArm[0] + upperArm[1] * lowerArm[1] + upperArm[2] * lowerArm[2]);

	vector<float> shoulderTorso = getVector(shoulder, torso);
	vector<float> crossTorsoArm = getCrossproduct(shoulderTorso, upperArm);
	vector<float> crossArms = getCrossproduct(upperArm, lowerArm);
	float elbowYaw = rightOrLeft * (acos(crossTorsoArm[0] * crossArms[0] + crossTorsoArm[1] * crossArms[1] + crossTorsoArm[2] * crossArms[2]) );

    if(rightOrLeft == 1)
	{
		if(elbowRoll > 1.55) elbowRoll = 1.5446;
		if(elbowRoll < 0.03) elbowRoll = 0.0349;
        if(elbowYaw > 2.0857) elbowYaw = 2.0857;
        if(elbowYaw < -2.0857) elbowYaw = -2.0857;
        if(shoulderRoll < -1.3265) shoulderRoll = -1.3265;
        if(shoulderRoll > 0.3142) shoulderRoll = 0.3142;
	}

	if(rightOrLeft == -1)
	{
		if(elbowRoll < -1.55) elbowRoll = -1.5446;
		if(elbowRoll > -0.03) elbowRoll = -0.0349;
        if(elbowYaw > 2.0857) elbowYaw = 2.0857;
        if(elbowYaw < -2.0857) elbowYaw = -2.0857;
        if(shoulderRoll > 1.3265) shoulderRoll = 1.3265;
        if(shoulderRoll < -0.3142) shoulderRoll = -0.3142;
    }

	std::vector<float> angles(4);
	angles[0] = shoulderPitch;
	angles[1] = shoulderRoll;
    angles[2] = elbowRoll;
	angles[3] = elbowYaw;

    if(rightOrLeft == 1 && Sample)
    {
       if( hand->getPositionConfidence() > 0.4 && elbow->getPositionConfidence() > 0.4 && shoulder->getPositionConfidence() > 0.4)  sampler.Update(angles, object, lHand.getPosition().y > head.getPosition().y + 200);
    }

	return angles;
}

vector<float> UserSkeleton::getVector(SkeletonJoint *first, SkeletonJoint *second)
{
	vector<float> vector(3);
	vector[0] = first->getPosition().x - second->getPosition().x;
	vector[1] = first->getPosition().y - second->getPosition().y;
	vector[2] = first->getPosition().z - second->getPosition().z;

	normalizeVector(vector);

	return vector;
}

vector<float> UserSkeleton::getCrossproduct(vector<float> first, vector<float> second)
{
	if(first.size() != 3 && second.size() != 3)
	{
		return vector<float>(3);
	}

	vector<float> crossProduct(3);
	crossProduct[0] = first[1] * second[2] - first[2] * second[1];
	crossProduct[1] = first[2] * second[0] - first[0] * second[2];
	crossProduct[0] = first[0] * second[1] - first[1] * second[0];

	normalizeVector(crossProduct);

	return crossProduct;
}

void UserSkeleton::normalizeVector(vector<float> &vector)
{
	if(vector.size() != 3) return;

	float lenght = sqrt(pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2));
	vector[0] = vector[0] / lenght;
	vector[1] = vector[1] / lenght;
	vector[2] = vector[2] / lenght;
}

vector<float> UserSkeleton::GetArmPosition()
{
    vector<float> arm(3);
    arm[0] = (torso.getPosition().x - rHand.getPosition().x) / 1000;
    arm[1] = (torso.getPosition().y - rHand.getPosition().y) / 1000;
    arm[2] = (torso.getPosition().z - rHand.getPosition().z) / 1000;
    return arm;
}

void UserSkeleton::calculateUserOrientation(nite::SkeletonJoint lShoulder, nite::SkeletonJoint rShoulder, nite::SkeletonJoint torso)
{
	//To get a orientation system relative to the user some basic vector geometry is done.

	//step 1: calculate line between the two shoulders this will be our x-axis
	std::vector<float> xAxis(3);
	xAxis[0] = lShoulder.getPosition().x - rShoulder.getPosition().x;
	xAxis[1] = lShoulder.getPosition().y - rShoulder.getPosition().y;
	xAxis[2] = lShoulder.getPosition().z - rShoulder.getPosition().z;

	std::vector<float> linePoint(3);
	linePoint[0] =  lShoulder.getPosition().x;
	linePoint[1] =  lShoulder.getPosition().y;
	linePoint[2] =  lShoulder.getPosition().z;

	//step 2: define plane
	std::vector<float> vTorso(3);
	vTorso[0] = torso.getPosition().x;
	vTorso[1] = torso.getPosition().y;
	vTorso[2] = torso.getPosition().z;

	float lambda = (xAxis[0] * (vTorso[0] - linePoint[0]) + xAxis[1] * (vTorso[1] - linePoint[1]) + xAxis[2] * (vTorso[2] - linePoint[2])) / (pow(xAxis[0], 2) + pow(xAxis[1], 2) + pow(xAxis[2], 2));

	//step 3: get intersection point
	std::vector<float> intersectionPoint(3);
	intersectionPoint[0] = linePoint[0] + lambda * xAxis[0];
	intersectionPoint[1] = linePoint[1] + lambda * xAxis[1];
	intersectionPoint[2] = linePoint[2] + lambda * xAxis[2];

	//step 4: define yAxis as the vector between intersection point and torso
	std::vector<float> yAxis(3);
	yAxis[0] = intersectionPoint[0] - vTorso[0];
	yAxis[1] = intersectionPoint[1] - vTorso[1];
	yAxis[2] = intersectionPoint[2] - vTorso[2];

	//step 5: zAxis is the vector orthogonal to x- and yAxis
	std::vector<float> zAxis(3);
	zAxis[0] = intersectionPoint[1] * yAxis[2] - intersectionPoint[2] * yAxis[1];
	zAxis[1] = intersectionPoint[2] * yAxis[0] - intersectionPoint[0] * yAxis[2];
	zAxis[0] = intersectionPoint[0] * yAxis[1] - intersectionPoint[1] * yAxis[0];

	//step 6: normalize axisis
	float xLenght = sqrt(pow(xAxis[0], 2) + pow(xAxis[1], 2) + pow(xAxis[2], 2));
	float yLenght = sqrt(pow(yAxis[0], 2) + pow(yAxis[1], 2) + pow(yAxis[2], 2));
	float zLenght = sqrt(pow(zAxis[0], 2) + pow(zAxis[1], 2) + pow(zAxis[2], 2));

	xAxis[0] = xAxis[0] / xLenght;
	xAxis[1] = xAxis[1] / xLenght;
	xAxis[2] = xAxis[2] / xLenght;

	yAxis[0] = yAxis[0] / yLenght;
	yAxis[1] = yAxis[1] / yLenght;
	yAxis[2] = yAxis[2] / yLenght;

	zAxis[0] = zAxis[0] / zLenght;
	zAxis[1] = zAxis[1] / zLenght;
	zAxis[2] = zAxis[2] / zLenght;


	this->_userOrientationX = xAxis;
	this->_userOrientationY = yAxis;
	this->_userOrientationZ = zAxis;
}

nite::SkeletonState UserSkeleton::updateUserState(const nite::UserData& user, unsigned long long ts)
{
	if (user.isNew())
		USER_MESSAGE("New")
	else if (user.isVisible() && !g_visibleUsers)
		USER_MESSAGE("Visible")
	else if (!user.isVisible() && g_visibleUsers)
		USER_MESSAGE("Out of Scene")
	else if (user.isLost())
		USER_MESSAGE("Lost")

		g_visibleUsers = user.isVisible();

	if (g_skeletonStates != user.getSkeleton().getState())
	{
		switch (g_skeletonStates = user.getSkeleton().getState())
		{
			case nite::SKELETON_NONE:
				USER_MESSAGE("Stopped tracking.")
					return SKELETON_NONE;
					break;
			case nite::SKELETON_CALIBRATING:
				USER_MESSAGE("Calibrating...")
					return SKELETON_CALIBRATING;
					break;
			case nite::SKELETON_TRACKED:
				USER_MESSAGE("Tracking!")
					return nite::SKELETON_TRACKED;
					break;
			case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
			case nite::SKELETON_CALIBRATION_ERROR_HANDS:
			case nite::SKELETON_CALIBRATION_ERROR_LEGS:
			case nite::SKELETON_CALIBRATION_ERROR_HEAD:
			case nite::SKELETON_CALIBRATION_ERROR_TORSO:
				USER_MESSAGE("Calibration Failed... :-|")
					return nite::SKELETON_NONE;
					break;
		}
	}
	return g_skeletonStates;
}

void UserSkeleton::Close()
{
	//this->userTracker.destroy();
	NiTE::shutdown();
}
