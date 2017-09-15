

NetworkErrorData::NetworkErrorData (NeuralNetwork* net, int nTimeSteps)
{
    num_layers = net->num_layers;
    led = new LayerErrorData*[net->num_layers];

    for (int l = 0; l < net->num_layers; l++)
        led[l] = (net->data_out[0][l] ? new LayerErrorData (nTimeSteps, net->layers[l]->GetSize ()) : NULL);
}


NetworkErrorData::~NetworkErrorData ()
{
    for (int l = 0; l < num_layers; l++)
    	delete [] led[l];

    delete [] led;
}


bool NetworkErrorData::IsDataAvailable (int nLayer)
{
    if (nLayer < 0 || nLayer >= num_layers)
        return false;

    return led[nLayer] != NULL;
}


LayerErrorData* NetworkErrorData::GetLayerData (int nLayer)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(IsDataAvailable (nLayer), (char*) "ERROR: No error data available for layer specified in NetworkErrorData::GetLayerData!");
    #endif

    return led[nLayer];
}
