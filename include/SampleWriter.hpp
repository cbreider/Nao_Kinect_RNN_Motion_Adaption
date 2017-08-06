#ifndef SAMPLEWRITER_HPP
#define SAMPLEWRITER_HPP

#include <alerror/alerror.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>

//class Connectom;
using namespace std;

class SampleWriter
{
  public:
    int Init();
    int Update(std::vector<float> angles, std::vector<float> object, bool caputerFlag);
    int UpdateOnlyUser(std::vector<float> angles, bool captureFlag);
    int Finalize();
    string DataToString(std::vector<float> data);

    int WriteCostumFile(string output, string filename);

    static vector<string> InputFiles;
    static vector<string> OutputFiles;

  private:
    bool IsLeftArmOverHead();
    void OpenStreams();
    string GetFilename(string type);
    //string DataToCSVString();

    ofstream _anglestreamOut;
    ofstream _anglestreamIn;
    ofstream _objectStream;
    bool _capture;
    bool _isCaputring;
    bool _lastFlag;
    bool _flag;
    bool _firstFrame;
    int _sampleNr;
    int startCounter = 0;
    string _foldername;

    std::vector<float> _lastObjectPosition;
};

#endif
