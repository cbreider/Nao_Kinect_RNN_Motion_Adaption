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
#include "Connectom.hpp";

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

int Start(bool sample);
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


     Utilities::LoadPath();

     while(true)
     {
         Utilities::WriteMessage("Choose the program type and hit Enter:", Utilities::Normal);
         Utilities::WriteMessage("1: Imitation-Mode", Utilities::Normal);
         Utilities::WriteMessage("2: Sampling-Mode", Utilities::Normal);
         Utilities::WriteMessage("3: Learning-Mode", Utilities::Normal);
         Utilities::WriteMessage("4: Recproducing-Mode", Utilities::Normal);
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
               Start(false);
               break;
            case 2:
                Utilities::WriteMessage("Sampling-Mode ...", Utilities::Info);
                Start(true);
                break;
            case 3:
                Utilities::WriteMessage("Starting Learning-mode", Utilities::Info);
                LearnFromData();
                break;
            case 4:
                Utilities::WriteMessage("Starting Recproducing-Mode ...", Utilities::Info);
                StartReproducing();
                break;
         }
     }
     return 0;
}


int StartReproducing()
{
    NNType::Type type;
    int t = Utilities::ChooseNNType();
    if(t == 1) type = NNType::RNNT;
    else if(t == 2) type = NNType::RNNCL;
    else if(t == 3) type = NNType::RNNPB;

    string folder = Utilities::ChooseDir();
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

    nao.InitTrainedNN(folder, type );

    std::ifstream io;
    io.open((folder + Utilities::NNfiles.initposeFile).c_str());
    std::vector<float> initialPose(4);
    io >>  initialPose[0];
    std::cout << initialPose[0];
    io >>  initialPose[1];
    std::cout << initialPose[1];
    io >>  initialPose[2];
    std::cout << initialPose[2];
    io >>  initialPose[3];
    std::cout << initialPose[3];
    nao.SetRightArm(initialPose, motion);


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
    int count = 0;
    bool first = true;
    std::vector<float> ob(3);
    ob[0] = 0;
    ob[1] = 0;
    ob[2] = 0;
    std::vector<float> diff(3);
    diff[0] = 0;
    diff[1] = 0;
    diff[2] = 0;

    while(true)
    {
        int key = cv::waitKey(1);
        if(key==27)
        {
            break;
        }

        //Update and get the user
        userState = kinect.Update(true, false);
        diff[0] = kinect._objectX  - ob[0];
        diff[1] = kinect._objectY - ob[1];
        diff[2] = kinect._objectZ - ob[2];

        ob[0] = kinect._objectX;
        ob[1] = kinect._objectY;
        ob[2] = kinect._objectZ;
        //std::cout <<  object[0] << "   " << object[1] << "   " <<  object[2] <<endl;

        nao.Object = diff;
        if(count > 150)
        {
            if(first) Utilities::WriteMessage("Now", Utilities::Info);
             nao.Reproduce(initialPose, motion);
             first = false;
        }
        else
        {
            nao.SetRightArm(initialPose, motion);
        }
        count++;
    }
    nao.Close(motion);
    kinect.CleanUpAndClose();
}

int Start(bool sample )
{
    ip = Utilities::GetNaosIp();

    //Initialize nao
    Utilities::WriteMessage("Initializing Nao... ", Utilities::NewProcedure);
    // Setting Nao's proxies for motion and speech
    std::string text = "Nao's IP-Address: "  + ip;
    Utilities::WriteMessage(text, Utilities::NewProcedure);
    AL::ALMotionProxy motion(ip, 9559);
    AL::ALTextToSpeechProxy tts(ip, 9559);

    status = nao.Init(motion);
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
        if(!sample)
        {
            if(wasKeyboardHit()) break;
        }
        else
        {
            int key = cv::waitKey(1);
            if(key==27)
            {
                break;
            }
        }


        //Update and get the user
        userState = kinect.Update(sample, true);

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

            if(!sample)
            {
                //ckeck if confidence is high enough
                if(lConfidence > 0.5)
                {
                    //apply user arm angles to nao
                    nao.SetLeftArm(kinect.GetUser()->GetLeftArmAngles(), motion);
                }
            }
            //same for the right arm
            if(rConfidence > 0.5)
            {
                nao.SetRightArm(kinect.GetUser()->GetRightArmAngles(), motion);
            }
        }
    }
    // Close Nao and kinect and exit
    nao.Close(motion);
    kinect.CleanUpAndClose();
}

int LearnFromData()
{
    string folder = Utilities::ChooseDir();
    NNType::Type type;
    int t = Utilities::ChooseNNType();
    if(t == 1) type = NNType::RNNT;
    else if(t == 2) type = NNType::RNNCL;
    else if(t == 3) type = NNType::RNNPB;


    //status = nao.Init(motion);
    //nao.Close(motion);
    int status = nao.InitNNAndStartLearning(folder, type);
    if(status == 1)
    {
        return 1;
    }
}



int Exit()
{


}
