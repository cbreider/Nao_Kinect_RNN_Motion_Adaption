#ifndef Utilities_HPP
#define Utilities_HPP

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

using namespace std;

#define RESET_C   "\033[0m"
#define ERROR_C    "\033[31m"      /* Red */
#define OK_C   "\033[32m"      /* Green */
#define WARNING_C  "\033[33m"      /* Yellow */
#define NEWPROC_C    "\033[34m"      /* Blue */
#define NORMAL_C   "\033[37m"      /* White */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */

class Utilities
{
public:
    enum  OutputType
    {
      OK,
      NewProcedure,
      Warning,
      Error,
       Normal,
        Info
    };
        static void WriteMessage(string message, OutputType type)
        {
            string color = GetColor(type);
            std::cout << color << message << RESET_C << std::endl;
        }

        static void WriteError(string message, const char * err)
        {
            std::cout << ERROR_C << message  << std::endl;
            std::cout << err << endl;
            std:cout << "" << RESET_C << endl;
        }

        static void WriteBlankLine()
        {
            std::cout << ""<< std::endl;
        }

        static int GetIntergerInput()
        {
            string line;
            int mode;
            while (std::getline(std::cin, line))
            {
                std::stringstream ss(line);
                if (ss >> mode)
                {
                    if (ss.eof())
                    {   // Success
                       return mode;
                    }
                }
                return 0;
            }
        }

        static string GetNaosIp()
        {
            string ip;
            // Loading Nao's IP-Address from .txt
            //For Debug \build-nao_toolchain1\config\IPAddr.txt
            //For bin \build-nao_toolchain1\sdk\bin\config\IPAddr.txt
            std::ifstream file;
            file.open("config/IPAddr.txt");
            std::getline(file, ip);
            file.close();
            return ip;
        }

        static string GetInputFile()
        {
            vector<string> files = vector<string>();
            string dir = "output";
            DIR *dp;
            struct dirent *dirp;
            if((dp  = opendir(dir.c_str())) == NULL) {
                cout << "Error(" << errno << ") opening " << dir << endl;
                return "";
            }

            int i = 1;
            while ((dirp = readdir(dp)) != NULL)
            {
                string fi = string(dirp->d_name); if(fi.find(".txt") !=
                std::string::npos)
                {
                    stringstream ss;
                    ss << i;
                    ss << ": ";
                    ss << fi;
                    WriteMessage(ss.str(), Normal);
                    files.push_back(fi);
                    i++;
                }
            }
            closedir(dp);

            int x = GetIntergerInput();
            return "output/" + files[x-1];

        }

        static void Assert(bool cond, char* err_str = NULL)
        {
            if(!cond)
            {
                if (err_str)
                    WriteMessage(err_str, Error);

                exit (1);
            }
        }

    private:

         static string GetColor(OutputType type)
        {
            switch(type)
            {
                  case OK:
                       return OK_C;
                   case NewProcedure:
                        return NEWPROC_C;
                    case Warning:
                        return WARNING_C;
                    case Error:
                        return ERROR_C;
                    case Normal:
                        return NORMAL_C;
                    case Info:
                        return BOLDMAGENTA;
            }
        }
};


enum class OutputType
{
  OK,
  NewProcedure,
  Warning,
  Error,
   Normal
};

#endif
