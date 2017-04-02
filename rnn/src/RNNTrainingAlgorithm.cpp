#include <vector>
#include "Utilities.hpp"

/**
  * Constructs a learning algorithm object, with the parameters determining the number of training epochs, the size of one training epoch, the learning rate, and the Paraetric Bias (PB) learning rate.
  *
  * @param nPasses The number of times for which the network will be run for nEpochSize steps; the weights will be updated after each pass
  * @param nEpochSize The size of one training epoch (a certain number of steps after which the weights will be updated)
  * @param fLearningRate The learning rate to be used for updating the network weights
  * @param fPbLearningRate The learning rate to be used for updating the PB values
  */
RNNTrainingAlgorithm::RNNTrainingAlgorithm (int nPasses, int nEpochSize, double fLearningRate, double fPbLearningRate)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nPasses > 0, (char*) "ERROR: nPasses must be greater than 0 in RNNTrainingAlgorithm::RNNTrainingAlgorithm!");
    Utilities::Assert(nEpochSize > 1, (char*) "ERROR: nEpochSize must be greater than 1 in RNNTrainingAlgorithm::RNNTrainingAlgorithm!");
    #endif

    passes = nPasses;
    epoch_size = nEpochSize;
    lr = fLearningRate;
    lr_pb = fPbLearningRate;
}

/**
  * Allocates memory based on the structure of the recurrent neural network to be trained, and the chosen epoch size.
  *
  * @param net The recurrent neural network to be trained
  * @param nEpochSize The epoch size to be used during training
  */
void RNNTrainingAlgorithm::AllocateMemory (RNN* net, int nEpochSize)
{
    g = new vec**[net->num_seq];
    for (int seq = 0; seq < net->num_seq; seq++)
    {
        g[seq] = new vec*[net->num_layers];

        for (int l = 0; l < net->num_layers; l++)
        {
            g[seq][l] = new vec[nEpochSize];

            for (int t = 0; t < nEpochSize; t++)
            	g[seq][l][t].zeros (net->layers[l]->GetSize ());
        }
    }

    y_d = new vec[net->num_layers];
    for (int l = 0; l < net->num_layers; l++)
        y_d[l].zeros (net->layers[l]->GetSize ());
}

/**
  * Trains a recurrent neural network.
  *
  * @param net The recurrent neural network to be trained
  */
void RNNTrainingAlgorithm::Train (RNN* net, int samplecount)
{
    Train(net, std::vector<int>{samplecount});
}

void RNNTrainingAlgorithm::Train (RNN* net, std::vector<int> samplecounter)
{
    net->ResetDataSources ();
    AllocateMemory (net, epoch_size);

    for (int p = 0; p < passes; p++)
    {
        if(Utilities::wasKeyboardHit())
        {
            return;
        }
        else
        {
            for (int seq = 0; seq < net->num_seq; seq++)
            {
                net->RunWithRecurrentConnections (seq, epoch_size, 0, (p == 0), (p == 0), ((p-1)% samplecounter[seq] == 0)); // collect states, reset network only before first pass
                ComputeGradients (net, seq, epoch_size);
                UpdatePbs (net, seq, epoch_size, lr_pb);
            }
            if(p%100 == 0)
            {
                std::cout << "PassNr.: " << p << endl;
                for(int i = 0; i< net->num_seq; i++)
                {
                    for(int ii = 0; ii < 4; ii++)
                    {
                        for(int iii = 0; iii< net->num_layers; iii++)
                        {
                            if(net->layers[iii]->IsPbLayer())
                                std::cout << "PB" << i <<": "<< net->u_def[i][iii] (ii) << endl;
                        }
                    }
                        std::cout << sum_e[i] << endl;
                        sum_e[i] = 0;
                }
                std::cout << "\n \n";
            }
            UpdateWeights (net);
        }
    }
}

/**
  * Tests a recurrent neural network's performance in reproducing a certain trajectory, returning error data.
  *
  * @param net The recurrent neural network whose performance is to be tested
  * @param nSequence The trajectory on whose data the network will be tested
  * @param nPasses The number of times for which the network will be run for nEpochSize steps; the PB values will be updated after each pass if bTrainPbs is true
  * @param nEpochSize The size of one training epoch (a certain number of steps after which the PB values will be updated if bTrainPbs is true)
  * @param nSkip The number of steps (not passes!) after which error data will start to be collected
  * @param bResetPbs Specifies whether the PB units should be reset to their default values (true) or not (false)
  * @param bTrainPbs Specifies whether the PB values should be updated during testing (true) or not (false)
  * @param fPbLearningRate The learning rate to be used for updating the PB values
  * @return Object containing error data for all output layers
  */
NetworkErrorData* RNNTrainingAlgorithm::Test (RNN* net, int nSequence, int nPasses, int nEpochSize, int nSkip, bool bResetPbs, bool bTrainPbs, double fPbLearningRate)
{
    net->ResetDataSources ();

    NetworkErrorData* ed = new NetworkErrorData (net, nPasses * nEpochSize);
    AllocateMemory (net, nEpochSize);

    for (int p = 0; p < nPasses+nSkip; p++)
    {
        net->Run (nSequence, nEpochSize, 0, true, (p == 0), (bResetPbs && p == 0)); // reset network only in first time-step (p=0)

        if (p >= nSkip)
            net->CollectErrorData (nSequence, ed);

        if (bTrainPbs)
        {
            ComputeGradients (net, nSequence, nEpochSize);
            UpdatePbs (net, nSequence, nEpochSize, fPbLearningRate);
        }
    }

    return ed;
}

