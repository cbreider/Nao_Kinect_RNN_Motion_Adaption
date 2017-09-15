#include <vector>
#include "Utilities.hpp";
#include <math.h>


DataSource::DataSource (int nNumberOfSets, int nSetSize)
{
    num_sets = nNumberOfSets;
    set_size = nSetSize;
}


inline int DataSource::GetNumberOfSets ()
{
    return num_sets;
}

inline int DataSource::GetSetSize ()
{
    return set_size;
}


StaticDataSource::StaticDataSource (int nNumberOfSets, int nSetSize) : DataSource (nNumberOfSets, nSetSize)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nNumberOfSets > 0, (char*) "ERROR: nNumberOfSets must be greater than 0 in DataSource::DataSource!");
    Utilities::Assert(nSetSize > 0, (char*) "ERROR: nSetSize must be greater than 0 in DataSource::DataSource!");
    #endif

    data = new vec[nNumberOfSets];

    for (int i = 0; i < nNumberOfSets; i++)
        data[i].zeros (nSetSize);
}


StaticDataSource::~StaticDataSource ()
{
    //delete [] data;
}


void StaticDataSource::GetSetAt (int index, vec* y)
{
    (*y) = data[index >= 0 ? index % num_sets : num_sets + (index % num_sets)];
}

FileDataSource::FileDataSource (int nNumberOfSets, int nSetSize, const char* strFileName, int type) : StaticDataSource (nNumberOfSets, nSetSize)
{
    ifstream src (strFileName, ifstream::in);

    if (src.fail ())
    {
        cout << "ERROR: Cannot open file " << strFileName << " in FileDataSource::FileDataSource!" << endl << flush;
        Utilities::Assert(false);
    }
    vector<float> tmp(nSetSize);
    float stmp;
    //object
    if(type == 1)
    {
        for (int i = 0; i < nNumberOfSets; i++)
        {
            for (int j = 0; j < nSetSize; j++)
            {
                src >> stmp;
                tmp[j] += stmp;
                //if(i % Utilities::Parameters.timeScaling == 0)
                //{
                    data[i] (j)= stmp;
                    tmp[j] = 0;
                //}

                stmp = 0;
            }
        }
    }
    else if(type == 2)
    {
        for (int i = 0; i < nNumberOfSets; i++)
        {
            for (int j = 0; j < nSetSize; j++)
            {
                src >> stmp;
                //if(i % Utilities::Parameters.timeScaling == 0)
                //{
                    data[i] (j)=  stmp ;//roundf(stmp *1000) /1000 ;
                //}
            }
        }
    }
    else if(type == 3)
    {
        for (int i = 0; i < nNumberOfSets; i++)
        {
            for (int j = 0; j < nSetSize; j++)
            {
                src >> stmp;
                //if(i % Utilities::Parameters.timeScaling == 0)
                //{
                    data[i] (j)=  stmp ;//roundf(stmp *1000) /1000 ;
                //}
                src >> stmp;
                src >> stmp;
                src >> stmp;
            }
        }
    }
    src.close ();
}


FileDataSource::FileDataSource (int nSetSize, std::vector<float> datain ) : StaticDataSource (1, nSetSize)
{

    //nnlib_assert(nSetSize == sizeof(data)/sizeof(*data));

    // Write the contents of the file into the data matrix

        for (int j = 0; j < nSetSize; j++)
        data[1](j)= datain[j];

}
