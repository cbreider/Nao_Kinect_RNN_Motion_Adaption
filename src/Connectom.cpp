
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
    //passNr++;
    //neuralnetwork->RunAfterTraining(0, 50, 0, false, true, true, angles);
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

void RNNCLWrapper::Test()
{
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
    layers[0] = new Layer(4);  // input amgles
    layers[1] = new Layer(3); // input object position
    layers[2] = new Layer(Utilities::Parameters.contextUnitCount, ActivationFunction::IDENTITY, false, false, Utilities::Parameters.fTau, NULL); //context loop x
    layers[3] = new Layer(Utilities::Parameters.hiddenUnitsCount, ActivationFunction::SIGMOID);
    layers[4] = new Layer(Utilities::Parameters.contextUnitCount); //context loop x+1
    //output Layer
    layers[5] = new Layer(4); // output - 4DOF

    //Set Trainalgorithm

    cbpt_alg = new BPTT(Utilities::Parameters.passes, Utilities::Parameters.epoch, Utilities::Parameters.lRate, 0.0);

    //Init network
    neuralnetwork = new RNN(6, layers, cbpt_alg, 1);

    RandomInitialization rand_init(Utilities::Parameters.minInit, Utilities::Parameters.maxInit);

    //Build synapsis (connect neurons/layers)
    //1. hidden layer
    neuralnetwork->ConnectLayerToLayer(0, 3, &rand_init, true); // connect input to hidden layer
    neuralnetwork->ConnectLayerToLayer(1, 3, &rand_init, true); //connect PB-Layer to hidden layer
    neuralnetwork->ConnectLayerToLayer(2, 3 , &rand_init, true); // connect hidden layer to output layer
    for(int i = 0; i< Utilities::Parameters.contextUnitCount; i++)
    {
        neuralnetwork->ConnectUnitToUnit(4, i,  2, i, Initialization::GenerateRandomWeight(Utilities::Parameters.minInit, Utilities::Parameters.maxInit), true);
    }
    neuralnetwork->ConnectLayerToLayer(3, 4, &rand_init, true);
    neuralnetwork->ConnectLayerToLayer(3, 5, &rand_init, true);
    Utilities::WriteMessage("Neural netork successfully created", Utilities::OK);
    Utilities::WriteBlankLine();
}

void RNNWrapper::LoadWeights()
{
    StaticDataSource context(10, 4);
    StaticDataSource context2(10, 3);
    neuralnetwork->SetInput(0, 0, &context, 5, 0);
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
    neuralnetwork->SetOutput(0, 5, &ds_out, false);
    neuralnetwork->ExportWeights(false, Path);
    neuralnetwork->Train(number_of_lines);
    neuralnetwork->ExportWeights(true, Path);
}

void RNNWrapper::Test()
{
    int number_of_lines;
    std::string line;

    string files_in;
    string files_out;
    string file_ob;
    std::ifstream iopb;
    iopb.open((Path + Utilities::NNfiles.PBfile).c_str());
    std::vector<vector<float> > initialPose(100, vector<float>(4));
    std::vector<vector<float> > initialPoseout(100, vector<float>(4));
    std::vector<vector<float> > ob(100, vector<float>(3));

        std::ifstream io;
        std::ifstream io_o;
        std::ifstream io_ob;
        io.open((Path + Utilities::NNfiles.anglesInFile).c_str());
        io_o.open((Path + Utilities::NNfiles.anglesOutFile).c_str());
        io_ob.open((Path + Utilities::NNfiles.objectFile).c_str());

        vector<float> pb(1);
       files_in = (Path + Utilities::NNfiles.anglesInFile).c_str();
       files_out = (Path + Utilities::NNfiles.anglesOutFile).c_str();

       Utilities::WriteMessage(files_in, Utilities::Info);
       ifstream file(files_in.c_str());
        ifstream fileout(files_out.c_str());
       int a =0;
        while (std::getline(file, line))
        {
            if(a < 100)
            {

                 io >> initialPose[a][0];
                 io >> initialPose[a][1];
                 io >> initialPose[a][2];
                 io >> initialPose[a][3];

                 io_o >> initialPoseout[a][0];
                 io_o >> initialPoseout[a][1];
                 io_o >> initialPoseout[a][2];
                 io_o >> initialPoseout[a][3];

                 io_ob >> ob[a][0];
                 io_ob >> ob[a][1];
                 io_ob >> ob[a][2];
                 io_ob >> ob[a][3];
            }
            ++number_of_lines;
            a++;
        }

        neuralnetwork->RunAfterTraining(0, number_of_lines, 0, true, true, initialPose, initialPoseout,pb, ob);

}

RNNPBWrapper::RNNPBWrapper(std::string path) : RecurrentNeuralNetworkWrapper(NNType::RNNPB, path)
{
        pb_count = 2;
        nr_seq = 4;
}

void RNNPBWrapper::Init()
{
    Utilities::WriteMessage("Setting ip Neural network...", Utilities::NewProcedure);

    //Creating Layers
    layers = new Layer*[6];
    // input Layer

    layers[0] = new Layer(4);  // input prev arm angles
    layers[1] = new Layer(pb_count, ActivationFunction::SIGMOID, true); //pblayer
    layers[2] = new Layer(Utilities::Parameters.contextUnitCount, ActivationFunction::IDENTITY, false, false, Utilities::Parameters.fTau, NULL); //context loop x
    layers[3] = new Layer(Utilities::Parameters.hiddenUnitsCount, ActivationFunction::SIGMOID);      //hidden Layer
    layers[4] = new Layer(Utilities::Parameters.contextUnitCount); //context loop x+1
    layers[5] = new Layer(4); // output - 4DOF


    //Set Trainalgorithm
    cbpt_alg = new BPTT(Utilities::Parameters.passes, Utilities::Parameters.epoch, Utilities::Parameters.lRate, Utilities::Parameters.pbLRate);

    //Init network
    neuralnetwork = new RNN(6, layers, cbpt_alg, nr_seq);

    RandomInitialization rand_init(Utilities::Parameters.minInit, Utilities::Parameters.maxInit);

    //Build synapsis (connect neurons/layers)
    //1. hidden layer
    neuralnetwork->ConnectLayerToLayer(0, 3, &rand_init, true);
    for(int i = 0; i< pb_count; i++)
    {
        for(int ii = 0; ii< Utilities::Parameters.hiddenUnitsCount; ii++)
        {
            neuralnetwork->ConnectUnitToUnit(1, i,  3, ii, Initialization::GenerateRandomWeight(Utilities::Parameters.minInit, Utilities::Parameters.maxInit), true);
        }
    }
    neuralnetwork->ConnectLayerToLayer(2, 3, &rand_init, true);
    neuralnetwork->ConnectLayerToLayer(3, 4, &rand_init, true);
    neuralnetwork->ConnectLayerToLayer(3, 5, &rand_init, true);
    for(int i = 0; i< Utilities::Parameters.contextUnitCount; i++)
    {
        neuralnetwork->ConnectUnitToUnit(4, i,  2, i, Initialization::GenerateRandomWeight(Utilities::Parameters.minInit, Utilities::Parameters.maxInit), true);
    }
    Utilities::WriteMessage("Neural netork successfully created", Utilities::OK);
    Utilities::WriteBlankLine();
}

void RNNPBWrapper::LoadWeights()
{
    StaticDataSource context(10, 4);
    for(int i = 0; i < nr_seq; i++)
    {
        neuralnetwork->SetInput(i, 0, &context, 5, 0);

    }
    neuralnetwork->ImportWeights(Path);
    passNr =-1;
}

void RNNPBWrapper::TrainFromSource()
{
    std::vector<int> number_of_lines(nr_seq);
    std::string line;

    std::vector<string> files_in(nr_seq);
    std::vector<string> files_out(nr_seq);

    std::vector<FileDataSource*> ds_in;
    std::vector<FileDataSource*> ds_out;

    for(int i = 0; i<nr_seq; i++ )
    {
       number_of_lines.push_back(0);
       files_in[i] = (Path + Utilities::GetMoreFilenames(Utilities::NNfiles.anglesInFile.c_str(), i+1).c_str());
       files_out[i] = (Path + Utilities::GetMoreFilenames(Utilities::NNfiles.anglesOutFile.c_str(), i+1).c_str());
       Utilities::WriteMessage(files_in[i], Utilities::Info);
       ifstream file(files_in[i].c_str());
        while (std::getline(file, line))
            ++number_of_lines[i];
    }
    for(int i = 0; i<nr_seq; i++ )
    {
        ds_in.push_back( new FileDataSource(number_of_lines[i], 4, files_in[i].c_str(), 2));
        ds_out.push_back(new FileDataSource(number_of_lines[i], 4, files_out[i].c_str(), 2));
        neuralnetwork->SetInput(i, 0, ds_in[i], -1);
        neuralnetwork->SetOutput(i, 5, ds_out[i], false);
    }
    neuralnetwork->ExportWeights(false, Path);
    neuralnetwork->Train(number_of_lines);
    neuralnetwork->ExportWeightsAndPBs(true, Path, nr_seq);
}

void RNNPBWrapper::Test()
{
    std::vector<int> number_of_lines(nr_seq);
    std::string line;

    std::vector<string> files_in(nr_seq);
    std::vector<string> files_out(nr_seq);

    std::vector<vector<float> > pb(nr_seq, vector<float>(pb_count));
    std::ifstream iopb;
    iopb.open((Path + Utilities::NNfiles.PBfile).c_str());
    std::vector<vector<float> > initialPose(50, vector<float>(4));
    std::vector<vector<float> > initialPoseout(50, vector<float>(4));
    std::vector<vector<float> > ob(50, vector<float>(3));
    for(int i = 0; i<nr_seq; i++ )
    {
        std::ifstream io;
        std::ifstream io_o;
        io.open((Path + Utilities::GetMoreFilenames(Utilities::NNfiles.anglesInFile.c_str(), i+1)).c_str());
        io_o.open((Path + Utilities::GetMoreFilenames(Utilities::NNfiles.anglesOutFile.c_str(), i+1)).c_str());

        for(int ii = 0; ii < pb_count; ii++)
        {
            iopb >> pb[i][ii];
            std::cout << pb[i][ii] << " ";

        }
       number_of_lines.push_back(0);
       files_in[i] = (Path + Utilities::GetMoreFilenames(Utilities::NNfiles.anglesInFile.c_str(), i+1).c_str());
       files_out[i] = (Path + Utilities::GetMoreFilenames(Utilities::NNfiles.anglesOutFile.c_str(), i+1).c_str());
       Utilities::WriteMessage(files_in[i], Utilities::Info);
       ifstream file(files_in[i].c_str());
        ifstream fileout(files_out[i].c_str());
       int a =0;
        while (std::getline(file, line))
        {
            if(a < 50)
            {

                 io >> initialPose[a][0];
                 io >> initialPose[a][1];
                 io >> initialPose[a][2];
                 io >> initialPose[a][3];

                 io_o >> initialPoseout[a][0];
                 io_o >> initialPoseout[a][1];
                 io_o >> initialPoseout[a][2];
                 io_o >> initialPoseout[a][3];


            }
            ++number_of_lines[i];
            a++;
        }

        neuralnetwork->RunAfterTraining(i, number_of_lines[i], 0, true, true, initialPose, initialPoseout,pb[i], ob);
    }
}

