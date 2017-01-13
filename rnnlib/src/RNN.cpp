
/**
  * Creates a new recurrent neural network consisting of nLayers layers stored in layerArray; the network will be able to learn nSequences trajectories using the training algorithm specified by rnnAlg.
  *
  * @param nLayers The number of layers in the network
  * @param layerArray An array containing pointers to the Layer objects which the network will be used as the network's layers
  * @param rnnAlg The training algorithm to be used
  * @param nSequences The number of a trajectories the network will be able to learn
  */
RNN::RNN (int nLayers, Layer** layerArray, RNNTrainingAlgorithm* rnnAlg, int nSequences) : NeuralNetwork (nLayers, layerArray, nSequences)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (rnnAlg != NULL, (char*) "ERROR: rnnAlg must not be NULL in RNN::RNN!");
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

/**
  * Trains the network using the algorithm passed to the constructor.
  */
void RNN::Train ()
{
    alg->Train (this);
}

/**
  * Tests the network's performance in reproducing a certain trajectory.
  *
  * @param nSequence The trajectory on whose data the network will be tested
  * @param nPasses The number of steps during which error data will be collected
  * @param nEpochSize The size of one epoch; this is only relevant if the Parametric Bias (PB) units are to be trained, as their values will be adjusted after nEpochSize steps
  * @param nSkip The number of epochs for which the network will be run before error data is collected
  * @param bResetPbs Specifies whether the PB units should be reset to their original values (true) or not (false)
  * @param bTrainPbs Specifies whether the PB units should be trained (true) or not (false)
  * @param fPbLearningRate Specifies the learning rate to be used for training the PB units
  */
NetworkErrorData* RNN::Test (int nSequence, int nPasses, int nEpochSize, int nSkip, bool bResetPbs, bool bTrainPbs, double fPbLearningRate)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nSequence >= 0 && nSequence < num_seq, (char*) "ERROR: Invalid nSequence parameter in RNN::Test!");
    nnlib_assert (nPasses > 0, (char*) "ERROR: nPasses must be greater than 0 in RNN::Test!");
    nnlib_assert (nEpochSize > 1, (char*) "ERROR: nEpochSize must be greater than 1 in RNN::Test!");
    #endif

    return alg->Test (this, nSequence, nPasses, nEpochSize, nSkip, bResetPbs, bTrainPbs, fPbLearningRate);
}

/**
  * Calls Test (int nSequence, int nPasses, int nEpochSize, int nSkip, bool bResetPbs, bool bTrainPbs, double fPbLearningRate) with nSequence=0. See the doumentation of that method for information about the parameters.
  */
NetworkErrorData* RNN::Test (int nPasses, int nEpochSize, int nSkip, bool bResetPbs, bool bTrainPbs, double fPbLearningRate)
{
    return Test (0, nPasses, nEpochSize, nSkip, bResetPbs, bTrainPbs, fPbLearningRate);
}

/**
  * Connects one layer (nSrcLayer) to another layer (nDstLayer), specifying whether the connections will be trainable or not.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nDstLayer The zero-based index of the destination layer
  * @param bTrainable Specifies whether the connections will be trainable (true) or not (false)
  */
void RNN::MarkLayerToLayerConnection (int nSrcLayer, int nDstLayer, bool bTrainable)
{
    for (int dst_unit = 0; dst_unit < layers[nDstLayer]->GetSize (); dst_unit++)
        for (int src_unit = 0; src_unit < layers[nSrcLayer]->GetSize (); src_unit++)
        {
            synapses[nDstLayer][dst_unit][nSrcLayer][src_unit] = true;
        	trainable[nDstLayer][dst_unit][nSrcLayer][src_unit] = bTrainable;
        }
}

/**
  * Connects one unit to a layer, specifying whether the connections will be trainable or not.
  *
  * @param nSrcLayer The zero-based index of the source unit's layer
  * @param nSrcUnit The unit's zero-based index inside nSrcLayer
  * @param nDstLayer The zero-based index of the destination layer
  * @param bTrainable Specifies whether the connections will be trainable (true) or not (false)
  */
void RNN::MarkUnitToLayerConnection (int nSrcLayer, int nSrcUnit, int nDstLayer, bool bTrainable)
{
    for (int dst_unit = 0; dst_unit < layers[nDstLayer]->GetSize (); dst_unit++)
    {
        synapses[nDstLayer][dst_unit][nSrcLayer][nSrcUnit] = true;
        trainable[nDstLayer][dst_unit][nSrcLayer][nSrcUnit] = bTrainable;
    }
}

/**
  * Connects one layer to another layer; the connections will be trainable. Overwrites the inherited method because some functionality still needed to be added.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nDstLayer The zero-based index of the destination layer
  */
void RNN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer);

    MarkLayerToLayerConnection (nSrcLayer, nDstLayer, true);
}

/**
  * Connects one layer to another layer, initializing the connection weights and specifying whether they will be trainable or not. Overwrites the inherited method because some functionality still needed to be added.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nDstLayer The zero-based index of the destination layer
  * @param initFunc The initialization method to be used for initializing the connections
  * @param bTrainable Specifies whether the connections will be trainable (true) or not (false)
  */
void RNN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer, Initialization* initFunc, bool bTrainable)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer, initFunc, bTrainable);

    MarkLayerToLayerConnection (nSrcLayer, nDstLayer, bTrainable);
}

/**
  * Connects one layer (nSrcLayer) to another layer (nDstLayer), initializing the connection weights and specifying whether they will be trainable or not. Overwrites the inherited method because some functionality still needed to be added.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nDstLayer The zero-based index of the destination layer
  * @param weights A matrix containing the connection weights; entry (x, y) specifies the weight to be used for the connection from unit x in nSrcLayer to unit y in nDstLayer
  * @param bTrainable Specifies whether the connections will be trainable (true) or not (false)
  */
void RNN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer, mat* weights, bool bTrainable)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer, weights, bTrainable);

    MarkLayerToLayerConnection (nSrcLayer, nDstLayer, bTrainable);
}


/**
  * Connects one unit to a layer; the connections will be trainable.
  *
  * @param nSrcLayer The zero-based index of the source unit's layer
  * @param nSrcUnit The unit's zero-based index inside nSrcLayer
  * @param nDstLayer The zero-based index of the destination layer
  */
void RNN::ConnectUnitToLayer (int nSrcLayer, int nSrcUnit, int nDstLayer)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nSrcLayer >= 0 && nSrcLayer < num_layers, (char*) "ERROR: nSrcLayer out of range in RNN::ConnectUnitToUnit!");
    nnlib_assert (nSrcUnit >= 0 && nSrcUnit < layers[nSrcLayer]->GetSize (), (char*) "ERROR: nSrcUnit out of range in RNN::ConnectUnitToUnit!");
    nnlib_assert (nDstLayer >= 0 && nDstLayer < num_layers, (char*) "ERROR: nDstLayer out of range in RNN::ConnectUnitToUnit!");
    #endif

    if (!w[nDstLayer][nSrcLayer])
        w[nDstLayer][nSrcLayer] = new mat (layers[nDstLayer]->GetSize (), layers[nSrcLayer]->GetSize ());

    w[nDstLayer][nSrcLayer]->zeros ();
    MarkUnitToLayerConnection (nSrcLayer, nSrcUnit, nDstLayer, true);
}

/**
  * Connects one unit to a layer, initializing the connection weights and specifying whether they will be trainable or not.
  *
  * @param nSrcLayer The zero-based index of the unit's layer
  * @param nSrcUnit The unit's zero-based index inside nSrcLayer
  * @param nDstLayer The zero-based index of the destination layer
  * @param initFunc The initialization method to be used for initializing the connections
  * @param bTrainable Specifies whether the connections will be trainable (true) or not (false)
  */
void RNN::ConnectUnitToLayer (int nSrcLayer, int nSrcUnit, int nDstLayer, Initialization* initFunc, bool bTrainable)
{
    ConnectUnitToLayer (nSrcLayer, nSrcUnit, nDstLayer);

    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (initFunc != NULL, (char*) "ERROR: initFunc is NULL in RNN::SetLayerToLayerWeights!");
    #endif

    vec weights (layers[nDstLayer]->GetSize ());
    initFunc->Initialize (&weights);
    w[nDstLayer][nSrcLayer]->col (nSrcUnit) = weights;
    MarkUnitToLayerConnection (nSrcLayer, nSrcUnit, nDstLayer, bTrainable);
}

/**
  * Connects one unit to a layer, initializing the connection weights and specifying whether they will be trainable or not.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nSrcUnit The unit's zero-based index inside nSrcLayer
  * @param nDstLayer The zero-based index of the destination layer
  * @param weights A row vector containing the connection weights; entry x specifies the weight to be used for the connection from nSrcUnit to unit x in nDstLayer
  * @param bTrainable Specifies whether the connections will be trainable (true) or not (false)
  */
void RNN::ConnectUnitToLayer (int nSrcLayer, int nSrcUnit, int nDstLayer, rowvec* weights, bool bTrainable)
{
    ConnectUnitToLayer (nSrcLayer, nSrcUnit, nDstLayer);

    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (weights != NULL, (char*) "ERROR: weights is NULL in RNN::SetLayerToLayerWeights!");
    nnlib_assert (weights->n_cols == w[nDstLayer][nSrcLayer]->n_rows, (char*) "ERROR: Vector weights has wrong dimension in RNN::ConnectUnitToLayer!");
    #endif

    w[nDstLayer][nSrcLayer]->col (nSrcUnit) = trans (*weights);
    MarkUnitToLayerConnection (nSrcLayer, nSrcUnit, nDstLayer, bTrainable);
}

/**
  * Connects one unit to another unit, setting the connection weight and specifying whether it will be trainable or not.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nSrcUnit The source unit's zero-based index inside nSrcLayer
  * @param nDstLayer The zero-based index of the destination layer
  * @param nSrcUnit The destination unit's zero-based index inside nSrcLayer
  * @param fWeight The weight to be assigned to the connection
  * @param bTrainable Specifies whether the connection will be trainable (true) or not (false)
  */
void RNN::ConnectUnitToUnit (int nSrcLayer, int nSrcUnit, int nDstLayer, int nDstUnit, double fWeight, bool bTrainable)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (nSrcLayer >= 0 && nSrcLayer < num_layers, (char*) "ERROR: nSrcLayer out of range in RNN::ConnectUnitToUnit!");
    nnlib_assert (nSrcUnit >= 0 && nSrcUnit < layers[nSrcLayer]->GetSize (), (char*) "ERROR: nSrcUnit out of range in RNN::ConnectUnitToUnit!");
    nnlib_assert (nDstLayer >= 0 && nDstLayer < num_layers, (char*) "ERROR: nDstLayer out of range in RNN::ConnectUnitToUnit!");
    nnlib_assert (nDstUnit >= 0 && nDstUnit < layers[nDstLayer]->GetSize (), (char*) "ERROR: nDstUnit out of range in RNN::ConnectUnitToUnit!");
    #endif

    synapses[nDstLayer][nDstUnit][nSrcLayer][nSrcUnit] = true;
    trainable[nDstLayer][nDstUnit][nSrcLayer][nSrcUnit] = bTrainable;
}
