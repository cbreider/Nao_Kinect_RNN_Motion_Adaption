#ifndef USERSKELETON_H
#define USERSKELETON_H

#include "NiTE.h"
#include "OpenNI.h"
#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;
using namespace nite;

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

	int Init(openni::Device &dev);

	std::vector<float> GetRightArmAngles();
	std::vector<float> GetLeftArmAngles();

	float GetRightArmPositionConfidence();
	float GetLeftArmPositionConfidence();

	SkeletonState Update();

	void Close();


private:

	Status niteRc;
	UserTrackerFrameRef userTrackerFrame;
	nite::SkeletonState g_skeletonStates;
	nite::UserTracker userTracker;
	bool g_visibleUsers;

	vector<float> _userOrientationX;
	vector<float> _userOrientationY;
	vector<float> _userOrientationZ;

	SkeletonJoint torso;

	SkeletonJoint rShoulder;
	SkeletonJoint rElbow;
	SkeletonJoint rHand;

	SkeletonJoint lShoulder;
	SkeletonJoint lElbow;
	SkeletonJoint lHand;

	SkeletonState updateUserState(const nite::UserData& user, unsigned long long ts);
	vector<float> calculateArmAngles(int rightOrLeft, nite::SkeletonJoint *shoulder, nite::SkeletonJoint *elbow, nite::SkeletonJoint *hand, SkeletonJoint *torso);
	void calculateUserOrientation(nite::SkeletonJoint lShoulder, nite::SkeletonJoint rShoulder, nite::SkeletonJoint torso);

	vector<float> getCrossproduct(vector<float> first, vector<float> second);
	vector<float> getVector(SkeletonJoint *first, SkeletonJoint *second);
	void normalizeVector(vector<float> &vector);
};


#endif
