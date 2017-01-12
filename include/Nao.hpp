#ifndef NAO_H
#define NAO_H

#include <alerror/alerror.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <stdio.h>
#include "Connectom.hpp"

using namespace std;


class Nao
{

public:

    int Init(AL::ALMotionProxy &motion);
    int InitForLearning(int passes);

    int SetRightArm(std::vector<float> rArm, AL::ALMotionProxy &motion, bool learn);
    int SetLeftArm(std::vector<float> lArm, AL::ALMotionProxy &motion);

    int SayIntroductionPhrase(AL::ALTextToSpeechProxy &tts);
    int SayInstructionPhrase(AL::ALTextToSpeechProxy &tts);

    int SetHead(AL::ALMotionProxy &motion);

    int Close(AL::ALMotionProxy &motion);
    int SetMotionStiffness(AL::ALMotionProxy &motion);

    int TrainOneStep(std::vector<float> rArm);
    int Train(string, string, string, int);
    void InitNNFromFile(string file);

    std::vector<float> Object;
    void Reproduce();
    std::string IPAddress;

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
