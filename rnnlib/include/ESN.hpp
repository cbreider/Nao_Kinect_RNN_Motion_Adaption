#ifndef ESN_HPP_INCLUDED
#define ESN_HPP_INCLUDED

/**
  * Represents an Echo State Network (ESN).
  */
class ESN : public NeuralNetwork
{
    public:
        ESN (int, ESNLayer**, int = 1);
        ~ESN ();

        void ConnectLayerToLayer (int, int);
        void ConnectLayerToLayer (int, int, Initialization*, bool);
        void ConnectLayerToLayer (int, int, mat*, bool);

        void Adapt (int, int);
        void Train (int, int);
        NetworkErrorData* Test (int, int, int);
        NetworkErrorData* Test (int, int);

    protected:
        void UpdateWeights ();

        bool** is_trainable;
};

#endif // ESN_HPP_INCLUDED
