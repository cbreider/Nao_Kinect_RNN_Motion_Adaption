#ifndef USERSKELETON_H
#define USERSKELETON_H

#include "NiTE.h"
#include "OpenNI.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "SampleWriter.hpp"

using namespace std;
using namespace nite;

#define M_PI           3.14159265358979323846
#define MAX_USERS  5
#define USER_MESSAGE(msg) \
	{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

class UserSkeleton
{

public:
	UserSkeleton()
	{
	}

	int Init(openni::Device &dev);

	std::vector<float> GetRightArmAngles();
	std::vector<float> GetLeftArmAngles();
    std::vector<float> GetArmPosition();
	float GetRightArmPositionConfidence();
	float GetLeftArmPositionConfidence();
    std::vector<float> object;
    SkeletonState Update();
    bool Sample;
	void Close();
    void Reset();

private:
     int  uid = 0;
    bool _shouldCapture;
    SampleWriter sampler;
	Status niteRc;
	UserTrackerFrameRef userTrackerFrame;
	nite::SkeletonState g_skeletonStates;
	nite::UserTracker userTracker;
	bool g_visibleUsers;

	vector<float> _userOrientationX;
	vector<float> _userOrientationY;
	vector<float> _userOrientationZ;

	SkeletonJoint torso;
    SkeletonJoint head;

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
