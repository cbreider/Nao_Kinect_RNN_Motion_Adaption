#ifndef ESNTRAININGALGORITHM_HPP_INCLUDED
#define ESNTRAININGALGORITHM_HPP_INCLUDED

/**
  * Abstract base class from which classes representing training algorithms for Echo State Networks (ESN) may be derived. Those classes must implement the method ComputeWeightMatrix().
  */
class ESNTrainingAlgorithm
{
    public:
        virtual mat ComputeWeightMatrix (mat*, mat*) = 0;

        static PseudoInverseAlgorithm* PSEUDOINV;
        static WienerHopfAlgorithm* WIENERHOPF;
        static LinearLeastSquaresAlgorithm* SOLVE;
};

/**
  * Represents the pseudoinverse training algorithm, which is slow but stable.
  */
class PseudoInverseAlgorithm : public ESNTrainingAlgorithm
{
    public:
        mat ComputeWeightMatrix (mat*, mat*);
};

/**
  * Represents the Wiener-Hopf method of computing an Echo State Network's output weight matrix; fast but unstable in some cases.
  */
class WienerHopfAlgorithm : public ESNTrainingAlgorithm
{
    public:
        mat ComputeWeightMatrix (mat*, mat*);
};

/**
  * Represents the linear least squares method of computing an Echo State Network's weight matrix that is implemented by Armadillo's solve() function; appears to be both fast and stable.
  */
class LinearLeastSquaresAlgorithm : public ESNTrainingAlgorithm
{
    public:
        mat ComputeWeightMatrix (mat*, mat*);
};

#endif // ESNTRAININGALGORITHM_HPP_INCLUDED
