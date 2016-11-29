#ifndef CTRNN_HPP_INCLUDED
#define CTRNN_HPP_INCLUDED

/**
  * Implements the continuous backpropagation through time algorithm. Objects of this class can be passed to RNN::RNN().
  */
class CBPTT : public RNNTrainingAlgorithm
{
    public:
        CBPTT (int, int, double, double);

    private:
        void ComputeGradients (RNN*, int, int);
        void UpdatePbs (RNN*, int, int, double);
        void UpdateWeights (RNN*);
};

#endif // CTRNN_HPP_INCLUDED
