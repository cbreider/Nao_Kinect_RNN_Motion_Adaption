#include "../include/SampleWriter.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

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


int SampleWriter::Update(std::vector<float> angles, std::vector<float> object, bool captureFlag)
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
            _objectStream << DataToString(object);
		}
}

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

int SampleWriter::Finalize()
{
  cout << "Closing Sample " << _sampleNr << "\n" << endl;
  _angleStream.close();
  _objectStream.close();
  _isCaputring = false;
}

int SampleWriter::WriteCostumFile(string output, string filename)
{
  return 1;
}

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

string SampleWriter::GetFilename(string type)
{
    std::stringstream stream;
    stream << _foldername;
    stream << "/Sample_";
    stream << type;
    stream << "_";
    stream << _sampleNr;
    stream << ".txt";
    return stream.str();
}
