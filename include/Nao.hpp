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

    int SetRightArm(std::vector<float> rArm, AL::ALMotionProxy &motion);
    int SetLeftArm(std::vector<float> lArm, AL::ALMotionProxy &motion);
    int InitNNAndStartLearning(string path, NNType::Type t);
    void Reproduce(vector<float> firstPose);
    int InitTrainedNN(string path, NNType::Type t);
    std::vector<float> Object;
    std::vector<float> rAngles;
    void Test();
    void SetRightHand(vector<float> rHandPosition, AL::ALMotionProxy &motion);
    int SayIntroductionPhrase(AL::ALTextToSpeechProxy &tts);
    int SayInstructionPhrase(AL::ALTextToSpeechProxy &tts);

    int SetHead(AL::ALMotionProxy &motion);

    int Close(AL::ALMotionProxy &motion);
    int SetMotionStiffness(AL::ALMotionProxy &motion);




    std::string IPAddress;

private:
    NNType::Type type;
    /*RNNWrapper* rnn;
    RNNCLWrapper* rnncl;
    RNNPBWrapper* rnnpb;*/
    RecurrentNeuralNetworkWrapper* naosNeuralNetwork;


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
