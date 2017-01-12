#ifndef CONNECTOM_HPP
#define CONNECTOM_HPP

#include <vector>
#include "string"

class RNN;
class Layer;
class BPTT;

class Connectom
{
    public:
        Connectom()
        {};

        int InitRNNPB();
        int InitCTRNNForRealTime(int = 1);
        int StartNewTrainCyclus(std::vector<float> object, std::vector<float> data);
        int StartTrainingFromSource(std::string, std::string, std::string, int);
        int Reset();

        void LoadWeights(std::string );

    private:
        RNN* neuralnetwork;
        Layer** layers;
        BPTT *cbpt_alg;
};


#endif
