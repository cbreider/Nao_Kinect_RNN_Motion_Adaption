#include "../include/SampleWriter.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "Utilities.hpp";

//init this object by initializing members
//note: could do that in header to for c++11
int SampleWriter::Init()
{
  _sampleNr = 0;
  _capture = false;
  _isCaputring = false;
  _lastFlag = false;
  _flag = false;
  _firstFrame = false;

  return 1;
}

 vector<string> SampleWriter::InputFiles;
 vector<string> SampleWriter::OutputFiles;

//updates capturing
int SampleWriter::Update(std::vector<float> angles, std::vector<float> object, bool captureFlag)
{
    //Set Flags
    _lastFlag = _flag;
    _flag = captureFlag;
    _capture = _flag & !_lastFlag; //do not set capture if captureFlag was also true last Time. It has to be reset first.

    //Got command to capture but there is no capturing running. So start new capturing.
    if (_capture && !_isCaputring)
    {
        startCounter = 0;
        std::cout << "Got Sample command" << '\n';

        _sampleNr++;

        OpenStreams();

        _isCaputring = true;
        _firstFrame = true;
    }
    else if (_capture && _isCaputring) // Got command to capture, but capturing is already running. So stop it.
    {
        Finalize();
    }

    //Capture frame
    if (_isCaputring)
    {
        //Time Offset of 300 frames to not start capuring imediatly, so that user could get into poition.
        startCounter++;
        if(startCounter > 150)
        {
            //write console-output
            if(startCounter == 151)
            {
                cout << "Capturing Sample" << _sampleNr << "..." << endl;
            }
            //check if object was detected correctly
            if((object[2] > 500) && (object[2] < 2000))
            {
                //capture differnce of objectPosition
                if(!_firstFrame)
                {
                    vector<float> tmp(3);

                    tmp[0] = (object[0] - _lastObjectPosition[0]);
                    tmp[1] = (object[1] - _lastObjectPosition[1]);
                    tmp[2] = (object[2] - _lastObjectPosition[2]);
                    _anglestreamIn << DataToString(angles);
                    _anglestreamOut << DataToString(angles);
                    _objectStream << DataToString(tmp);
                }
                else
                {
                    //Capture first initial pose.
                    _anglestreamIn << DataToString(angles);
                    ofstream initialPose;
                    initialPose.open(GetFilename(Utilities::NNfiles.initposeFile).c_str());
                    initialPose << DataToString(angles);
                    initialPose.close();
                }
                _lastObjectPosition = object;
            }
            if (_firstFrame)
            {
                _firstFrame = false;
            }
            //hard set of counter to not run in overflow-error of int32 (although this should not happen)
            startCounter = 2001;
        }
    }
}

//not used for the moment but manly the same as SampleWriter::Update(std::vector<float> angles, std::vector<float> object, bool captureFlag)
int SampleWriter::UpdateOnlyUser(std::vector<float> angles, bool captureFlag)
{
        _lastFlag = _flag;
        _flag = captureFlag;

        _capture = _flag & !_lastFlag;

        if (_capture && !_isCaputring)
        {
            _sampleNr++;
            stringstream ss;
            ss << "Capturing Sample";
            ss << _sampleNr;

              Utilities::WriteMessage(ss.str(), Utilities::Info);

            OpenStreams();

            _isCaputring = true;
            _firstFrame = true;
        }
        else if (_capture && _isCaputring)
        {
            Finalize();
        }

        if (_isCaputring)
        {
            if (_firstFrame)
            {
                _firstFrame = false;
            }
            _anglestreamOut << DataToString(angles);
        }
}

//close streams
int SampleWriter::Finalize()
{
    stringstream ss;
    ss << "Closing Sample ";
    ss << _sampleNr;

    Utilities::WriteMessage(ss.str(), Utilities::Error);
  _anglestreamOut.close();
  _anglestreamIn.close();
  _objectStream.close();
  _isCaputring = false;
}

//not implemented
int SampleWriter::WriteCostumFile(string output, string filename)
{
  return 1;
}

//writes the data of a vector into a whitespace-seperated string with linebrack at the end
string SampleWriter::DataToString(std::vector<float> data)
{
  int count = data.size();
  std::stringstream ss;
  for(int i = 0; i < count; i++)
  {
     ss << data[i];
     ss << " ";
  }
  ss << "\n";
  return ss.str();
}

//make a new filename with folder, sample-type and sample-number
string SampleWriter::GetFilename(string type)
{
    std::stringstream stream;
    stream << Utilities::PathToOutput;
    stream << type;
    stream << ".";
    stream << _sampleNr;

    string st = stream.str();

    if(type == "object")
        this->OutputFiles.push_back(st.c_str());
    else if(type == "angle")
        this->InputFiles.push_back((st.c_str()));


    return st;
}

void SampleWriter::OpenStreams()
{
    _anglestreamOut.open(GetFilename(Utilities::NNfiles.anglesOutFile).c_str());
    _anglestreamIn.open(GetFilename(Utilities::NNfiles.anglesInFile).c_str());
    _objectStream.open(GetFilename(Utilities::NNfiles.objectFile).c_str());
}
