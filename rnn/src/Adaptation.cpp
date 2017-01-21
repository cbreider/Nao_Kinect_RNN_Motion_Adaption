
/**
  * Constructor
  *
  * @param fMu The mu used in Intrinsic Plasticity Adaptation
  * @param fLearningRate The learning rate to be used during Intrinsic Plasticity adaptation
  */
Adaptation::Adaptation (double fMu, double fLearningRate)
{
    size = 0;
    mu = fMu;
    lr = fLearningRate;
    a = NULL;
    b = NULL;
}

/**
  * Destructor; deletes the arrays of A and B parameters
  */
Adaptation::~Adaptation ()
{
    delete [] a;
    delete [] b;
}

/**
  * Sets the number of units (the layer's size) whose activation functions are to be adapted; is only called by ESNLayer
  *
  * @param nSize Size of the layer whose activation function is to be adapted
  */
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

/**
  * Returns the A parameter of a unit whose activation function was adapted using Intrinsic Plasticity
  *
  * @param The unit's index in the layer
  * @return The unit's A parameter
  */
inline double Adaptation::GetA (int nUnit)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nUnit >= 0 && nUnit < size, (char*) "ERROR: nUnit out of range in Adaptation::GetA!");
    #endif

    return a[nUnit];
}

/**
  * Returns the B parameter of a unit whose activation function was adapted using Intrinsic Plasticity
  *
  * @param The unit's index in the layer
  * @return The unit's B parameter
  */
inline double Adaptation::GetB (int nUnit)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nUnit >= 0 && nUnit < size, (char*) "ERROR: nUnit out of range in Adaptation::GetB!");
    #endif

    return b[nUnit];
}

/**
  * Adapts the A and B parameters of the layer's activation function using Intrinsic Plasticity. Called after each step for which the network is run during the adaptation phase.
  *
  * @param x The layer's induced local fields at the current time-step
  * @param y The layer's activation at the current time-step
  */
void Adaptation::Adapt (vec* x, vec* y)
{
    for (int i = 0; i < size; i++)
    {
    	a[i] += lr * (1.0 / a[i] + (*x) (i) - (2.0 + 1.0 / mu) * (*x) (i) * (*y) (i) + 1.0 / mu * (*x) (i) * (*y) (i) * (*y) (i));
    	b[i] += lr * (1.0 - (2.0 + 1.0 / mu) * (*y) (i) + 1.0 / mu * (*y) (i) * (*y) (i));
    }
}
