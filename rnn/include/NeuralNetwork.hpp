#ifndef NEURALNETWORK_HPP_INCLUDED
#define NEURALNETWORK_HPP_INCLUDED

#include <string>

/**
  * Abstract base class for neural networks.
  */
class NeuralNetwork
{
    public:
        NeuralNetwork (int, Layer**, int = 1);
        ~NeuralNetwork ();
        void SetInput (int, int, DataSource*, int = -1, int = 0);
        void SetInput (int, DataSource*, int = -1, int = 0);
        void SetOutput (int, int, DataSource*, bool);
        void SetOutput (int, DataSource*, bool);
        void CopyOutputTo (int, int, bool);
        void SetDefState (int, int, int, double);
        void SetDefStates (int, int, vec*);

        void ConnectLayerToLayer (int, int);
        void ConnectLayerToLayer (int, int, Initialization*, bool);
        void ConnectLayerToLayer (int, int, mat*, bool);

        void ExportWeightsAndPBs(bool afterTraining, std::string, int);
        void ExportWeights(bool afterTraining, std::string);
        void ImportWeights(std::string);
        std::vector<float> RunOneTime (int nSeq, std::vector<float> object, int passNr, vector<float> firstangles);

    protected:
        void Run (int, int, int, bool, bool, bool);
        void RunWithRecurrentConnections (int , int , int , bool , bool , bool);
        void Reset (int, int, bool);
        void ResetDataSources ();
        void CollectErrorData (int, NetworkErrorData*);

        int num_layers;
        int num_seq;
        Layer** layers;
        mat*** w;
        DataSource*** data_in;
        DataSource*** data_out;
        int** data_in_index;
        int** data_out_index;
        bool** use_tf;
        int* copy_output;
        int* copy_output_after;

        vec* temp;
        vec** prv_u;
        vec** cur_u;
        vec** u_def;
        vec** y;
        mat** u;
        mat** s;
        mat** d;

        friend class RNNTrainingAlgorithm;
        friend class BPTT;
        friend class CTRNN;
        friend class NetworkErrorData;
};

#endif // NEURALNETWORK_HPP_INCLUDED
