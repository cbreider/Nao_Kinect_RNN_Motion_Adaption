#ifndef BPTT_HPP_INCLUDED
#define BPTT_HPP_INCLUDED

/**
  * Implements the back-propagation through time algorithm. Objects of this class can be passed to RNN::RNN().
  */
class BPTT : public RNNTrainingAlgorithm
{
    public:
        BPTT (int, int, double, double);

    private:
        void ComputeGradients (RNN*, int, int);
        void UpdatePbs (RNN*, int, int, double);
        void UpdateWeights (RNN*);
};

#endif // BPTT_HPP_INCLUDED
