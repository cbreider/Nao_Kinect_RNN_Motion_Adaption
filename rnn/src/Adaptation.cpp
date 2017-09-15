

Adaptation::Adaptation (double fMu, double fLearningRate)
{
    size = 0;
    mu = fMu;
    lr = fLearningRate;
    a = NULL;
    b = NULL;
}


Adaptation::~Adaptation ()
{
    delete [] a;
    delete [] b;
}


void Adaptation::Resize (int nSize)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSize > 0, (char*) "ERROR: nSize must be greater than 0 in Adaptation::Resize!");
    #endif

    if (size > 0)
    {
        delete [] a;
        delete [] b;
    }

    size = nSize;
    a = new double[size];
    b = new double[size];

    for (int i = 0; i < size; i++)
    {
        a[i] = 1.0;
        b[i] = 0.0;
    }
}


inline double Adaptation::GetA (int nUnit)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nUnit >= 0 && nUnit < size, (char*) "ERROR: nUnit out of range in Adaptation::GetA!");
    #endif

    return a[nUnit];
}


inline double Adaptation::GetB (int nUnit)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nUnit >= 0 && nUnit < size, (char*) "ERROR: nUnit out of range in Adaptation::GetB!");
    #endif

    return b[nUnit];
}


void Adaptation::Adapt (vec* x, vec* y)
{
    for (int i = 0; i < size; i++)
    {
    	a[i] += lr * (1.0 / a[i] + (*x) (i) - (2.0 + 1.0 / mu) * (*x) (i) * (*y) (i) + 1.0 / mu * (*x) (i) * (*y) (i) * (*y) (i));
    	b[i] += lr * (1.0 - (2.0 + 1.0 / mu) * (*y) (i) + 1.0 / mu * (*y) (i) * (*y) (i));
    }
}
