
#include <iostream>
#include "Utilities.hpp"
#include "../rnn/rnn.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <tr1/memory>
#include <memory>
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

void RecurrentNeuralNetworkWrapper::PredictNextStep(std::vector<float> object, std::vector<float> angles)
{
    //if(passNr == 80) passNr = 0;
    passNr++;
    neuralnetwork->RunAfterTraining(0, 80, 0, false, true, true, angles);
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

    //Creating Layers
    layers = new Layer*[4];
    // input Layer

    layers[0] = new Layer(4);  // input prev arm angles
    layers[1] = new Layer(Utilities::Parameters.hiddenUnitsCount,  ActivationFunction::SIGMOID, false, false, Utilities::Parameters.fTau, NULL); //context loop x
    layers[2] = new Layer(Utilities::Parameters.hiddenUnitsCount, ActivationFunction::SIGMOID);      //hidden Layer
    layers[3] = new Layer(4); // output - 4DOF
    //layers[4] = new Layer(Utilities::Parameters.contextUnitCount); //context loop x+1

    //Set Trainalgorithm
    cbpt_alg = new BPTT(Utilities::Parameters.passes, Utilities::Parameters.epoch, Utilities::Parameters.lRate, 0.0);

    //Init network
    neuralnetwork = new RNN(4, layers, cbpt_alg);

    RandomInitialization rand_init(Utilities::Parameters.minInit, Utilities::Parameters.maxInit);

    //Build synapsis (connect neurons/layers)
    //1. hidden layer
    neuralnetwork->ConnectLayerToLayer(0, 2, &rand_init, true); // connect input to hidden layer
    neuralnetwork->ConnectLayerToLayer(1, 2, &rand_init, true); //connect PB-Layer to hidden layer

    neuralnetwork->ConnectLayerToLayer(2, 3, &rand_init, true); // connect hidden layer to output layer
    neuralnetwork->ConnectLayerToLayer(2, 1, &rand_init, true); // connect hidden layer to context loop x+1
    //neuralnetwork->ConnectLayerToLayer(4, 1, &rand_init, true); // connect hidden layer to context loop x+1
    for(int i = 0; i< Utilities::Parameters.hiddenUnitsCount; i++)
    {
        neuralnetwork->ConnectUnitToUnit(2, i,  1, i, Initialization::GenerateRandomWeight(-0.5, 0.5), true);
    }
    Utilities::WriteMessage("Neural netork successfully created", Utilities::OK);
    Utilities::WriteBlankLine();
}

void RNNCLWrapper::LoadWeights()
{
    StaticDataSource context(10, 4);
    StaticDataSource context2(10, 3);
    neuralnetwork->SetInput(0, 0, &context, 3, 0);
    //neuralnetwork->SetInput(0, 1, &context2, 4, 0);
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

    FileDataSource ds_in (number_of_lines, 3, (Path + Utilities::NNfiles.objectFile).c_str(), 1);
    FileDataSource ds_inA (number_of_lines, 4, (Path + Utilities::NNfiles.anglesInFile).c_str(), 2);
    FileDataSource ds_out (number_of_lines, 4, (Path + Utilities::NNfiles.anglesOutFile).c_str(), 2);
    StaticDataSource context(number_of_lines, 4);

    neuralnetwork->SetInput(0, 0, &ds_inA, -1, 0);
    //neuralnetwork->SetInput(0, 1, &ds_in, -1, 0);
    neuralnetwork->SetOutput(3, &ds_out, false);

    neuralnetwork->ExportWeights(false, Path);
    neuralnetwork->Train(number_of_lines);
    neuralnetwork->ExportWeights(true, Path);
}

RNNWrapper::RNNWrapper(std::string path) : RecurrentNeuralNetworkWrapper(NNType::RNNT, path)
{

}

void RNNWrapper::Init()
{
    Utilities::WriteMessage("Settng ip Neural network...", Utilities::NewProcedure);

    //Creating Layers
    layers = new Layer*[6];
    // input Layer

    //layers[0] = new Layer(4);  // input prev arm angles
    layers[0] = new Layer(4);  // input object position
    layers[1] = new Layer(3);  // input object position
    //hidden Layer
    layers[2] = new Layer(Utilities::Parameters.hiddenUnitsCount, ActivationFunction::SIGMOID);

    //output Layer
    layers[3] = new Layer(4); // output - 4DOF

    //Set Trainalgorithm

    cbpt_alg = new BPTT(Utilities::Parameters.passes, Utilities::Parameters.epoch, Utilities::Parameters.lRate, 0.0);

    //Init network
    neuralnetwork = new RNN(4, layers, cbpt_alg);

    RandomInitialization rand_init(Utilities::Parameters.minInit, Utilities::Parameters.maxInit);

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

    FileDataSource ds_in (number_of_lines, 3, (Path + Utilities::NNfiles.objectFile).c_str(), 1);
    FileDataSource ds_inA (number_of_lines, 4, (Path + Utilities::NNfiles.anglesInFile).c_str(), 2);
    FileDataSource ds_out (number_of_lines, 4, (Path + Utilities::NNfiles.anglesOutFile).c_str(), 2);
    neuralnetwork->SetInput(0, 0, &ds_inA, -1, 0);
    neuralnetwork->SetInput(0, 1, &ds_in, -1, 0);
    neuralnetwork->SetOutput(3, &ds_out, false);
    neuralnetwork->ExportWeights(false, Path);
    neuralnetwork->Train(number_of_lines);
    neuralnetwork->ExportWeights(true, Path);
}


RNNPBWrapper::RNNPBWrapper(std::string path) : RecurrentNeuralNetworkWrapper(NNType::RNNPB, path)
{

}

void RNNPBWrapper::Init()
{
    Utilities::WriteMessage("Setting ip Neural network...", Utilities::NewProcedure);

    //Creating Layers
    layers = new Layer*[5];
    // input Layer

    layers[0] = new Layer(4);  // input prev arm angles
    layers[1] = new Layer(Utilities::Parameters.hiddenUnitsCount, ActivationFunction::SIGMOID, false, false, Utilities::Parameters.fTau, NULL); //context loop x
    layers[2] = new Layer(Utilities::Parameters.hiddenUnitsCount, ActivationFunction::SIGMOID);      //hidden Layer
    layers[3] = new Layer(4); // output - 4DOF
    //layers[4] = new Layer(Utilities::Parameters.contextUnitCount); //context loop x+1
    layers[4] = new Layer(2, ActivationFunction::SIGMOID, true); //pblayer

    //Set Trainalgorithm
    cbpt_alg = new BPTT(Utilities::Parameters.passes, Utilities::Parameters.epoch, Utilities::Parameters.lRate, 0.1);

    //Init network
    neuralnetwork = new RNN(5, layers, cbpt_alg, 4);

    RandomInitialization rand_init(Utilities::Parameters.minInit, Utilities::Parameters.maxInit);

    //Build synapsis (connect neurons/layers)
    //1. hidden layer
    neuralnetwork->ConnectLayerToLayer(0, 2, &rand_init, true); // connect input to hidden layer
    neuralnetwork->ConnectLayerToLayer(1, 2, &rand_init, true); //connect PB-Layer to hidden layer
    neuralnetwork->ConnectLayerToLayer(4, 2, &rand_init, false);
    neuralnetwork->ConnectLayerToLayer(2, 3, &rand_init, true); // connect hidden layer to output layer
    //neuralnetwork->ConnectLayerToLayer(2, 4, &rand_init, true); // connect hidden layer to context loop x+1
    for(int i = 0; i< Utilities::Parameters.hiddenUnitsCount; i++)
    {
        neuralnetwork->ConnectUnitToUnit(2, i,  1, i, Initialization::GenerateRandomWeight(-0.5, 0.5), true);
    }
    Utilities::WriteMessage("Neural netork successfully created", Utilities::OK);
    Utilities::WriteBlankLine();
}

void RNNPBWrapper::LoadWeights()
{
    StaticDataSource context(10, 4);
    neuralnetwork->SetInput(0, 0, &context, 3, 0);
    //neuralnetwork->SetInput(0, 1, &context, 4, 0 );
    neuralnetwork->ImportWeights(Path);
    passNr =-1;
}

void RNNPBWrapper::TrainFromSource()
{
    std::vector<int> number_of_lines(4);
    std::string line;

    std::vector<string> files_in(4);
    std::vector<string> files_out(4);



    for(int i = 0; i<4; i++ )
    {
       number_of_lines.push_back(0);
       files_in[i] = (Path + Utilities::GetMoreFilenames(Utilities::NNfiles.anglesInFile.c_str(), i+1).c_str());
       files_out[i] = (Path + Utilities::GetMoreFilenames(Utilities::NNfiles.anglesOutFile.c_str(), i+1).c_str());
       Utilities::WriteMessage(files_in[i], Utilities::Info);
       ifstream file(files_in[i].c_str());
        while (std::getline(file, line))
            ++number_of_lines[i];
    }

    FileDataSource ds_in_one(number_of_lines[0], 4, files_in[0].c_str(), 2);
    FileDataSource ds_in_two(number_of_lines[1], 4, files_in[1].c_str(), 2);
    FileDataSource ds_in_three(number_of_lines[2], 4, files_in[2].c_str(), 2);
    FileDataSource ds_in_four(number_of_lines[3], 4, files_in[3].c_str(), 2);

    FileDataSource ds_out_one (number_of_lines[0], 4,  files_out[0].c_str(), 2);
    FileDataSource ds_out_two (number_of_lines[1], 4,  files_out [1].c_str(), 2);
    FileDataSource ds_out_three (number_of_lines[2], 4, files_out[2].c_str(), 2);
    FileDataSource ds_out_four (number_of_lines[3], 4,  files_out[3].c_str(), 2);

     neuralnetwork->SetInput(0, 0, &ds_in_one, -1, 0);
     neuralnetwork->SetOutput(0, 3, &ds_out_one, false);
     neuralnetwork->SetInput(1, 0, &ds_in_two, -1, 0);
     neuralnetwork->SetOutput(1, 3, &ds_out_two, false);
     neuralnetwork->SetInput(2, 0, &ds_in_three, -1, 0);
     neuralnetwork->SetOutput(2, 3, &ds_out_three, false);
     neuralnetwork->SetInput(3, 0, &ds_in_four, -1, 0);
     neuralnetwork->SetOutput(3, 3, &ds_out_four, false);

    neuralnetwork->ExportWeights(false, Path);
    neuralnetwork->Train(number_of_lines);
    neuralnetwork->ExportWeights(true, Path);

    /*for(int  i = 1; i< 50000; ++i)
    {
        if(i%100 == 0) std::cout << i << endl;
        this->LoadWeights();
        neuralnetwork->Train();
        neuralnetwork->ExportWeights(true, Path);
    }*/
}

