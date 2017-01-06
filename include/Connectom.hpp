#ifndef CONNECTOM_HPP
#define CONNECTOM_HPP

#include <vector>

class RNN;
class Layer;
class BPTT;
class Connectom
{
    public:
        Connectom()
        {};

        int InitRNNPB();
        int InitCTRNNForRealTime();
        int StartNewTrainCyclus(std::vector<float> object, std::vector<float> data);
        int StarTrainingFromSource();
        int Reset();


    private:
        RNN* neuralnetwork;
        Layer** layers;
        BPTT *cbpt_alg;
};


#endif
