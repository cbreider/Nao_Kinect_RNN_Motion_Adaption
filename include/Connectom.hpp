#ifndef CONNECTOM_HPP
#define CONNECTOM_HPP

class RNN;
class Layer;
class BPTT;
class Connectom
{
    public:
        Connectom()
        {};

        int Init();
        int StartNewTrainCyclus();
        int Reset();


    private:
        RNN* neuralnetwork;
        Layer** layers;
        BPTT *cbpt_alg;
};


#endif
