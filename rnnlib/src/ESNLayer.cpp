
/**
  * Creates a new layer for an Echo State Network.
  *
  * @param nUnits Number of units in the layer
  * @param actFunc The layer's activation function; default function if actFunc is NULL: IdentityActivation
  * @param trainingAlgorithm The training method to be used to compute the weights of the incoming connections; default algorithm if trainingAlgorithm is NULL: LinearLeastSquaresAlgorithm (will not be applied if none of the incoming connections are marked trainable)
  * @param initFunc The initialization function for the internal connection weights
  * @param weights A matrix of weights from which the connection weights will be read if initFunc is NULL; the value at (x, y) will be used for the weight of the connection from unit x to unit y (x and y are treated as zero-based indices)
  * @param adapt The adaptation function to be applied to the activation function
  * @param bIsPbESNLayer Specifies whether this is a Parametric Bias (PB) layer (true) or not (false); PB neurons always have constant internal values in ESNs
  * @param fTau The layer's time constant
  * @param fDefStates The layer's default internal states; the inputs will be added to these states in the first step before the activation function is applied; default value if fDefStates is NULL: vector of zeros
  */
ESNLayer::ESNLayer (int nUnits, ActivationFunction* actFunc, ESNTrainingAlgorithm* trainingAlgorithm, Initialization* initFunc, mat* weights, Adaptation* adapt, bool bIsPbESNLayer, double fTau, vec* fDefStates) : Layer (nUnits, actFunc, bIsPbESNLayer, true, fTau, fDefStates)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    if (adapt)
    {
        nnlib_assert (actFunc != NULL, (char*) "ERROR: Only layers with sigmoid activation function can be adapted!");
        nnlib_assert (actFunc->GetId () == ActivationFunction::ID_SIGMOID, (char*) "ERROR: Only layers with sigmoid activation function can be adapted!");
    }
    #endif

    train_alg = (trainingAlgorithm ? trainingAlgorithm : new LinearLeastSquaresAlgorithm ()); // linear least squares is the default training method
    w.zeros (num_units, num_units);
    init = initFunc;
    ad = adapt;

    if (init)
        initFunc->Initialize (&w);
    else if (weights)
        w = (*weights);

    if (ad)
        ad->Resize (num_units);
}

/**
  * Sets the training method to be used to compute the weights of the incoming connections.
  *
  * @param alg The training method to be used to compute the weights of the incoming connections
  */
ESNTrainingAlgorithm* ESNLayer::SetTrainingAlgorithm (ESNTrainingAlgorithm* alg)
{
    ESNTrainingAlgorithm* old_alg = train_alg;
    train_alg = alg;
    return old_alg;
}

/**
  * Sets the initialization function for the internal connections.
  *
  * @param initFunc The initialization function for the internal connection weights
  */
Initialization* ESNLayer::SetInitialization (Initialization* initFunc)
{
    Initialization* old_init = init;
    init = initFunc;
    init->Initialize (&w);
    return old_init;
}

/**
  * Sets the weights of the internal connections using a square matrix (entry (x, y) is the weight to be used for the connection from unit x to unit y).
  *
  * @param mWeights The matrix from which the weights will be read
  */
void ESNLayer::SetDefaultWeights (const mat* mWeights)
{
    w = (*mWeights);
}

/**
  * Retrieves the weights of the internal connections using a square matrix (entry (x, y) is the weight to be used for the connection from unit x to unit y).
  *
  * @param mWeights [out] The matrix into which the weights will be written
  */
void ESNLayer::GetDefaultWeights (mat* mWeights)
{
    (*mWeights) = w;
};

/**
  * Returns the layer's spectral radius (i.e. the largest absolute eigenvalue of the weight matrix of the internal connections).
  *
  * @return The layer's spectral radius
  */
double ESNLayer::GetSpectralRadius ()
{
    cx_colvec eigval;
    cx_mat eigvec;
    eig_gen (eigval, eigvec, w);

    colvec abs_eigval = abs (eigval);
    colvec abs_eigval_sorted = sort (abs_eigval, 1);
    return abs_eigval_sorted (0);
}

/**
  * Sets the layer's spectral radius (i.e. the largest absolute eigenvalue of the weight matrix of the internal connections).
  *
  * @param r The layer's new spectral radius
  */
void ESNLayer::SetSpectralRadius (double r)
{
    w /= GetSpectralRadius ();
    w *= r;
}

/**
  * Checks whether this layer has an adaptable activation function.
  *
  * @return A value of true indicates that the layer's activation function is adaptable
  */
inline bool ESNLayer::IsAdaptable ()
{
    return ad != NULL && act_func->GetId () == ActivationFunction::ID_SIGMOID;
}

/**
  * Adapts the layer's activation function. This method should be more generic; right now it can only be used for Intronsic Plasticity (IP) adaptation.
  *
  * @param x The induced local fields at the current time-step
  * @param y The outputs computed based on the induced local fields in x
  */
inline void ESNLayer::Adapt (vec* x, vec* y)
{
    if (ad && act_func->GetId () == ActivationFunction::ID_SIGMOID)
        ad->Adapt (x, y);
}

/**
  * Computes the new weight matrix for the layer's incoming connections based on the collected states of the layer's units and the desired outputs.
  *
  * @param s The collected states of the layer's units
  * @param d The layer's desired outputs
  */
inline mat ESNLayer::ComputeWeightMatrix (mat* s, mat* d)
{
    return train_alg->ComputeWeightMatrix (s, d);
}

/**
  * Computes the layer's outputs (they'll be stored in y) based on the induced local fields stored in x.
  *
  * @param x [in] The layer's induced local fields
  * @param y [out] The layer's outputs, computed based on the induced local fields in x
  */
inline void ESNLayer::ComputeActivations (vec* x, vec* y)
{
    if (ad)
        ((SigmoidActivation*) act_func)->ComputeActivations (x, ad, y);
    else
        act_func->ComputeActivations (x, y);
}
