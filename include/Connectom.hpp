#ifndef CONNECTOM_HPP
#define CONNECTOM_HPP

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include "Utilities.hpp"

class RNN;
class Layer;
class BPTT;
class RNNWrapper;
class RNNCLWrapper;
class RNNPBWrapper;

namespace NNType {
enum Type
{
   RNNT,
   RNNCL,
   RNNPB,
};
}

class RecurrentNeuralNetworkWrapper
{
    public:

        RecurrentNeuralNetworkWrapper(NNType::Type type, std::string path);
        static RecurrentNeuralNetworkWrapper* GetRNN(NNType::Type type, std::string path);
        virtual void Init() = 0;
        virtual void TrainFromSource() = 0;
        virtual void LoadWeights() = 0;
        virtual void Test() = 0;
        int Reset(){}
        void PredictNextStep(std::vector<float> object, std::vector<float> angles);

        NNType::Type NNtype;

    private:

        RNN* neuralnetwork;
        Layer** layers;
        BPTT *cbpt_alg;
        int passNr = -1;
        std::string Path;

        friend class RNNWrapper;
        friend class RNNCLWrapper;
        friend class RNNPBWrapper;

};

class RNNWrapper : RecurrentNeuralNetworkWrapper
{
    public:
        RNNWrapper(std::string);

        void Init();
        void TrainFromSource();
        void LoadWeights();
        void Test();
        friend class RecurrentNeuralNetworkWrapper;
};

class RNNCLWrapper : RecurrentNeuralNetworkWrapper
{
    public:
        RNNCLWrapper(std::string);
        void Init();
        void TrainFromSource();
        void LoadWeights();
        void Test();

        friend class RecurrentNeuralNetworkWrapper;
};

class RNNPBWrapper : RecurrentNeuralNetworkWrapper
{
    public:
        RNNPBWrapper(std::string);
        void Init();
        void TrainFromSource();
        void LoadWeights();
        void Test();
   private:
        int pb_count = 0;
        int nr_seq = 0;
        friend class RecurrentNeuralNetworkWrapper;
};

#endif
