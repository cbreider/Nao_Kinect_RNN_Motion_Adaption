#ifndef INITIALIZATION_HPP_INCLUDED
#define INITIALIZATION_HPP_INCLUDED

/**
  * Abstract base class for initialization methods for objects of class ESNLayer.
  */
class Initialization
{
    public:
        Initialization ();
        virtual void Initialize (mat*) = 0;
        static inline double GenerateRandomWeight (double, double);
};

/**
  * When passed as an argument to an appropriate method (e.g. RNN::ConnectLayerToLayer), objects of this class randomly creates and internal connections in an ESNLayer, assigning random weights to them.
  */
class RandomInitialization : public Initialization
{
    public:
        RandomInitialization (double, double);
        void Initialize (mat*);

    protected:
        double min, max;
};

/**
  * When passed as an argument to an appropriate method (e.g. RNN::ConnectLayerToLayer), objects of this class randomly creates sparse sparse internal connections in an ESNLayer, assigning random weights to them.
  */
class RandomSparseInitialization : public RandomInitialization
{
    public:
        RandomSparseInitialization (double, double, double);
        void Initialize (mat*);

    protected:
        double sparseness;
};

/**
  * When passed as an argument to an appropriate method (e.g. RNN::ConnectLayerToLayer), objects of this class shuffle an identity matrix which specifies the internal connection weights of an ESNLayer. Constant or random weights may be used instead of the default weight (1.0).
  */
class PermutationInitialization : public Initialization
{
    public:
        PermutationInitialization (double);
        PermutationInitialization (double, double);
        void Initialize (mat*);

    private:
        bool use_const;
        double min, max;
};

#endif // INITIALIZATION_HPP_INCLUDED
