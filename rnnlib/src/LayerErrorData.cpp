
/**
  * Creates a new LayerErrorData object that will be able to hold nTimeSteps sets of data (desired output & actual output) of dimension 2 * nDataVectorDimension.
  *
  * @param nTimeSteps The number of data sets the object will be able to hold
  * @param nDataVectorDimension The dimension of one output data set; the data sets stored in this object will be twice the size, as both the desired output and the actual output will be stored
  */
LayerErrorData::LayerErrorData (int nTimeSteps, int nDataVectorDimension)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nTimeSteps > 0, (char*) "ERROR: Number of time-steps of ErrorData must be greater than 0 in LayerErrorData::LayerErrorData!");
    #endif

    count = 0;

    d.zeros (nTimeSteps, nDataVectorDimension);
    y.zeros (nTimeSteps, nDataVectorDimension);
}

/**
  * Adds a pair of desired output and actual output data.
  *
  * @param fDesiredOutput The desired output at some time-step t
  * @param fActualOutput The actual output at time-step t
  */
void LayerErrorData::AddPair (vec* fDesiredOutput, vec* fActualOutput)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert ((unsigned) count < d.n_rows, (char*) "ERROR: ErrorData object is already full in LayerErrorData::AddPair!");
    #endif

    d.row (count) = trans (*fDesiredOutput);
    y.row (count) = trans (*fActualOutput);
    count++;
}

/**
  * Sets the desired output at a particular time-step.
  *
  * @param nTimeStep The time-step
  * @param fVector The desired output at time-step nTimeStep
  */
void LayerErrorData::SetDesiredOutputAt (int nTimeStep, vec* fVector)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nTimeStep >= 0 && nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::SetDesiredOutputAt!");
    #endif

    d.row (nTimeStep) = trans (*fVector);
}

/**
  * Sets the actual output at a particular time-step.
  *
  * @param nTimeStep The time-step
  * @param fVector The actual output at time-step nTimeStep
  */
void LayerErrorData::SetOutputAt (int nTimeStep, vec* fVector)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nTimeStep >= 0 || nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::SetOutputAt!");
    #endif

    y.row (nTimeStep) = trans (*fVector);
}

/**
  * Sets the desired output and actual output at a particular time-step.
  *
  * @param nTimeStep The time-step
  * @param fDesiredOutputVector The desired output at time-step nTimeStep
  * @param fOutputVector The actual output at time-step nTimeStep
  */
void LayerErrorData::SetPairAt (int nTimeStep, vec* fDesiredOutputVector, vec* fOutputVector)
{
    SetDesiredOutputAt (nTimeStep, fDesiredOutputVector);
    SetOutputAt (nTimeStep, fOutputVector);
}

/**
  * Retrieves the desired output at a particular time-step.
  *
  * @param nTimeStep The time-step
  * @param out [out] Will contain the desired output at time-step nTimeStep
  */
void LayerErrorData::GetDesiredOutputAt (int nTimeStep, vec* out)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nTimeStep >= 0 && nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::GetDesiredOutputAt!");
    #endif

    *out = trans (d.row (nTimeStep));
}

/**
  * Retrieves the actual output at a particular time-step.
  *
  * @param nTimeStep The time-step
  * @param out [out] Will contain the actual output at time-step nTimeStep
  */
void LayerErrorData::GetOutputAt (int nTimeStep, vec* out)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nTimeStep >= 0 || nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::GetOutputAt!");
    #endif

    *out = trans (y.row (nTimeStep));
}

/**
  * Retrieves the desired output and the actual output at a particular time-step.
  *
  * @param nTimeStep The time-step
  * @param out [out] Will contain the desired output and the actual output at time-step nTimeStep
  */
void LayerErrorData::GetPairAt (int nTimeStep, vec* out)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nTimeStep >= 0 || nTimeStep < (int) d.n_rows, (char*) "ERROR: Invalid nTimeStep argument in LayerErrorData::GetPairAt!");
    #endif

    out->zeros (d.n_cols * 2);
    out->submat (0, 0, d.n_cols-1, 0) = trans (d.row (nTimeStep));
    out->submat (d.n_cols, 0, d.n_cols * 2 - 1, 0) = trans (y.row (nTimeStep));
}

/**
  * Returns the number of data sets that can be stored in this object.
  *
  * @return The number of data sets that can be stored in this object
  */
inline int LayerErrorData::GetTimeSteps ()
{
    return d.n_rows;
}

/**
  * Returns the size of a set of desired outputs (keep in mind that a set of actual outputs has the same size).
  *
  * @return The size of a set of desired/actual outputs
  */
inline int LayerErrorData::GetDimension ()
{
    return d.n_cols;
}

/**
  * Computes the mean squared error of all the pairs of desired and actual outputs of a particular unit.
  *
  * @param nUnit The zero-based index of the unit whose mean squared error is to be computed
  * @return The mean squared error of all the pairs of desired and actual outputs of a particular unit
  */
double LayerErrorData::GetMse (int nUnit)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nUnit >= 0 || nUnit < (int) d.n_cols, (char*) "ERROR: nUnit out of range in LayerErrorData::GetMse!");
    #endif

    vec diff = d.col (nUnit) - y.col (nUnit);

    return dot (diff, diff) / d.n_rows;
}

/**
  * Computes the average mean squared error of all the pairs of desired and actual outputs of all units.
  *
  * @return The average mean squared error of all the pairs of desired and actual outputs of all units
  */
double LayerErrorData::GetMse ()
{
    double e = 0.0;

    for (int unit = 0; unit < (int) d.n_cols; unit++)
        e += GetMse (unit);

    return e / d.n_cols;
}

/**
  * Computes the Kullback-Leibler divergence of all the pairs of desired and actual outputs of a particular unit.
  *
  * @param nUnit The zero-based index of the unit for which the Kullback-Leibler divergence is to be computed
  * @return The Kullback-Leibler divergence of all the pairs of desired and actual outputs of a particular unit
  */
double LayerErrorData::GetKld (int nUnit)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nUnit >= 0 || nUnit < (int) d.n_cols, (char*) "ERROR: nUnit out of range in LayerErrorData::GetKld!");
    #endif

    vec temp = d.col (nUnit);

    return dot (temp, log (temp) - log (y.col (nUnit))) / d.n_rows;
}

/**
  * Computes the average Kullback-Leibler divergence of all the pairs of desired and actual outputs of all units.
  *
  * @return The average Kullback-Leibler divergence of all the pairs of desired and actual outputs of all units
  */
double LayerErrorData::GetKld ()
{
    double e = 0.0;

    for (int unit = 0; unit < (int) d.n_cols; unit++)
        e += GetKld (unit);

    return e / d.n_cols;
}
