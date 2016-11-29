
/**
  * Constructor; creates a NetworkErrorData object based upon the structure of 'net' which will hold 'nTimeSteps' sets of error data for each output layer.
  *
  * @param net The neural network for which error data will be collected; the NetworkErrorData object needs this pointer to collect information about the number of layers and their respective sizes
  * @param nTimeSteps The number of steps for which error data will be collected
  */
NetworkErrorData::NetworkErrorData (NeuralNetwork* net, int nTimeSteps)
{
    num_layers = net->num_layers;
    led = new LayerErrorData*[net->num_layers];

    for (int l = 0; l < net->num_layers; l++)
        led[l] = (net->data_out[0][l] ? new LayerErrorData (nTimeSteps, net->layers[l]->GetSize ()) : NULL);
}

/**
  * Destructor; frees memory
  */
NetworkErrorData::~NetworkErrorData ()
{
    for (int l = 0; l < num_layers; l++)
    	delete [] led[l];

    delete [] led;
}

/**
  * Checks if error data is available for a particular layer.
  *
  * @param nLayer The zero-based index of the layer
  * @return true if error data for layer nLayer is available, false otherwise
  */
bool NetworkErrorData::IsDataAvailable (int nLayer)
{
    if (nLayer < 0 || nLayer >= num_layers)
        return false;

    return led[nLayer] != NULL;
}

/**
  * Returns a pointer to a LayerErrorData object containing error data for the specified layer.
  *
  * @param nLayer The zero-based index of the layer whose error data is to be retrieved
  * @return The error data collected for layer nLayer
  */
LayerErrorData* NetworkErrorData::GetLayerData (int nLayer)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    nnlib_assert (IsDataAvailable (nLayer), (char*) "ERROR: No error data available for layer specified in NetworkErrorData::GetLayerData!");
    #endif

    return led[nLayer];
}
