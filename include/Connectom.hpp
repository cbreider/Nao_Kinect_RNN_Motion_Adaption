#ifndef CONNECTOM_HPP
#define CONNECTOM_HPP

#include <vector>
#include "string"

class RNN;
class Layer;
class BPTT;

class RecurrentNeuralNetwork
{
    public:
        RecurrentNeuralNetwork()
        {};

        int InitRNNPB();
        int InitCTRNNForRealTime(int = 30000);
        int StartNewTrainCyclus(std::vector<float> object, std::vector<float> data);
        int StartTrainingFromSource(std::string, std::string, std::string, int);
        int Reset();
        std::vector<float> PredictNextStep(std::vector<float> object, std::vector<float> angles);

        void LoadWeights(std::string );

    private:
        RNN* neuralnetwork;
        Layer** layers;
        BPTT *cbpt_alg;
        int passNr;
};


#endif
