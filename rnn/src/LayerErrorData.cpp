
LayerErrorData::LayerErrorData (int nTimeSteps, int nDataVectorDimension)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nTimeSteps > 0, (char*) "ERROR: Number of time-steps of ErrorData must be greater than 0 in LayerErrorData::LayerErrorData!");
    #endif

    count = 0;

    d.zeros (nTimeSteps, nDataVectorDimension);
    y.zeros (nTimeSteps, nDataVectorDimension);
}

void LayerErrorData::AddPair (vec* fDesiredOutput, vec* fActualOutput)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert((unsigned) count < d.n_rows, (char*) "ERROR: ErrorData object is already full in LayerErrorData::AddPair!");
    #endif

    d.row (count) = trans (*fDesiredOutput);
    y.row (count) = trans (*fActualOutput);
    count++;
}


void LayerErrorData::SetDesiredOutputAt (int nTimeStep, vec* fVector)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nTimeStep >= 0 && nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::SetDesiredOutputAt!");
    #endif

    d.row (nTimeStep) = trans (*fVector);
}


void LayerErrorData::SetOutputAt (int nTimeStep, vec* fVector)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nTimeStep >= 0 || nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::SetOutputAt!");
    #endif

    y.row (nTimeStep) = trans (*fVector);
}


void LayerErrorData::SetPairAt (int nTimeStep, vec* fDesiredOutputVector, vec* fOutputVector)
{
    SetDesiredOutputAt (nTimeStep, fDesiredOutputVector);
    SetOutputAt (nTimeStep, fOutputVector);
}


void LayerErrorData::GetDesiredOutputAt (int nTimeStep, vec* out)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nTimeStep >= 0 && nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::GetDesiredOutputAt!");
    #endif

    *out = trans (d.row (nTimeStep));
}


void LayerErrorData::GetOutputAt (int nTimeStep, vec* out)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nTimeStep >= 0 || nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::GetOutputAt!");
    #endif

    *out = trans (y.row (nTimeStep));
}


void LayerErrorData::GetPairAt (int nTimeStep, vec* out)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nTimeStep >= 0 || nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::GetPairAt!");
    #endif

    out->zeros (d.n_cols * 2);
    out->submat (0, 0, d.n_cols-1, 0) = trans (d.row (nTimeStep));
    out->submat (d.n_cols, 0, d.n_cols * 2 - 1, 0) = trans (y.row (nTimeStep));
}


inline int LayerErrorData::GetTimeSteps ()
{
    return d.n_rows;
}

inline int LayerErrorData::GetDimension ()
{
    return d.n_cols;
}


double LayerErrorData::GetMse (int nUnit)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nUnit >= 0 || nUnit < (int) d.n_cols, (char*) "ERROR: nUnit out of range in LayerErrorData::GetMse!");
    #endif

    vec diff = d.col (nUnit) - y.col (nUnit);

    return dot (diff, diff) / d.n_rows;
}


double LayerErrorData::GetMse ()
{
    double e = 0.0;

    for (int unit = 0; unit < (int) d.n_cols; unit++)
        e += GetMse (unit);

    return e / d.n_cols;
}


double LayerErrorData::GetKld (int nUnit)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nUnit >= 0 || nUnit < (int) d.n_cols, (char*) "ERROR: nUnit out of range in LayerErrorData::GetKld!");
    #endif

    vec temp = d.col (nUnit);

    return dot (temp, log (temp) - log (y.col (nUnit))) / d.n_rows;
}


double LayerErrorData::GetKld ()
{
    double e = 0.0;

    for (int unit = 0; unit < (int) d.n_cols; unit++)
        e += GetKld (unit);

    return e / d.n_cols;
}
