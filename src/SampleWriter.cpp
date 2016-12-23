#include "../include/SampleWriter.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

//init this object by initializing members
//note: could do that in header to for c++11
int SampleWriter::Init()
{
  _foldername = "output";
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

        _angleStream.open(GetFilename("angle").c_str());
        _objectStream.open(GetFilename("object").c_str());
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
        if(startCounter > 300)
        {
            //write console-output
            if(startCounter == 301)
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

                    tmp[0] = (object[0] - _lastObjectPosition[0]) / 2;
                    tmp[1] = (object[1] - _lastObjectPosition[1]) / 2;
                    tmp[2] = (object[2] - _lastObjectPosition[2]) / 2;
                    _angleStream << DataToString(angles);
                    _objectStream << DataToString(tmp);
                }
                else
                {
                    //Capture first initial pose.
                    ofstream initialPose;
                    initialPose.open(GetFilename("initialPose").c_str());
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
            cout << "Capturing Sample" << _sampleNr << "..." << endl;


            _angleStream.open(GetFilename("angle").c_str());
            _objectStream.open(GetFilename("object").c_str());
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
            _angleStream << DataToString(angles);
        }
}

//close streams
int SampleWriter::Finalize()
{
  cout << "Closing Sample " << _sampleNr << "\n" << endl;
  _angleStream.close();
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
    stream << _foldername;
    stream << "/Sample_";
    stream << type;
    stream << "_";
    stream << _sampleNr;
    stream << ".txt";

    string st = stream.str();

    if(type == "object")
        this->OutputFiles.push_back(st.c_str());
    else if(type == "angle")
        this->InputFiles.push_back((st.c_str()));


    return st;
}
