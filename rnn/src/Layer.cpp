
/**
  * Creates a new layer for a recurrent neural network.
  *
  * @param nUnits Number of units in the layer
  * @param actFunc The layer's activation function; default if actFunc is NULL: IdentityActivation
  * @param bIsPbLayer Specifies whether this is a Parametric Bias layer (true) or not (false)
  * @param bIsConst If bIsPbLayer is true, this will specify whether the biases are constant (true) or trainable (false)
  * @param fTau The layer's time constant
  * @param fDefStates The layer's default internal states; the inputs will be added to this state in the first step before the activation function is applied; default if fDefStates is NULL: vector of zeros
  */
Layer::Layer (int nUnits, ActivationFunction* actFunc, bool bIsPbLayer, bool bIsConst, double fTau, vec* fDefStates)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nUnits > 0, (char*) "ERROR: nUnits must be at least 1 in Layer::Layer");
    #endif

    num_units = nUnits;
    act_func = (actFunc ? actFunc : ActivationFunction::IDENTITY);
    tau = fTau;
    u.zeros (num_units);
    is_pb = bIsPbLayer;
    is_const_pb = bIsConst;

    if (fDefStates)
        u = (*fDefStates);
    //else if (is_pb)
        //u.ones (nUnits);
}

/**
  * Returns the number of units in the layer.
  *
  * @return The number of units in the layer
  */
inline int Layer::GetSize ()
{
    return num_units;
}

/**
  * Returns the layer's time constant.
  *
  * @return The layer's time constant
  */
inline double Layer::GetTimeConstant ()
{
    return tau;
}

/**
  * Sets the layer's default states.
  *
  * @param fStates The layer's default states
  */
inline void Layer::SetDefaultStates (const vec* fStates)
{
    u = (*fStates);
}

/**
  * Retrieves the layer's default states.
  *
  * @param fCopyDest [out] Will contain the layer's default states upon return of the method
  */
inline void Layer::GetDefaultStates (vec* fCopyDest)
{
    (*fCopyDest) = u;
}

/**
  * Sets a single unit's default state.
  *
  * @param nIndex The unit's zero-based index
  * @param fValue The unit's new default value
  */
inline void Layer::SetDefaultState (int nIndex, double fValue)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nIndex >= 0 && nIndex < num_units, (char*) "ERROR: nIndex out of range in Layer::SetDefaultState!");
    #endif

    u (nIndex) = fValue;
}

/**
  * Returns a single unit's default state.
  *
  * @param nIndex The unit's zero-based index
  * @return The unit's default value
  */
inline double Layer::GetDefaultState (int nIndex)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nIndex >= 0 && nIndex < num_units, (char*) "ERROR: nIndex out of range in Layer::GetDefaultState!");
    #endif

    return u (nIndex);
}

/**
  * Checks whether this is a Parametric Bias (PB) layer.
  *
  * @return true if this is a PB layer, false otherwise
  */
inline bool Layer::IsPbLayer ()
{
    return is_pb;
}

/**
  * Checks whether this is a constant Parametric Bias (PB) layer
  *
  * @return true if this is a constant PB layer, false otherwise
  */
inline bool Layer::IsConst ()
{
    return is_const_pb;
}

/**
  * Computes the layer's outputs (they'll be stored in y) based on the induced local fields stored in x.
  *
  * @param x [in] The layer's induced local fields
  * @param y [out] The layer's outputs, computed based on the induced local fields in x
  */
inline void Layer::ComputeActivations (vec* x, vec* y)
{
    act_func->ComputeActivations (x, y);
}

/**
  * Computes the derivatives of the layer's outputs (they'll be stored in y) based on the induced local fields stored in x.
  *
  * @param x [in] The layer's induced local fields
  * @param y [out] The derivatives of the layer's outputs, computed based on the induced local fields in x
  */
inline void Layer::ComputeDerivatives (vec* x, vec* y_d)
{
    act_func->ComputeDerivatives (x, y_d);
}
