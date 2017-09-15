

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


inline int Layer::GetSize ()
{
    return num_units;
}

inline double Layer::GetTimeConstant ()
{
    return tau;
}


inline void Layer::SetDefaultStates (const vec* fStates)
{
    u = (*fStates);
}


inline void Layer::GetDefaultStates (vec* fCopyDest)
{
    (*fCopyDest) = u;
}


inline void Layer::SetDefaultState (int nIndex, double fValue)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nIndex >= 0 && nIndex < num_units, (char*) "ERROR: nIndex out of range in Layer::SetDefaultState!");
    #endif

    u (nIndex) = fValue;
}


inline double Layer::GetDefaultState (int nIndex)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nIndex >= 0 && nIndex < num_units, (char*) "ERROR: nIndex out of range in Layer::GetDefaultState!");
    #endif

    return u (nIndex);
}


inline bool Layer::IsPbLayer ()
{
    return is_pb;
}


inline bool Layer::IsConst ()
{
    return is_const_pb;
}


inline void Layer::ComputeActivations (vec* x, vec* y)
{
    act_func->ComputeActivations (x, y);
}

inline void Layer::ComputeDerivatives (vec* x, vec* y_d)
{
    act_func->ComputeDerivatives (x, y_d);
}
