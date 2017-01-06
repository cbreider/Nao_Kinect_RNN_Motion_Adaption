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
#include "UserInterface.hpp"

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

     UserMessage::WriteMessage("____________________________________________________________________________________________", UserMessage::Info);
     UserMessage::WriteMessage("                                                                                                                                                    ", UserMessage::Info);
     UserMessage::WriteMessage("Nao MotionAdaption from human demonstration, with Kinect and Recurrent Neural Network ", UserMessage::Info);
     UserMessage::WriteMessage("                  TH-Koeln - Cologne University of Applied Sciences                                         ", UserMessage::Info );
     UserMessage::WriteMessage("               Faculty of Computer Science and Engineering Science                                       ", UserMessage::Info);
     UserMessage::WriteMessage("                                Institute for Automation and IT                                                           ", UserMessage::Info);
     UserMessage::WriteMessage("                                                                                                                                                     ", UserMessage::Info);
     UserMessage::WriteMessage("                                Author: Christian Breiderhoff.                                                            ", UserMessage::Info);
     UserMessage::WriteMessage("                                                                                                                                                      ", UserMessage::Info);
     UserMessage::WriteMessage("______________________________________________________________________________________________", UserMessage::Info);
     UserMessage::WriteBlankLine();
     UserMessage::WriteBlankLine();

     while(true)
     {
         UserMessage::WriteMessage("Choose the program type and hit Enter:", UserMessage::Normal);
         UserMessage::WriteMessage("1: Imitation-Mode", UserMessage::Normal);
         UserMessage::WriteMessage("2: Realtime Learning-Mode", UserMessage::Normal);
         UserMessage::WriteMessage("3: Sampling-Mode", UserMessage::Normal);
         UserMessage::WriteMessage("4: Learning-Mode", UserMessage::Normal);
         UserMessage::WriteMessage("5: Recproducing-Mode", UserMessage::Normal);
         UserMessage::WriteMessage("Any other Key: Exit", UserMessage::Normal);

             std::string line;
            int mode;
            while (std::getline(std::cin, line))
            {
                std::stringstream ss(line);
                if (ss >> mode)
                {
                    if (ss.eof())
                    {   // Success
                        break;
                    }
                }
                UserMessage::WriteMessage("Remember to save you output from the 'output'-directory", UserMessage::Info);
                return 0;
            }

         if(mode < 1 || mode > 5)
         {
             UserMessage::WriteMessage("Remember to save you output from the 'output'-directory", UserMessage::Info);
             return 0;
         }

         switch(mode)
         {
            case 1:
                UserMessage::WriteMessage("Starting Imitation-Mode ...", UserMessage::Info);
               Start(true, false, false);
               break;
            case 2:
                UserMessage::WriteMessage("Realtime Learning-Mode ...", UserMessage::Info);
                Start(false, false, true);
                break;
            case 3:
                UserMessage::WriteMessage("Sampling-Mode ...", UserMessage::Info);
                Start(false, true, false);
                break;
            case 4:
                UserMessage::WriteMessage("Starting Learning-mode", UserMessage::Error);
                LearnFromData();
                break;
            case 5:
                UserMessage::WriteMessage("Starting Recproducing-Mode ...", UserMessage::Info);
                StartReproducing();
                break;
         }
     }
}


int StartReproducing()
{
    GetNaosIp();

    //Initialize nao
    UserMessage::WriteMessage("Initializing Nao... ", UserMessage::NewProcedure);
    // Setting Nao's proxies for motion and speech
    std::string text = "Nao's IP-Address: "  + ip;
    UserMessage::WriteMessage(text, UserMessage::NewProcedure);
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
    GetNaosIp();

    //Initialize nao
    UserMessage::WriteMessage("Initializing Nao... ", UserMessage::NewProcedure);
    // Setting Nao's proxies for motion and speech
    std::string text = "Nao's IP-Address: "  + ip;
    UserMessage::WriteMessage(text, UserMessage::NewProcedure);
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
    UserMessage::WriteMessage("Initializing Nao... ", UserMessage::NewProcedure);
    // Setting Nao's proxies for motion and speech


    //status = nao.Init(motion);
    //nao.Close(motion);
    status = nao.InitForLearning();
    status = 0;
    if(status == 1)
    {
        return 1;
    }

    nao.Train();

}

void GetNaosIp()
{
    // Loading Nao's IP-Address from .txt
    //For Debug \build-nao_toolchain1\config\IPAddr.txt
    //For bin \build-nao_toolchain1\sdk\bin\config\IPAddr.txt
    std::ifstream file;
    file.open("config/IPAddr.txt");
    std::getline(file, ip);
    file.close();
}

int Exit()
{


}
