#ifndef USERINTERFACE_HPP
#define USERINTERFACE_HPP

#include <iostream>

using namespace std;

#define RESET_C   "\033[0m"
#define ERROR_C    "\033[31m"      /* Red */
#define OK_C   "\033[32m"      /* Green */
#define WARNING_C  "\033[33m"      /* Yellow */
#define NEWPROC_C    "\033[34m"      /* Blue */
#define NORMAL_C   "\033[37m"      /* White */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */

class UserMessage
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
