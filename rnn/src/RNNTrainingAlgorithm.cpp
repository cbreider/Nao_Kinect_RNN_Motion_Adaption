#include <vector>
#include "Utilities.hpp"


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

    sum_e.push_back(0);
    sum_e.push_back(0);
    sum_e.push_back(0);
    sum_e.push_back(0);
}


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
                net->Run (seq, epoch_size, 0, false, (p == 0), (p == 0)); // collect states, reset network only before first pass
                ComputeGradients (net, seq, epoch_size);
                UpdatePbs (net, seq, epoch_size, lr_pb);
            }
            if(p%1000 == 0)
            {
                float err = 0;
                std::cout << "PassNr.: " << p << endl;
                for(int i = 0; i< net->num_seq; i++)
                {
                    err += sum_e[i];
                    std::cout << "Seq: " << i<< " error: "<< sum_e[i] / (1000 * epoch_size) << endl;
                    sum_e[i] = 0;
                    for(int iii = 0; iii< net->num_layers; iii++)
                    {
                         if(net->layers[iii]->IsPbLayer())
                         {
                            for(int ii = 0; ii < net->layers[iii]->GetSize(); ii++)
                            {
                                std::cout << "     PB" << ii <<": "<< net->u_def[i][iii] (ii) << endl;
                            }
                         }
                    }

                }
                std::cout << endl << "overall error: " << err / (1000 * epoch_size );
                std::cout << "\n \n";
            }
            UpdateWeights (net);
        }
    }
}

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

