
/**
  * Constructor
  */
Initialization::Initialization ()
{
    srand (time (NULL));
}

/**
  * Generates a random number x, with min < x <= max.
  *
  * @param min Lower bound
  * @param max Upper bound
  */
inline double Initialization::GenerateRandomWeight (double min, double max)
{
    return min + (max - min) * rand () / RAND_MAX;
}

/**
  * Creates an initialization object that will initialize connection weights with random numbers x, with fMin < x <= fMax.
  *
  * @param fMin Lower bound
  * @param fMax Upper bound
  */
RandomInitialization::RandomInitialization (double fMin, double fMax)
{
    min = fMin;
    max = fMax;
}

/**
  * Fills a matrix with random numbers x, with fMin < x <= fMax.
  *
  * @param m [out] The matrix to be initialized
  */
void RandomInitialization::Initialize (mat* m)
{
    for (int i = 0; i < (int) m->n_rows; i++)
    	for (int j = 0; j < (int) m->n_cols; j++)
    		(*m) (i, j) = GenerateRandomWeight (min, max);
}

/**
  * Creates an initialization object that will sparsely initialize connection weights with random numbers x, with fMin < x <= fMax. The non-initialized weights will be zero.
  *
  * @param fMin Lower bound
  * @param fMax Upper bound
  * @param fSparseness The sparseness to be used (0 <= fSparseness <= 1; 0 = no weights will be set; 1 = all weights will be set; 0.5 = (roughly) 50% of all weights will be set)
  */
RandomSparseInitialization::RandomSparseInitialization (double fMin, double fMax, double fSparseness) : RandomInitialization (fMin, fMax)
{
    sparseness = fSparseness;
}

/**
  * Fills a matrix with random numbers (see RandomInitialization() for restrictions on the random numbers) and a certain amount of zeros, depending on the sparsity specified in the constructor.
  *
  * @param m [out] The matrix to be initialized
  */
void RandomSparseInitialization::Initialize (mat* m)
{
    for (int i = 0; i < (int) m->n_rows; i++)
    	for (int j = 0; j < (int) m->n_cols; j++)
    		(*m) (i, j) = ((double) rand () / RAND_MAX <= sparseness ? GenerateRandomWeight (min, max) : 0.0);
}

/**
  * Creates an initialization object that will initialize a square matrix representing connection weights (with element (x, y) containing the weight to be used for the connection from unit x to unit y) by turning it into an identity matrix, shuffling its columns and multiplying it by fWeight.
  *
  * @param fWeight The constant weight to be used
  */
PermutationInitialization::PermutationInitialization (double fWeight)
{
    use_const = true;
    min = fWeight;
}

/**
  * Creates an initialization object that will initialize a square matrix representing connection weights (with element (x, y) containing the weight to be used for the connection from unit x to unit y) by turning it into an identity matrix, shuffling its columns and multiplying its 1.0 by randomly generated numbers x, with fMin < x <= fMax.
  *
  * @param fMin Lower bound for the random numbers (fMin < x)
  * @param fMax Upper bound for the random numbers (x <= fMax)
  */
PermutationInitialization::PermutationInitialization (double fMin, double fMax)
{
    use_const = false;
    min = fMin;
    max = fMax;
}

/**
  * Initializes m by turning it into an identity matrix and shuffling its columns. Depending on which constructor was used, the 1.0 elements will be replaced by a constant weight or by different random numbers (see PermutationInitialization() for more information).
  *
  * @param m The matrix to be initialized
  */
void PermutationInitialization::Initialize (mat* m)
{
    m->zeros ();

    for (int i = 0; i < (int) (m->n_rows < m->n_cols ? m->n_rows : m->n_cols); i++)
    	(*m) (i, i) = (use_const ? min : GenerateRandomWeight (min, max));

    *m = shuffle (*m);
}
