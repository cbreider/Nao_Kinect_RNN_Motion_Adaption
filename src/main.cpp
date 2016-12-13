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

using namespace openni;
using namespace nite;
using namespace std;


int main(int argc, char* argv[])
{

	printf("Nao MotionAdaption from human demonstration, with Kinect and Recurrent Neural Network");
	printf("TH-Koeln - Cologne University of Applied Sciences \n" );
	printf("Faculty of Computer Science and Engineering Science \n");
	printf("Institute for Automation and IT \n \n");
	printf("Author: Christian Breiderhoff. \n \n \n");

	bool first = true;
	int status = 1;
	nite::SkeletonState userState;
	float rConfidence = 0.0f;
	float lConfidence = 0.0f;
	std::string ip;



	Kinect kinect;
    //Nao nao;

	//Initialize nao
	printf("Initializing Nao... \n");

	// Loading Nao's IP-Address from .txt
	//For Debug \build-nao_toolchain1\config\IPAddr.txt
	//For .exe \build-nao_toolchain1\sdk\bin\config\IPAddr.txt
	std::ifstream file;
	file.open("config/IPAddr.txt");
	std::getline(file, ip);
	file.close();

	std::cout<<"Nao's IP-Address: ";
	std::cout<<ip<< endl;
	printf("\n \n");

	// Setting Nao's proxies for motion and speech
    //AL::ALMotionProxy motion(ip, 9559);
    //AL::ALTextToSpeechProxy tts(ip, 9559);

	//status = nao.Init(motion);
	//nao.Close(motion);
    /*status = nao.Init(motion);
    status = 0;
	if(status == 1)
	{
		printf("Failed to initialize Nao \n");
		return 1;
	}
	else
	{
		printf("Nao succesfully initialized \n");
    }*/

	//Initialize kinect
	status = kinect.InitKinect();
	if(status == 1)
	{
		printf("Failed to initialize Kinect \n");
		return 1;
	}


	// Nao should say some phrase for introduction
    //nao.SayIntroductionPhrase(tts);
    //nao.SetMotionStiffness(motion);
	//run
	while(!wasKeyboardHit())
	{
		//Update and get the user
		userState = kinect.GetUser()->Update();

			// check user state
			if(userState == nite::SKELETON_NONE)
			{
				first = true;
				continue;
			}
			else if(userState == nite::SKELETON_CALIBRATING && first )
			{
				first = false;
                //nao.SayInstructionPhrase(tts);
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
                    kinect.GetUser()->GetLeftArmAngles();
                    //nao.SetLeftArm(kinect.GetUser()->GetLeftArmAngles(), motion);
				}

				//same for the right arm
				if(rConfidence > 0.5)
				{
                    kinect.GetUser()->GetRightArmAngles();
                    //nao.SetRightArm(kinect.GetUser()->GetRightArmAngles(), motion);
				}
			}

	}

	// Close Nao and kinect and exit
    //nao.Close(motion);
	kinect.CleanUpAndClose();
	return 0;
}



