
#include <iostream>
#include "Utilities.hpp"
#include "../rnn/rnn.hpp"
#include <fstream>
#include <string>
#include <vector>

#include "Connectom.hpp"

using namespace std;


RecurrentNeuralNetworkWrapper::RecurrentNeuralNetworkWrapper(NNType::Type type, std::string path)
{
    NNtype = type;
    Path = path;
}

 RecurrentNeuralNetworkWrapper* RecurrentNeuralNetworkWrapper::GetRNN(NNType::Type type, std::string path)
{
    switch(type)
    {
        RecurrentNeuralNetworkWrapper* foo;
        case NNType::RNNT:
            foo = new RNNWrapper(path);
            return foo;
        case NNType::RNNCL:
            foo = new RNNCLWrapper(path);
            return foo;
        case NNType::RNNPB:
            foo = new RNNPBWrapper(path);
            return foo;
    }
}

std::vector<float> RecurrentNeuralNetworkWrapper::PredictNextStep(std::vector<float> object, std::vector<float> angles)
{
    passNr++;
    return neuralnetwork->RunOneTime(0, object, passNr, angles);
}

void RecurrentNeuralNetworkWrapper::LoadSettings()
{
    Utilities::WriteMessage("Loading NN-Parameters...", Utilities::Info);
    std::ifstream io;
    std::string file = Path + Utilities::NNfiles.paramsFile;
    io.open(file.c_str());
    std::string foo;

    io >> foo;
    io >> parameters.hiddenUnitsCount;
    std::cout << foo << parameters.hiddenUnitsCount << endl;

    io >> foo;
    io >> parameters.passes;
    std::cout << foo << parameters.passes << endl;

    io >> foo;
    io >> parameters.epoch;
    std::cout << foo << parameters.epoch << endl;

    io >> foo;
    io >> parameters.lRate;
    std::cout << foo << parameters.lRate << endl;

    io >> foo;
    io >> parameters.minInit;
    std::cout << foo << parameters.minInit << endl;

    io >> foo;
    io >> parameters.maxInit;
    std::cout << foo << parameters.maxInit << endl;

    io >> foo;
    io >> parameters.contextUnitCount;
    std::cout << foo << parameters.contextUnitCount << endl;

    io >> foo;
    io >> parameters.fTau;
    std::cout << foo << parameters.fTau << endl;


    Utilities::WriteBlankLine();
}



/*int RecurrentNeuralNetwork::InitRNNPB()
{

    Utilities::WriteMessage("Settng ip Neural network...", Utilities::NewProcedure);
    //Creating Layers
      layers = new Layer*[6];
      // input Layer
      layers[0] = new Layer(7);  // input

      layers[1] = new Layer(2, ActivationFunction::IDENTITY, true); // parametric bias

      layers[2] = new Layer(2); //context loop x

      //hidden Layer
      layers[3] = new Layer(12, ActivationFunction::SIGMOID);

      //output Layer
      layers[4] = new Layer(4); // output - 4DOF
      layers[5] = new Layer(2); //context loop x+1

      //Set Trainalgorithm
       cbpt_alg = new BPTT(1000, 10, 0.1, 0.0);

     //Init network
      neuralnetwork = new RNN(6, layers, cbpt_alg);

      RandomInitialization rand_init(-0.1, 0.1);

      //Build synapsis (connect neurons/layers)
        //1. hidden layer
      neuralnetwork->ConnectLayerToLayer(0, 3, &rand_init, true); // connect input to hidden layer
      neuralnetwork->ConnectLayerToLayer(1, 3, &rand_init, true); //connect PB-Layer to hidden layer
      neuralnetwork->ConnectLayerToLayer(2, 3, &rand_init, true); // connect context loop x to hidden layer

      neuralnetwork->ConnectLayerToLayer(3, 4, &rand_init, true); // connect hidden layer to output layer
      neuralnetwork->ConnectLayerToLayer(3, 5, &rand_init, true); // connect hidden layer to context loop x+1

      neuralnetwork->ConnectUnitToUnit(5, 0, 2, 0, 1, false); //connect context loop x+1 (1) to context loop x (1)
      neuralnetwork->ConnectUnitToUnit(5, 1, 2, 1, 1, false); //connect context loop x+1 (2) to context loop x (2)

       Utilities::WriteMessage("Neural netork successfully created", Utilities::OK);
       Utilities::WriteBlankLine();
      return 0;
}*/

RNNCLWrapper::RNNCLWrapper(std::string path) : RecurrentNeuralNetworkWrapper(NNType::RNNCL, path)
{

}

void RNNCLWrapper::Init()
{
    Utilities::WriteMessage("Setting ip Neural network...", Utilities::NewProcedure);

    LoadSettings();

    //Creating Layers
    layers = new Layer*[6];
    // input Layer

    layers[0] = new Layer(4);  // input prev arm angles
    layers[1] = new Layer(3);  // input object position
    layers[2] = new Layer(parameters.contextUnitCount, NULL, false, false, 20, NULL); //context loop x
    layers[3] = new Layer(parameters.hiddenUnitsCount, ActivationFunction::SIGMOID);      //hidden Layer
    layers[4] = new Layer(4); // output - 4DOF
    layers[5] = new Layer(parameters.contextUnitCount); //context loop x+1

    //Set Trainalgorithm
    cbpt_alg = new BPTT(parameters.passes, parameters.epoch, parameters.lRate, 0.0);

    //Init network
    neuralnetwork = new RNN(6, layers, cbpt_alg);

    RandomInitialization rand_init(parameters.minInit, parameters.maxInit);

    //Build synapsis (connect neurons/layers)
    //1. hidden layer
    neuralnetwork->ConnectLayerToLayer(0, 3, &rand_init, true); // connect input to hidden layer
    neuralnetwork->ConnectLayerToLayer(1, 3, &rand_init, true); //connect PB-Layer to hidden layer
    neuralnetwork->ConnectLayerToLayer(2, 3, &rand_init, true); //connect PB-Layer to hidden layer

    neuralnetwork->ConnectLayerToLayer(3, 4, &rand_init, true); // connect hidden layer to output layer
    neuralnetwork->ConnectLayerToLayer(3, 5, &rand_init, true); // connect hidden layer to context loop x+1

    Utilities::WriteMessage("Neural netork successfully created", Utilities::OK);
    Utilities::WriteBlankLine();
}

void RNNCLWrapper::LoadWeights()
{
    StaticDataSource context(10, 4);
    StaticDataSource context2(10, 3);
    neuralnetwork->SetInput(0, 0, &context, 4, 0);
    neuralnetwork->SetInput(0, 1, &context2, -1, 0);
    neuralnetwork->SetInput(0, 2, &context, 5, 0 );
    neuralnetwork->ImportWeights(Path);
    passNr =-1;
}

void RNNCLWrapper::TrainFromSource()
{
    int number_of_lines = 0;
    std::string line;
    std::ifstream myfile((Path + Utilities::NNfiles.anglesInFile).c_str());

    while (std::getline(myfile, line))
        ++number_of_lines;

    FileDataSource ds_in (number_of_lines, 3, (Path + Utilities::NNfiles.objectFile).c_str());
    FileDataSource ds_inA (number_of_lines, 4, (Path + Utilities::NNfiles.anglesInFile).c_str());
    FileDataSource ds_out (number_of_lines, 4, (Path + Utilities::NNfiles.anglesOutFile).c_str());
    StaticDataSource context(number_of_lines, 4);
    neuralnetwork->SetInput(0, 0, &ds_inA, -1, 0);
    neuralnetwork->SetInput(0, 1, &ds_in, -1, 0);
    neuralnetwork->SetInput(0, 2, &context, 5, 0 );
    neuralnetwork->SetOutput(4, &ds_out, false);

    neuralnetwork->ExportWeights(false, Path);
    neuralnetwork->Train();
    neuralnetwork->ExportWeights(true, Path);
}

RNNWrapper::RNNWrapper(std::string path) : RecurrentNeuralNetworkWrapper(NNType::RNNT, path)
{

}

void RNNWrapper::Init()
{
    Utilities::WriteMessage("Settng ip Neural network...", Utilities::NewProcedure);

    LoadSettings();

    //Creating Layers
    layers = new Layer*[6];
    // input Layer

    layers[0] = new Layer(4);  // input prev arm angles
    layers[1] = new Layer(3);  // input object position

    //hidden Layer
    layers[2] = new Layer(parameters.hiddenUnitsCount, ActivationFunction::SIGMOID);

    //output Layer
    layers[3] = new Layer(4); // output - 4DOF

    //Set Trainalgorithm

    cbpt_alg = new BPTT(parameters.passes, parameters.epoch, parameters.lRate, 0.0);

    //Init network
    neuralnetwork = new RNN(4, layers, cbpt_alg);

    RandomInitialization rand_init(parameters.minInit, parameters.maxInit);

    //Build synapsis (connect neurons/layers)
    //1. hidden layer
    neuralnetwork->ConnectLayerToLayer(0, 2, &rand_init, true); // connect input to hidden layer
    neuralnetwork->ConnectLayerToLayer(1, 2, &rand_init, true); //connect PB-Layer to hidden layer
    neuralnetwork->ConnectLayerToLayer(2, 3 , &rand_init, true); // connect hidden layer to output layer

    Utilities::WriteMessage("Neural netork successfully created", Utilities::OK);
    Utilities::WriteBlankLine();
}

void RNNWrapper::LoadWeights()
{
    StaticDataSource context(10, 4);
    StaticDataSource context2(10, 3);
    neuralnetwork->SetInput(0, 0, &context, 3, 0);
    neuralnetwork->SetInput(0, 1, &context2, -1, 0);
    neuralnetwork->ImportWeights(Path);
    passNr =-1;
}

void RNNWrapper::TrainFromSource()
{
    int number_of_lines = 0;
    std::string line;
    std::ifstream myfile((Path + Utilities::NNfiles.anglesInFile).c_str());

    while (std::getline(myfile, line))
        ++number_of_lines;

    FileDataSource ds_in (number_of_lines, 3, (Path + Utilities::NNfiles.objectFile).c_str());
    FileDataSource ds_inA (number_of_lines, 4, (Path + Utilities::NNfiles.anglesInFile).c_str());
    FileDataSource ds_out (number_of_lines, 4, (Path + Utilities::NNfiles.anglesOutFile).c_str());
    neuralnetwork->SetInput(0, 0, &ds_inA, -1, 0);
    neuralnetwork->SetInput(0, 1, &ds_in, -1, 0);
    neuralnetwork->SetOutput(3, &ds_out, false);
    neuralnetwork->ExportWeights(false, Path);
    neuralnetwork->Train();
    neuralnetwork->ExportWeights(true, Path);
}


RNNPBWrapper::RNNPBWrapper(std::string path) : RecurrentNeuralNetworkWrapper(NNType::RNNPB, path)
{

}

void RNNPBWrapper::Init()
{
    //TODO
}

void RNNPBWrapper::LoadWeights()
{
    //TODO
}

void RNNPBWrapper::TrainFromSource()
{
    //TODO
}

