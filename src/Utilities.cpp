#include "Utilities.hpp";
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

std::string Utilities::PATH;
std::string Utilities::PathToData;
std::string Utilities::PathToOutput;

NNFiles Utilities::NNfiles;


    void Utilities::LoadPath()
    {

        std::ifstream io;
        io.open("config/PathToData.txt");

        io >> PATH;

        PathToData = PATH + "/nn_data";
        PathToOutput = PATH + "/sample_output";

        NNfiles.weightsFile = "/weights.txt";
        NNfiles.initweightsFile = "/initialWeights.txt";
        NNfiles.initposeFile = "/initialPose.txt";
        NNfiles.anglesInFile = "/anglesIn.txt";
        NNfiles.anglesOutFile = "/anglesOut.txt";
        NNfiles.objectFile = "/objectPos.txt";
        NNfiles.paramsFile = "/nnParameters.txt";

        Utilities::WriteMessage("File-Folder was set to: " + PATH, Utilities::Normal);
    }

    string Utilities::ChooseDir()
    {
        Utilities::WriteMessage("Choose a folder to load NN from:", Utilities::Normal);

        vector<string> dirs = vector<string>();
        string dir = PathToData;
        DIR *dp;
        struct dirent *dirp;
        if((dp  = opendir(dir.c_str())) == NULL) {
            cout << "Error(" << errno << ") opening " << dir << endl;
            return "";
        }

        int i = 1;
        while ((dirp = readdir(dp)) != NULL)
        {
            string fi = string(dirp->d_name);
            if(fi.find(".") == std::string::npos)
            {

                stringstream ss;
                ss << i;
                ss << ": ";
                ss << fi;
                WriteMessage(ss.str(), Normal);
                dirs.push_back(fi);
                i++;
            }

        }
        closedir(dp);

        int x = GetIntergerInput();
        Utilities::WriteMessage("You have chosen: " + dirs[x-1], Utilities::Normal);
        Utilities::WriteBlankLine();
        return PathToData + "/" + dirs[x-1];
    }
