#ifndef CONNECTOM_HPP
#define CONNECTOM_HPP

class RNN;
class Layer;

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
};


#endif
