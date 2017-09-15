#include <vector>


RNN::RNN (int nLayers, Layer** layerArray, RNNTrainingAlgorithm* rnnAlg, int nSequences) : NeuralNetwork (nLayers, layerArray, nSequences)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(rnnAlg != NULL, (char*) "ERROR: rnnAlg must not be NULL in RNN::RNN!");
    #endif

    alg = rnnAlg;


    synapses = new bool***[num_layers];
    trainable = new bool***[num_layers];

    // Set all values in synapses and trainable to false, indicating that there are no synapses yet
    for (int dst_layer = 0; dst_layer < num_layers; dst_layer++)
    {
        synapses[dst_layer] = new bool**[layers[dst_layer]->GetSize ()];
    	trainable[dst_layer] = new bool**[layers[dst_layer]->GetSize ()];
    	for (int dst_unit = 0; dst_unit < layers[dst_layer]->GetSize (); dst_unit++)
    	{
    	    synapses[dst_layer][dst_unit] = new bool*[num_layers];
    		trainable[dst_layer][dst_unit] = new bool*[num_layers];
    		for (int src_layer = 0; src_layer < num_layers; src_layer++)
    		{
    		    synapses[dst_layer][dst_unit][src_layer] = new bool[layers[src_layer]->GetSize ()];
    			trainable[dst_layer][dst_unit][src_layer] = new bool[layers[src_layer]->GetSize ()];

    			for (int src_unit = 0; src_unit < layers[src_layer]->GetSize (); src_unit++)
    			{
    			    synapses[dst_layer][dst_unit][src_layer][src_unit] = false;
                    trainable[dst_layer][dst_unit][src_layer][src_unit] = false;
    			}
    		}
    	}
    }
}


void RNN::Train (int samplecount)
{
    alg->Train (this, samplecount);
}

void RNN::Train (std::vector<int> samplecount)
{
    alg->Train (this, samplecount);
}


NetworkErrorData* RNN::Test (int nSequence, int nPasses, int nEpochSize, int nSkip, bool bResetPbs, bool bTrainPbs, double fPbLearningRate)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSequence >= 0 && nSequence < num_seq, (char*) "ERROR: Invalid nSequence parameter in RNN::Test!");
    Utilities::Assert(nPasses > 0, (char*) "ERROR: nPasses must be greater than 0 in RNN::Test!");
    Utilities::Assert(nEpochSize > 1, (char*) "ERROR: nEpochSize must be greater than 1 in RNN::Test!");
    #endif

    return alg->Test (this, nSequence, nPasses, nEpochSize, nSkip, bResetPbs, bTrainPbs, fPbLearningRate);
}

NetworkErrorData* RNN::Test (int nPasses, int nEpochSize, int nSkip, bool bResetPbs, bool bTrainPbs, double fPbLearningRate)
{
    return Test (0, nPasses, nEpochSize, nSkip, bResetPbs, bTrainPbs, fPbLearningRate);
}


void RNN::MarkLayerToLayerConnection (int nSrcLayer, int nDstLayer, bool bTrainable)
{
    for (int dst_unit = 0; dst_unit < layers[nDstLayer]->GetSize (); dst_unit++)
        for (int src_unit = 0; src_unit < layers[nSrcLayer]->GetSize (); src_unit++)
        {
            synapses[nDstLayer][dst_unit][nSrcLayer][src_unit] = true;
        	trainable[nDstLayer][dst_unit][nSrcLayer][src_unit] = bTrainable;
        }
}


void RNN::MarkUnitToLayerConnection (int nSrcLayer, int nSrcUnit, int nDstLayer, bool bTrainable)
{
    for (int dst_unit = 0; dst_unit < layers[nDstLayer]->GetSize (); dst_unit++)
    {
        synapses[nDstLayer][dst_unit][nSrcLayer][nSrcUnit] = true;
        trainable[nDstLayer][dst_unit][nSrcLayer][nSrcUnit] = bTrainable;
    }
}


void RNN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer);

    MarkLayerToLayerConnection (nSrcLayer, nDstLayer, true);
}

void RNN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer, Initialization* initFunc, bool bTrainable)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer, initFunc, bTrainable);

    MarkLayerToLayerConnection (nSrcLayer, nDstLayer, bTrainable);
}


void RNN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer, mat* weights, bool bTrainable)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer, weights, bTrainable);

    MarkLayerToLayerConnection (nSrcLayer, nDstLayer, bTrainable);
}


void RNN::ConnectUnitToLayer (int nSrcLayer, int nSrcUnit, int nDstLayer)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSrcLayer >= 0 && nSrcLayer < num_layers, (char*) "ERROR: nSrcLayer out of range in RNN::ConnectUnitToUnit!");
    Utilities::Assert(nSrcUnit >= 0 && nSrcUnit < layers[nSrcLayer]->GetSize (), (char*) "ERROR: nSrcUnit out of range in RNN::ConnectUnitToUnit!");
    Utilities::Assert(nDstLayer >= 0 && nDstLayer < num_layers, (char*) "ERROR: nDstLayer out of range in RNN::ConnectUnitToUnit!");
    #endif

    if (!w[nDstLayer][nSrcLayer])
        w[nDstLayer][nSrcLayer] = new mat (layers[nDstLayer]->GetSize (), layers[nSrcLayer]->GetSize ());

    w[nDstLayer][nSrcLayer]->zeros ();
    MarkUnitToLayerConnection (nSrcLayer, nSrcUnit, nDstLayer, true);
}
void RNN::ConnectLayerToLayerStatic(int nSrcLayer, int nDstLayer, RandomInitialization* initFunc, bool bTrainable)
{

    NeuralNetwork::ConnectLayerToLayerStatic (nSrcLayer, nDstLayer, initFunc, bTrainable);

    MarkLayerToLayerConnection (nSrcLayer, nDstLayer, bTrainable);
}

void RNN::ConnectUnitToLayer (int nSrcLayer, int nSrcUnit, int nDstLayer, Initialization* initFunc, bool bTrainable)
{
    ConnectUnitToLayer (nSrcLayer, nSrcUnit, nDstLayer);

    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(initFunc != NULL, (char*) "ERROR: initFunc is NULL in RNN::SetLayerToLayerWeights!");
    #endif

    vec weights (layers[nDstLayer]->GetSize ());
    initFunc->Initialize (&weights);
    w[nDstLayer][nSrcLayer]->col (nSrcUnit) = weights;
    MarkUnitToLayerConnection (nSrcLayer, nSrcUnit, nDstLayer, bTrainable);
}


void RNN::ConnectUnitToLayer (int nSrcLayer, int nSrcUnit, int nDstLayer, rowvec* weights, bool bTrainable)
{
    ConnectUnitToLayer (nSrcLayer, nSrcUnit, nDstLayer);

    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(weights != NULL, (char*) "ERROR: weights is NULL in RNN::SetLayerToLayerWeights!");
    Utilities::Assert(weights->n_cols == w[nDstLayer][nSrcLayer]->n_rows, (char*) "ERROR: Vector weights has wrong dimension in RNN::ConnectUnitToLayer!");
    #endif

    w[nDstLayer][nSrcLayer]->col (nSrcUnit) = trans (*weights);
    MarkUnitToLayerConnection (nSrcLayer, nSrcUnit, nDstLayer, bTrainable);
}


void RNN::ConnectUnitToUnit (int nSrcLayer, int nSrcUnit, int nDstLayer, int nDstUnit, double fWeight, bool bTrainable)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSrcLayer >= 0 && nSrcLayer < num_layers, (char*) "ERROR: nSrcLayer out of range in RNN::ConnectUnitToUnit!");
    Utilities::Assert(nSrcUnit >= 0 && nSrcUnit < layers[nSrcLayer]->GetSize (), (char*) "ERROR: nSrcUnit out of range in RNN::ConnectUnitToUnit!");
    Utilities::Assert(nDstLayer >= 0 && nDstLayer < num_layers, (char*) "ERROR: nDstLayer out of range in RNN::ConnectUnitToUnit!");
    Utilities::Assert(nDstUnit >= 0 && nDstUnit < layers[nDstLayer]->GetSize (), (char*) "ERROR: nDstUnit out of range in RNN::ConnectUnitToUnit!");
    #endif

    if (!w[nDstLayer][nSrcLayer])
        w[nDstLayer][nSrcLayer] = new mat (layers[nDstLayer]->GetSize (), layers[nSrcLayer]->GetSize ());

    (*w[nDstLayer][nSrcLayer]) (nDstLayer, nSrcUnit)  = fWeight;

    synapses[nDstLayer][nDstUnit][nSrcLayer][nSrcUnit] = true;
    trainable[nDstLayer][nDstUnit][nSrcLayer][nSrcUnit] = bTrainable;
}
