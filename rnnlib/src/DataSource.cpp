
/**
  * Constructor taking the number of sets to be stored in this object and the size of those sets as arguments.
  *
  * @param nNumberOfSets The number of sets to be stored in this object
  * @param nSetSize The size of the sets to be stored in this object
  */
DataSource::DataSource (int nNumberOfSets, int nSetSize)
{
    num_sets = nNumberOfSets;
    set_size = nSetSize;
}

/**
  * Returns the number of sets stored in this object.
  *
  * @return The number of sets
  */
inline int DataSource::GetNumberOfSets ()
{
    return num_sets;
}

/**
  * Returns the size of the sets stored in this object.
  *
  * @return The size of the sets
  */
inline int DataSource::GetSetSize ()
{
    return set_size;
}

/**
  * Constructor taking the number of sets to be stored in this object and the size of those sets as arguments. Allocates memory to store the data in and initializes it with zero.
  *
  * @param nNumberOfSets The number of sets to be stored in this object
  * @param nSetSize The size of the sets to be stored in this object
  */
StaticDataSource::StaticDataSource (int nNumberOfSets, int nSetSize) : DataSource (nNumberOfSets, nSetSize)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nNumberOfSets > 0, (char*) "ERROR: nNumberOfSets must be greater than 0 in DataSource::DataSource!");
    nnlib_assert (nSetSize > 0, (char*) "ERROR: nSetSize must be greater than 0 in DataSource::DataSource!");
    #endif

    data = new vec[nNumberOfSets];

    for (int i = 0; i < nNumberOfSets; i++)
        data[i].zeros (nSetSize);
}

/**
  * Destructor; deletes the memory allocated in the constructor.
  */
StaticDataSource::~StaticDataSource ()
{
    delete [] data;
}

/**
  * Copies the data of set 'index' to 'y'
  *
  * @param index The set's index
  * @param y [out] Will contain the data stored in the set adressed by 'index'
  */
void StaticDataSource::GetSetAt (int index, vec* y)
{
    (*y) = data[index >= 0 ? index % num_sets : num_sets + (index % num_sets)];
}

/**
  * Constructor taking the number of sets to be stored in this object, the size of those sets, and the path of the file from which to read the data as arguments. Allocates memory and fills it with the data stored in the file. The file must contain at least nNumberOfSets*nSetSize whitespace-separated double values and no other characters.
  *
  * @param nNumberOfSets The number of sets to be stored in this object
  * @param nSetSize The size of the sets to be stored in this object
  * @param strFileName The path of the file from which nNumberOfSets sets of size nSetSize will be read and stored in the object
  */
FileDataSource::FileDataSource (int nNumberOfSets, int nSetSize, char* strFileName) : StaticDataSource (nNumberOfSets, nSetSize)
{
    ifstream src (strFileName, ifstream::in);

    if (src.fail ())
    {
        cout << "ERROR: Cannot open file " << strFileName << " in FileDataSource::FileDataSource!" << endl << flush;
        nnlib_assert (false);
    }

    // Write the contents of the file into the data matrix
    for (int i = 0; i < nNumberOfSets; i++)
        for (int j = 0; j < nSetSize; j++)
        	src >> data[i] (j);

    src.close ();
}
