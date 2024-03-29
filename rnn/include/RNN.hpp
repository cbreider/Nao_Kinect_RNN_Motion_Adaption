#ifndef RNN2_HPP_INCLUDED
#define RNN2_HPP_INCLUDED

#include <vector>
/**
  * Represents recurrent neural networks that are trained using gradient-based methods.
  */
class RNN : public NeuralNetwork
{
    public:
        RNN (int, Layer**, RNNTrainingAlgorithm*, int = 1);
        void Train (int samplecount);
        void Train (std::vector<int> samplecount);
        NetworkErrorData* Test (int, int, int, int = 0, bool = false, bool = false, double = 0.0);
        NetworkErrorData* Test (int, int, int = 0, bool = false, bool = false, double = 0.0);
        void ConnectLayerToLayer (int, int);
        void ConnectLayerToLayer (int, int, Initialization*, bool);
        void ConnectLayerToLayer (int, int, mat*, bool);
        void ConnectLayerToLayerStatic(int nSrcLayer, int nDstLayer, RandomInitialization* initFunc, bool bTrainable);
        void ConnectUnitToLayer (int, int, int);
        void ConnectUnitToLayer (int, int, int, Initialization*, bool);
        void ConnectUnitToLayer (int, int, int, rowvec*, bool);
        void ConnectUnitToUnit (int, int, int, int, double, bool);

    protected:
        void MarkLayerToLayerConnection (int, int, bool);
        void MarkUnitToLayerConnection (int, int, int, bool);

        RNNTrainingAlgorithm* alg;
        bool**** synapses;
        bool**** trainable;

        friend class RNNTrainingAlgorithm;
        friend class BPTT;
        friend class CBPTT;
};

#endif // RNN2_HPP_INCLUDED
