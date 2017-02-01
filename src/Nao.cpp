#include "../include/Nao.hpp"
#include <alerror/alerror.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <stdio.h>
#include "Utilities.hpp"

int Nao::Init(AL::ALMotionProxy &motion)
{
    jointNamesRightArm = AL::ALValue::array("RShoulderPitch", "RShoulderRoll", "RElbowRoll", "RElbowYaw");
    jointNamesLeftArm = AL::ALValue::array("LShoulderPitch", "LShoulderRoll", "LElbowRoll", "LElbowYaw");
    //jointNamesLeftArm = AL::ALValue::array("LShoulderPitch");
    //jointNamesRightArm = AL::ALValue::array("RShoulderPitch");

	jointNamesHead = AL::ALValue::array("HeadPitch", "HeadYaw");
	timeSt = 1.0f;
    stiffnessForMotion = AL::ALValue::array(0.8f, 0.4f, 0.4f, 0.4f);
	stiffnessForHead = AL::ALValue::array(0.5f, 0.5f);
	fractionMaxSpeedH = 0.2f;
	fractionMaxSpeedA = 1.0f;

    int status = setMotionStiffness(motion);
    motion.openHand("RHand");
    motion.openHand("LHand");
    //naosNeuralNetwork = new Connectom();

    Utilities::WriteBlankLine();

    if(status == 1)
    {
        Utilities::WriteMessage("Failed to initialize Nao ", Utilities::Error);
    }
    else
    {
        Utilities::WriteMessage("Nao succesfully initialized ", Utilities::OK);
    }
    Utilities::WriteBlankLine();
    Utilities::WriteBlankLine();

    return status;
}

int Nao::InitNNAndStartLearning(string path, NNType::Type t)
{
    Utilities::WriteBlankLine();
    type = t;

    naosNeuralNetwork = RecurrentNeuralNetworkWrapper::GetRNN(type, path);
    naosNeuralNetwork->Init();
    Utilities::WriteMessage("Nao's Neural Network' succesfully initialized ", Utilities::OK);
    naosNeuralNetwork->TrainFromSource();
    Utilities::WriteMessage("Starting Training...", Utilities::Info);
    Utilities::WriteMessage("NN is trained.", Utilities::Info);


    Utilities::WriteBlankLine();
    Utilities::WriteBlankLine();

    return 0;
}

int Nao::InitTrainedNN(string path, NNType::Type t)
{
    Utilities::WriteBlankLine();
    type = t;

    naosNeuralNetwork = RecurrentNeuralNetworkWrapper::GetRNN(type, path);
    naosNeuralNetwork->Init();
    Utilities::WriteMessage("Nao's Neural Network' succesfully initialized ", Utilities::OK);
    naosNeuralNetwork->LoadWeights();
    Utilities::WriteMessage("Weights loaded", Utilities::Info);

    Utilities::WriteBlankLine();
    Utilities::WriteBlankLine();

    return 0;
}

void Nao::Reproduce(vector<float> firstPose, AL::ALMotionProxy &motion)
{
    rAngles = naosNeuralNetwork->PredictNextStep(Object, firstPose);
    SetRightArm(rAngles , motion);
}

int Nao::SetRightArm(std::vector<float> rArm, AL::ALMotionProxy &motion)
{
	try
	{
		//printf("%0.3f, %0.3f \n", rArm[0], rArm[1]);
		AL::ALValue targetAngles = AL::ALValue::array(rArm[0], rArm[1], rArm[2], rArm[3]);
		motion.setAngles(jointNamesRightArm, targetAngles, fractionMaxSpeedA);
		return 0;
	}
	catch(const AL::ALError& e)
	{
		return 1;
	}
}

int Nao::SetLeftArm(std::vector<float> lArm, AL::ALMotionProxy &motion)
{
	try
	{
		AL::ALValue targetAngles = AL::ALValue::array(lArm[0], lArm[1], lArm[2], lArm[3]);
        motion.setAngles(jointNamesLeftArm, targetAngles, fractionMaxSpeedA);
		return 0;
	}
	catch(const AL::ALError& e)
	{
		return 1;
	}
}

int Nao::SayIntroductionPhrase(AL::ALTextToSpeechProxy &tts)
{
	try
	{
		tts.say("Hey Buddy");
		tts.say("Stand in front of the camera");
        tts.say("And show me how you move your arms ");
        tts.say("I will imitate your motions");
        //tts.say("Then Start moving around so That I can detect you");
		return 0;
	}
	catch(const AL::ALError& e)
	{
		return 1;
	}
}

int Nao::SayInstructionPhrase(AL::ALTextToSpeechProxy &tts)
{
	try
	{
		tts.say("Show me how you move your arms ");
		tts.say("I will imitate your motions");
		return 0;
	}
	catch(const AL::ALError& e)
	{
		return 1;
	}
}

int Nao::SetMotionStiffness(AL::ALMotionProxy &motion)
{
	try
	{
		motion.stiffnessInterpolation(jointNamesRightArm, stiffnessForMotion, time);
		motion.stiffnessInterpolation(jointNamesLeftArm, stiffnessForMotion, time);
		return 0;
	}
	catch(const AL::ALError& e)
	{
		return 1;
	}
}


int Nao::setMotionStiffness(AL::ALMotionProxy &motion)
{
	try
	{
		motion.stiffnessInterpolation(jointNamesRightArm, stiffnessForMotion, timeSt);
		motion.stiffnessInterpolation(jointNamesLeftArm, stiffnessForMotion, timeSt);
		return 0;
	}
	catch(const AL::ALError& e)
	{
		return 1;
	}
}

int Nao::SetHead(AL::ALMotionProxy &motion)
{
	try
	{
		//TODO
		return 0;
	}
	catch(const AL::ALError& e)
	{
		return 1;
	}
}

int Nao::Close(AL::ALMotionProxy &motion)
{
	AL::ALValue stiffness = 0.0f;
    AL::ALValue time = 1.0f;
    motion.stiffnessInterpolation(jointNamesRightArm, stiffness, time);
	motion.stiffnessInterpolation(jointNamesLeftArm, stiffness, time);
    motion.closeHand("RHand");
    motion.closeHand("LHand");
	return 0;
}
