#ifndef RNNTRAININGALGORITHM_HPP_INCLUDED
#define RNNTRAININGALGORITHM_HPP_INCLUDED

/**
  * Abstract base class for classes representing training algorithms for recurrent neural networks.
  */
class RNNTrainingAlgorithm
{
    public:
        RNNTrainingAlgorithm (int, int, double, double);

    protected:
        void AllocateMemory (RNN*, int);
        virtual void ComputeGradients (RNN*, int, int) = 0;
        virtual void UpdatePbs (RNN*, int, int, double) = 0;
        virtual void UpdateWeights (RNN*) = 0;
        void Train (RNN*);
        NetworkErrorData* Test (RNN*, int, int, int, int, bool, bool, double);

        int passes;
        int epoch_size;
        double lr;
        double lr_pb;
        vec*** g;
        vec* y_d;

    friend class RNN;
};

#endif // RNNTRAININGALGORITHM_HPP_INCLUDED
