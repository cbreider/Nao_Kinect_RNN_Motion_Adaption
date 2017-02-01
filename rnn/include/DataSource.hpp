#ifndef DATASOURCE_HPP_INCLUDED
#define DATASOURCE_HPP_INCLUDED

#include <vector>

/**
  * Abstract base class for classes representing data sources that are used as input to a neural network. Derived classes must define the method GetSetAt().
  */
class DataSource
{
    public:
        DataSource (int, int);
        inline int GetSetSize ();
        inline int GetNumberOfSets ();
        virtual void GetSetAt (int, vec*) = 0;

    protected:
        int num_sets;
        int set_size;
};

/**
  * This class may be used as a base class for classes representing static data sources, i.e. data sources whose contents are written into this class's 'data' attribute by their respective constructors. Objects of this class will have all elements of 'data' set to zero.
  */
class StaticDataSource : public DataSource
{
    public:
        StaticDataSource (int, int);
        void GetSetAt (int, vec*);
        ~StaticDataSource ();

    protected:
        vec* data;
};

/**
  * This class grants access to the data stored in a file whose path has to be passed to the constructor.
  */
class FileDataSource : public StaticDataSource
{
    public:
        FileDataSource (int, int, const char*, int );
        FileDataSource(int , std::vector<float> );

};

#endif // DATASOURCE_HPP_INCLUDED
