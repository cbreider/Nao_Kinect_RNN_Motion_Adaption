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
NNParams Utilities::Parameters;
std::string Utilities::NNFolder;

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
        NNfiles.paramsFile = "/Parameters.txt";
        NNfiles.PBfile = "/PBvalues";
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
        Utilities::NNFolder = PathToData + "/" + dirs[x-1];
        return PathToData + "/" + dirs[x-1];
    }

    void Utilities::LoadNNSettings()
    {
        Utilities::WriteMessage("Loading NN-Parameters...", Utilities::Info);
        std::ifstream io;
        std::string file = NNFolder + Utilities::NNfiles.paramsFile;
        io.open(file.c_str());
        std::string foo;

        io >> foo;
        io >> Parameters.hiddenUnitsCount;
        std::cout << foo << Parameters.hiddenUnitsCount << endl;

        io >> foo;
        io >> Parameters.passes;
        std::cout << foo << Parameters.passes << endl;

        io >> foo;
        io >> Parameters.epoch;
        std::cout << foo << Parameters.epoch << endl;

        io >> foo;
        io >> Parameters.lRate;
        std::cout << foo << Parameters.lRate << endl;

        io >> foo;
        io >> Parameters.minInit;
        std::cout << foo << Parameters.minInit << endl;

        io >> foo;
        io >> Parameters.maxInit;
        std::cout << foo << Parameters.maxInit << endl;

        io >> foo;
        io >> Parameters.contextUnitCount;
        std::cout << foo << Parameters.contextUnitCount << endl;

        io >> foo;
        io >> Parameters.fTau;
        std::cout << foo << Parameters.fTau << endl;

        io >> foo;
        io >> Parameters.pbLRate;
        std::cout << foo << Parameters.pbLRate << endl;

        io >> foo;
        io >> Parameters.spaceScaling;
        std::cout << foo << Parameters.spaceScaling << endl;

        io >> foo;
        io >> Parameters.timeScaling;
        std::cout << foo << Parameters.timeScaling << endl;

        Utilities::WriteBlankLine();
    }

    string Utilities::GetMoreFilenames(std::string base, int number)
    {
        stringstream ss;
        ss << base;
        ss << number;

        Utilities::WriteMessage(ss.str(), Utilities::Info);
        return ss.str();
    }
