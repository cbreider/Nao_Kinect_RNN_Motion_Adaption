#ifndef USERSKELETON_H
#define USERSKELETON_H

#include "NiTE.h"
#include "OpenNI.h"
#include <stdio.h>
#include <math.h>

#include <vector>

#define M_PI           3.14159265358979323846  
#define MAX_USERS 1
#define USER_MESSAGE(msg) \
	{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

class UserSkeleton
{
public:
	UserSkeleton()
	{
	};
	nite::UserTracker userTracker;
	int Init(openni::Device &dev);
	std::vector<float> GetRightArmAngles();
	std::vector<float> GetLeftArmAngles();
	float GetRightArmPositionConfidence();
	float GetLeftArmPositionConfidence();
	nite::SkeletonState  Update();
	void Close();
	bool g_visibleUsers;
	nite::SkeletonState g_skeletonStates;

private:


	nite::Status niteRc;
	nite::UserTrackerFrameRef userTrackerFrame;

	std::vector<float> _userOrientationX;
	std::vector<float> _userOrientationY;
	std::vector<float> _userOrientationZ;
	std::vector<float> _tMatrixX;
	std::vector<float> _tMatrixY;
	std::vector<float> _tMatrixZ;
	nite::SkeletonJoint RShoulder;
	nite::SkeletonJoint RElbow;
	nite::SkeletonJoint RHand;

	nite::SkeletonJoint LShoulder;
	nite::SkeletonJoint LElbow;
	nite::SkeletonJoint LHand;

	std::vector<float> calculateArmAngles(int rightOrLeft, nite::SkeletonJoint *shoulder, nite::SkeletonJoint *elbow, nite::SkeletonJoint *hand);
	nite::SkeletonState updateUserState(const nite::UserData& user, unsigned long long ts);
	std::vector<float> getTransformedVector(nite::SkeletonJoint *first, nite::SkeletonJoint *second);
	void calculateUserOrientationAndTranforationMatrix(nite::SkeletonJoint neck, nite::SkeletonJoint rHip, nite::SkeletonJoint lHip);
};


#endif
