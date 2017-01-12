/*__________________________________________________________________________________________________________

                                   Nao motion imitation from Kinect - Simple test Kinect

                                    TH-Koeln - Cologne University of Applied Sciences
                                    Faculty of Computer Science and Engineering Science
                                            Institute for Automation and IT

                                             Author: Christian Breiderhoff

                                            Copy Left - General Public License

                                            This Project contains Code from
                                    PrimeSense(R) OpenNI x86 v2.2, PrimeSense(R) x86 NiTE v2.2
                                            Aldebaran Naoqi C++ SDK x86 v2.1.4.13
 __________________________________________________________________________________________________________*/


#include "../include/Kinect.hpp"
#include "../include/Nao.hpp"
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include "../include/OniSampleUtilities.h"
#include "Utilities.hpp"

using namespace openni;
using namespace nite;
using namespace std;

bool first = true;
int status = 1;
nite::SkeletonState userState;
float rConfidence = 0.0f;
float lConfidence = 0.0f;
std::string ip;

Kinect kinect;
Nao nao;

int Start(bool imitate, bool sample, bool learn );
int StartReproducing();
int LearnFromData();
void GetNaosIp();
int Exit();

int main(int argc, char* argv[])
{

     Utilities::WriteMessage("____________________________________________________________________________________________", Utilities::Info);
     Utilities::WriteMessage("                                                                                                                                                    ", Utilities::Info);
     Utilities::WriteMessage("Nao MotionAdaption from human demonstration, with Kinect and Recurrent Neural Network ", Utilities::Info);
     Utilities::WriteMessage("                  TH-Koeln - Cologne University of Applied Sciences                                         ", Utilities::Info );
     Utilities::WriteMessage("               Faculty of Computer Science and Engineering Science                                       ", Utilities::Info);
     Utilities::WriteMessage("                                Institute for Automation and IT                                                           ", Utilities::Info);
     Utilities::WriteMessage("                                                                                                                                                     ", Utilities::Info);
     Utilities::WriteMessage("                                Author: Christian Breiderhoff.                                                            ", Utilities::Info);
     Utilities::WriteMessage("                                                                                                                                                      ", Utilities::Info);
     Utilities::WriteMessage("______________________________________________________________________________________________", Utilities::Info);
     Utilities::WriteBlankLine();
     Utilities::WriteBlankLine();

     while(true)
     {
         Utilities::WriteMessage("Choose the program type and hit Enter:", Utilities::Normal);
         Utilities::WriteMessage("1: Imitation-Mode", Utilities::Normal);
         Utilities::WriteMessage("2: Realtime Learning-Mode", Utilities::Normal);
         Utilities::WriteMessage("3: Sampling-Mode", Utilities::Normal);
         Utilities::WriteMessage("4: Learning-Mode", Utilities::Normal);
         Utilities::WriteMessage("5: Recproducing-Mode", Utilities::Normal);
         Utilities::WriteMessage("Any other Key: Exit", Utilities::Normal);

         int mode = Utilities::GetIntergerInput();

         if(mode < 1 || mode > 5)
         {
             Utilities::WriteMessage("Remember to save you output from the 'output'-directory", Utilities::Info);
             break;
         }

         switch(mode)
         {
            case 1:
                Utilities::WriteMessage("Starting Imitation-Mode ...", Utilities::Info);
               Start(true, false, false);
               break;
            case 2:
                Utilities::WriteMessage("Realtime Learning-Mode ...", Utilities::Info);
                Start(false, false, true);
                break;
            case 3:
                Utilities::WriteMessage("Sampling-Mode ...", Utilities::Info);
                Start(false, true, false);
                break;
            case 4:
                Utilities::WriteMessage("Starting Learning-mode", Utilities::Error);
                LearnFromData();
                break;
            case 5:
                Utilities::WriteMessage("Starting Recproducing-Mode ...", Utilities::Info);
                StartReproducing();
                break;
         }
     }
     return 0;
}


int StartReproducing()
{
    Utilities::WriteMessage("Choose a file to load NN weights input from:", Utilities::Normal);
    string w = Utilities::GetInputFile();
    ip = Utilities::GetNaosIp();

    //Initialize nao
    Utilities::WriteMessage("Initializing Nao... ", Utilities::NewProcedure);
    // Setting Nao's proxies for motion and speech
    std::string text = "Nao's IP-Address: "  + ip;
    Utilities::WriteMessage(text, Utilities::NewProcedure);
    AL::ALMotionProxy motion(ip, 9559);
    AL::ALTextToSpeechProxy tts(ip, 9559);

    //status = nao.Init(motion);
    //nao.Close(motion);
    status = nao.Init(motion);
    status = 0;
    if(status == 1)
    {
        return 1;
    }

    nao.InitNNFromFile(w);
    //Initialize kinect
    status = kinect.InitKinect(false);
    if(status == 1)
    {
        printf("Failed to initialize Kinect \n");
        return 1;
    }

    // Nao should say some phrase for introduction
    //nao.SayIntroductionPhrase(tts);
    //nao.SetMotionStiffness(motion);
    //run
    while(true)
    {
        int key = cv::waitKey(1);
        if(key==27)
        {
            break;
        }

        //Update and get the user
        userState = kinect.Update(true, false);
        std:vector<float> ob(3);
        ob[0] = kinect._objectX;
        ob[1] = kinect._objectY;
        ob[2] = kinect._objectZ;
        nao.Object = ob;
        nao.Reproduce();
    }

}

int Start(bool imitate, bool sample, bool learn )
{
    ip = Utilities::GetNaosIp();

    //Initialize nao
    Utilities::WriteMessage("Initializing Nao... ", Utilities::NewProcedure);
    // Setting Nao's proxies for motion and speech
    std::string text = "Nao's IP-Address: "  + ip;
    Utilities::WriteMessage(text, Utilities::NewProcedure);
    AL::ALMotionProxy motion(ip, 9559);
    AL::ALTextToSpeechProxy tts(ip, 9559);

    //status = nao.Init(motion);
    //nao.Close(motion);
    status = nao.Init(motion);
    status = 0;
    if(status == 1)
    {
        return 1;
    }

    //Initialize kinect
    status = kinect.InitKinect(sample);
    if(status == 1)
    {
        printf("Failed to initialize Kinect \n");
        return 1;
    }

    // Nao should say some phrase for introduction
    //nao.SayIntroductionPhrase(tts);
    //nao.SetMotionStiffness(motion);
    //run
    while(true)
    {
        int key = cv::waitKey(1);
        if(key==27)
        {
            break;
        }


        //Update and get the user
        userState = kinect.Update(sample || learn, true);

        // check user state
        if(userState == nite::SKELETON_NONE)
        {
            first = true;
            continue;
        }
        else if(userState == nite::SKELETON_CALIBRATING && first)
        {
            first = false;
            // nao.SayInstructionPhrase(tts);
        }
        else if(userState == nite::SKELETON_TRACKED)
        {
            // get the positionconfidence for arms of the user
            rConfidence = kinect.GetUser()->GetRightArmPositionConfidence();
            lConfidence = kinect.GetUser()->GetLeftArmPositionConfidence();

            //ckeck if confidence is high enough
            if(lConfidence > 0.5)
            {
                //apply user arm angles to nao
                nao.SetLeftArm(kinect.GetUser()->GetLeftArmAngles(), motion);
            }

            //same for the right arm
            if(rConfidence > 0.5)
            {
                if(learn)
                {
                    std:vector<float> ob(3);
                    ob[0] = - kinect._objectX;
                    ob[1] = - kinect._objectY;
                    ob[2] = - kinect._objectZ;
                    nao.Object = ob;
                }
                nao.SetRightArm(kinect.GetUser()->GetRightArmAngles(), motion, learn);
            }
        }
    }
    // Close Nao and kinect and exit
    nao.Close(motion);
    kinect.CleanUpAndClose();
}

int LearnFromData()
{
    Utilities::WriteMessage("Choose a file to load NN angles input from:", Utilities::Normal);
    string anglesIn = Utilities::GetInputFile();
    Utilities::WriteMessage("You have chosen: " + anglesIn, Utilities::Normal);
    Utilities::WriteBlankLine();

    Utilities::WriteMessage("Choose a file to load NN angles output from:", Utilities::Normal);
    string anglesOut = Utilities::GetInputFile();
    Utilities::WriteMessage("You have chosen: " + anglesOut, Utilities::Normal);
    Utilities::WriteBlankLine();

    Utilities::WriteMessage("Choose a file to load NN object input from:", Utilities::Normal);
    string object = Utilities::GetInputFile();
    Utilities::WriteMessage("You have chosen: " + object, Utilities::Normal);
    Utilities::WriteBlankLine();

    Utilities::WriteMessage("How many traing passes?", Utilities::Normal);
    int passes = Utilities::GetIntergerInput();
    Utilities::WriteMessage("Initializing Nao... ", Utilities::NewProcedure);
    // Setting Nao's proxies for motion and speech


    //status = nao.Init(motion);
    //nao.Close(motion);
    status = nao.InitForLearning(passes);
    status = 0;
    if(status == 1)
    {
        return 1;
    }

    nao.Train(anglesIn, object, anglesOut, passes);

}



int Exit()
{


}
