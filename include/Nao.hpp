#ifndef NAO_H
#define NAO_H

#include <alerror/alerror.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <stdio.h>
#include "../include/Connectom.hpp"

//class Connectom;

class Nao
{

public:

	int Init(AL::ALMotionProxy &motion);

	int SetRightArm(std::vector<float> rArm, AL::ALMotionProxy motion);
	int SetLeftArm(std::vector<float> lArm, AL::ALMotionProxy motion);

	int SayIntroductionPhrase(AL::ALTextToSpeechProxy &tts);
	int SayInstructionPhrase(AL::ALTextToSpeechProxy &tts);

	int SetHead(AL::ALMotionProxy &motion);

	int Close(AL::ALMotionProxy &motion);
	int SetMotionStiffness(AL::ALMotionProxy &motion);

    int Train();

private:
    Connectom naosNeuralNetwork;
	float fractionMaxSpeedH;
	float fractionMaxSpeedA;

	AL::ALValue jointNamesRightArm;
	AL::ALValue jointNamesLeftArm;
	AL::ALValue jointNamesHead;

	AL::ALValue timeSt;
	AL::ALValue stiffnessForMotion;
	AL::ALValue stiffnessForHead;

	int setMotionStiffness(AL::ALMotionProxy &motion);
};

#endif
