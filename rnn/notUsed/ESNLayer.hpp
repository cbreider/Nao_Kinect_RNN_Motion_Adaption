#ifndef ESNLAYER_HPP_INCLUDED
#define ESNLAYER_HPP_INCLUDED

/**
  * Represents the layer of an Echo State Network (ESN).
  */
class ESNLayer : public Layer
{
    public:
        ESNLayer (int, ActivationFunction* = NULL, ESNTrainingAlgorithm* = NULL, Initialization* = NULL, mat* = NULL, Adaptation* = NULL, bool = false, double = 1.0, vec* = NULL);

        ESNTrainingAlgorithm* SetTrainingAlgorithm (ESNTrainingAlgorithm*);
        Initialization* SetInitialization (Initialization*);
        void SetDefaultWeights (const mat*);
        void GetDefaultWeights (mat*);
        double GetSpectralRadius ();
        void SetSpectralRadius (double);
        inline bool IsAdaptable ();
        inline void Adapt (vec*, vec*);
        inline mat ComputeWeightMatrix (mat*, mat*);
        inline void ComputeActivations (vec*, vec*);

    protected:
        ESNTrainingAlgorithm* train_alg;
        Initialization* init;
        Adaptation* ad;
        mat w;
};

#endif // ESNLAYER_HPP_INCLUDED
