
/**
  * Creates a new Echo State Network with nLayers layers (the layers are stored in layerArray) that will be able to learn nSequences different trajectories.
  *
  * @param nLayers The ESN's number of layers
  * @param layerArray The layers to be used
  * @param nSequences Number of trajectories to be learnt
  */
ESN::ESN (int nLayers, ESNLayer** layerArray, int nSequences) : NeuralNetwork (nLayers, (Layer**) layerArray, nSequences)
{
    is_trainable = new bool*[num_layers];

    for (int l = 0; l < num_layers; l++)
    {
    	w[l][l] = new mat (layers[l]->GetSize (), layers[l]->GetSize ());
    	((ESNLayer*) layers[l])->GetDefaultWeights (w[l][l]);
    	is_trainable[l] = new bool[num_layers];
        memset (is_trainable[l], 0, num_layers * sizeof (bool));
    }
}

/**
  * Destructor
  */
ESN::~ESN ()
{
    for (int i = 0; i < num_layers; i++)
        delete [] is_trainable[i];

    delete [] is_trainable;
}

/**
  * Adapts the activation functions of all layers with adaptable activation functions.
  *
  * @param nSteps Number of steps during which adaptation is to be performed (adaptation will be performed after each step)
  * @param nSkip Number of steps to be skipped before adaptation is performed
  */
void ESN::Adapt (int nSteps, int nSkip)
{
    vec temp_x, temp_y;

    // Run the network for nSkip steps to wash out initial states
    for (int seq = 0; seq < num_seq; seq++)
        Run (seq, 0, nSkip, false, true, true);

    // Run the network, adapting the adaptable activation functions after each step
    for (int i = 0; i < nSteps; i++)
        for (int seq = 0; seq < num_seq; seq++)
        {
            Run (seq, 1, 0, false, i == 0, i == 0);

            for (int l = 0; l < num_layers; l++)
                if (((ESNLayer*) layers[l])->IsAdaptable ())
                {
                    temp_x = u[seq][l].col (0);
                    temp_y = trans (s[seq][l].row (0));
                    ((ESNLayer*) layers[l])->Adapt (&temp_x, &temp_y);
                }
        }
}

/**
  * Trains the ESN; After nSkip steps are skipped, the nPasses state vectors are collected for training
  *
  * @param nPasses Number of state vectors to be collected for training
  * @param nSkip Number of steps to be skipped before training
  */
void ESN::Train (int nPasses, int nSkip)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nPasses > 0, (char*) "ERROR: nPasses must be greater than 0 in ESN::Train!");
    Utilities::Assert(nSkip >= 0, (char*) "ERROR: nSkip must be at least 0 in ESN::Train!");
    #endif

    ResetDataSources ();

    for (int seq = 0; seq < num_seq; seq++)
    	Run (seq, nPasses, nSkip, false, true, true);

    UpdateWeights ();
}

/**
  * Tests the ESN's performance in reproducing trajectory nSequence, running the network for nPasses steps after skipping nSkip steps.
  *
  * @param nSequence Specifies the trajectory whose data (input and desired-output) will be used for testing; different data may be used during the training and testing phase if NeuralNetwork::SetInput or NeuralNetwork::SetOutput is called after training.
  * @param nPasses The number of steps for which the test will be run
  * @param nSkip The Number of steps to be skipped before error data is collected
  */
NetworkErrorData* ESN::Test (int nSequence, int nPasses, int nSkip)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSequence >= 0 && nSequence < num_seq, (char*) "ERROR: Invalid nSequence parameter in ESN::Test!");
    Utilities::Assert(nPasses > 0, (char*) "ERROR: nPasses must be greater than 0 in ESN::Train!");
    Utilities::Assert(nSkip >= 0, (char*) "ERROR: nSkip must be at least 0 in ESN::Train!");
    #endif

    ResetDataSources ();
    Run (nSequence, nPasses, nSkip, true, true, true);

    NetworkErrorData* ed = new NetworkErrorData (this, nPasses);
    CollectErrorData (nSequence, ed);

    return ed;
}

/**
  * Calls ESN::Test (int nSequence, int nPasses, int nSkip) with nSequence=0
  */
NetworkErrorData* ESN::Test (int nPasses, int nSkip)
{
    return Test (0, nPasses, nSkip);
}

/**
  * Computes the ESN's updated weight matrices
  */
void ESN::UpdateWeights ()
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    // Check if desired outputs were specified correctly
    for (int l = 0; l < num_layers; l++)
    {
        bool has_output = (data_out[0][l] != NULL);

        for (int seq = 1; seq < num_seq; seq++)
        	Utilities::Assert((data_out[seq][l] != NULL) == has_output, (char*) "ERROR: If an ESN is to learn multiple patterns and a layer has desired outputs for one pattern, it also needs desired outputs for the other ones!");
    }
    #endif

    int passes = s[0][0].n_rows; // s[0][0].n_rows contains the number of states that were collected

    // Iterate over all layers
    for (int dst_layer = 0; dst_layer < num_layers; dst_layer++)
    {
        // Only weights of synapses ending in an output layer can be adjusted
        if (data_out[0][dst_layer])
        {
            int units = 0, offset_slice = 0;

            // Are any trainable connections ending in this layer?
            for (int src_layer = 0; src_layer < num_layers; src_layer++)
                if (is_trainable[dst_layer][src_layer])
                    units += layers[src_layer]->GetSize ();

            // Adjust trainable weights, provided that there are any
            if (units > 0)
            {
                mat slices (passes * num_seq, units);
                mat d_all (passes * num_seq, layers[dst_layer]->GetSize ());

                // If multiple trajectories are to be learned, concatenate the desired-output matrices
                for (int seq = 0; seq < num_seq; seq++)
                	d_all.submat (passes * seq, 0, passes * (seq + 1) - 1, layers[dst_layer]->GetSize () - 1) = d[seq][dst_layer];

                // Write the inputs applied to trainable connections into a separate matrix so only those connections will be trained
                for (int src_layer = 0; src_layer < num_layers; src_layer++)
                    if (is_trainable[dst_layer][src_layer])
                    {
                        for (int seq = 0; seq < num_seq; seq++)
                            slices.submat (passes * seq, offset_slice, passes * (seq + 1) - 1, offset_slice + layers[src_layer]->GetSize () - 1) = s[seq][src_layer];

                        offset_slice += layers[src_layer]->GetSize ();
                    }

                // Compute the updated weights
                mat w_new = ((ESNLayer*) layers[dst_layer])->ComputeWeightMatrix (&slices, &d_all);
                int offset_w_new = 0;

                // Write the new weights into the old weight matrices
                for (int src_layer = 0; src_layer < num_layers; src_layer++)
                    if (is_trainable[dst_layer][src_layer])
                    {
                        (*w[dst_layer][src_layer]) = w_new.submat (0, offset_w_new, layers[dst_layer]->GetSize () - 1, offset_w_new + layers[src_layer]->GetSize () - 1);
                        offset_w_new += layers[src_layer]->GetSize ();
                    }
            }
        }
    }
}

/**
  * Connects a layer to another layer. Overwrites the inherited method because some functionality still needed to be added.
  *
  * @param nSrcLayer Index of the source layer
  * @param nDstLayer Index of the destination layer
  *
  */
void ESN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer);

    w[nDstLayer][nSrcLayer]->zeros ();
    is_trainable[nDstLayer][nSrcLayer] = true;
}

/**
  * Connects a layer to another layer, initializes the connections' weights, and specifies whether the connections will be trainable. Overwrites the inherited method because some functionality still needed to be added.
  *
  * @param nSrcLayer Index of the source layer
  * @param nDstLayer Index of the destination layer
  * @param initFunc The initialization function to be used
  * @param bTrainable Specifies whether the connections will be trainable or not
  *
  */
void ESN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer, Initialization* initFunc, bool bTrainable)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer, initFunc, bTrainable);

    is_trainable[nDstLayer][nSrcLayer] = bTrainable;
}

/**
  * Connects a layer to another layer, initializes the connections' weights, and specifies whether the connections will be trainable. Overwrites the inherited method because some functionality still needed to be added.
  *
  * @param nSrcLayer Index of the source layer
  * @param nDstLayer Index of the destination layer
  * @param weights The weight matrix to be used for initialization of the weights (entry [x, y] will be the weight used for the connection from unit x in nSrcLayer to unit y in nDstLayer)
  * @param bTrainable Specifies whether the connections will be trainable or not
  *
  */
void ESN::ConnectLayerToLayer (int nSrcLayer, int nDstLayer, mat* weights, bool bTrainable)
{
    NeuralNetwork::ConnectLayerToLayer (nSrcLayer, nDstLayer, weights, bTrainable);

    is_trainable[nDstLayer][nSrcLayer] = bTrainable;
}
