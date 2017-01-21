#ifndef NETWORKERRORDATA_HPP_INCLUDED
#define NETWORKERRORDATA_HPP_INCLUDED

/**
  * Represents error data collected during the testing phase of a neural network.
  */
class NetworkErrorData
{
    public:
        NetworkErrorData (NeuralNetwork*, int);
        ~NetworkErrorData ();
        bool IsDataAvailable (int);
        LayerErrorData* GetLayerData (int);

    private:
        int num_layers;
        LayerErrorData** led;
};

#endif // NETWORKERRORDATA_HPP_INCLUDED
