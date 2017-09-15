

Initialization::Initialization ()
{
    srand (time (NULL));
}


inline double Initialization::GenerateRandomWeight (double min, double max)
{
    //double f = (double)rand() / RAND_MAX;
    //return fMin + f * (fMax - fMin);
    double ra =  min + (max - min) * rand () / RAND_MAX;
    return ra;
}


RandomInitialization::RandomInitialization (double fMin, double fMax)
{
    min = fMin;
    max = fMax;
}


void RandomInitialization::Initialize (mat* m)
{
    for (int i = 0; i < (int) m->n_rows; i++)
    	for (int j = 0; j < (int) m->n_cols; j++)
    		(*m) (i, j) = GenerateRandomWeight (min, max);
}

void RandomInitialization::InitializeStatic (mat* m)
{
    for (int i = 0; i < (int) m->n_rows; i++)
        for (int j = 0; j < (int) m->n_cols; j++)
            (*m) (i, j) = 1;
}

RandomSparseInitialization::RandomSparseInitialization (double fMin, double fMax, double fSparseness) : RandomInitialization (fMin, fMax)
{
    sparseness = fSparseness;
}


void RandomSparseInitialization::Initialize (mat* m)
{
    for (int i = 0; i < (int) m->n_rows; i++)
    	for (int j = 0; j < (int) m->n_cols; j++)
    		(*m) (i, j) = ((double) rand () / RAND_MAX <= sparseness ? GenerateRandomWeight (min, max) : 0.0);
}


PermutationInitialization::PermutationInitialization (double fWeight)
{
    use_const = true;
    min = fWeight;
}

PermutationInitialization::PermutationInitialization (double fMin, double fMax)
{
    use_const = false;
    min = fMin;
    max = fMax;
}


void PermutationInitialization::Initialize (mat* m)
{
    m->zeros ();

    for (int i = 0; i < (int) (m->n_rows < m->n_cols ? m->n_rows : m->n_cols); i++)
    	(*m) (i, i) = (use_const ? min : GenerateRandomWeight (min, max));

    *m = shuffle (*m);
}
