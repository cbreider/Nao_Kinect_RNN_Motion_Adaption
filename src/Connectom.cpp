
#include <iostream>
#include "Utilities.hpp"
#include "../rnn/rnn.hpp"

#include "../include/Connectom.hpp"

using namespace std;

int RecurrentNeuralNetwork::InitRNNPB()
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
}

int RecurrentNeuralNetwork::InitCTRNNForRealTime(int passes)
{

    Utilities::WriteMessage("Settng ip Neural network...", Utilities::NewProcedure);
    //Creating Layers
      layers = new Layer*[6];
      // input Layer
      layers[0] = new Layer(4);  // input prev arm angles
       layers[1] = new Layer(3);  // input object position

      //layers[1] = new Layer(2, ActivationFunction::IDENTITY, true); // parametric bias

      layers[2] = new Layer(4); //context loop x

      //hidden Layer
      layers[3] = new Layer(12, ActivationFunction::SIGMOID);

      //output Layer
      layers[4] = new Layer(4); // output - 4DOF
      layers[5] = new Layer(4); //context loop x+1

      //Set Trainalgorithm
       cbpt_alg = new BPTT(passes, 60, 0.01, 0.0);

     //Init network
      neuralnetwork = new RNN(6, layers, cbpt_alg);

      RandomInitialization rand_init(-1, 1);

      //Build synapsis (connect neurons/layers)
        //1. hidden layer
      neuralnetwork->ConnectLayerToLayer(0, 3, &rand_init, true); // connect input to hidden layer
      neuralnetwork->ConnectLayerToLayer(1, 3, &rand_init, true); //connect PB-Layer to hidden layer
      neuralnetwork->ConnectLayerToLayer(2, 3, &rand_init, true); //connect PB-Layer to hidden layer
      //neuralnetwork->ConnectLayerToLayer(2, 3, &rand_init, true); // connect context loop x to hidden layer

      neuralnetwork->ConnectLayerToLayer(3, 4, &rand_init, true); // connect hidden layer to output layer
      neuralnetwork->ConnectLayerToLayer(3, 5, &rand_init, true); // connect hidden layer to context loop x+1

      //neuralnetwork->ConnectUnitToUnit(5, 0, 2, 0, 1, false); //connect context loop x+1 (1) to context loop x (1)
      //neuralnetwork->ConnectUnitToUnit(5, 1, 2, 1, 1, false); //connect context loop x+1 (2) to context loop x (2)

       Utilities::WriteMessage("Neural netork successfully created", Utilities::OK);
       Utilities::WriteBlankLine();
      return 0;
}


int RecurrentNeuralNetwork::StartNewTrainCyclus(vector<float> object, vector<float> data)
{
        FileDataSource ds_in (3, object);
        FileDataSource ds_out (4, data);
        neuralnetwork->SetInput(0, 0, &ds_in, 4, 0);
        neuralnetwork->SetInput(0, 1, &ds_in, -1, 0);

        neuralnetwork->SetOutput(4, &ds_out, false);
        neuralnetwork->Train();
        return 1;
}

void RecurrentNeuralNetwork::LoadWeights(string file)
{
    StaticDataSource context(10, 4);
    StaticDataSource context2(10, 3);
    neuralnetwork->SetInput(0, 0, &context, 4, 0);
    neuralnetwork->SetInput(0, 1, &context2, -1, 0);
    neuralnetwork->SetInput(0, 2, &context, 5, 0 );
    neuralnetwork->ImportWeights(file);
    passNr =-1;
}

int RecurrentNeuralNetwork::StartTrainingFromSource(string angleIn, string object, string angleOut, int passes)
{
        int number_of_lines = 0;
        std::string line;
        std::ifstream myfile(angleIn.c_str());

        while (std::getline(myfile, line))
            ++number_of_lines;

        FileDataSource ds_in (number_of_lines, 3, object.c_str());
        FileDataSource ds_inA (number_of_lines, 4, angleIn.c_str());
        FileDataSource ds_out (number_of_lines, 4, angleOut.c_str());
        StaticDataSource context(number_of_lines, 4);
        neuralnetwork->SetInput(0, 0, &ds_inA, -1, 0);
        neuralnetwork->SetInput(0, 1, &ds_in, -1, 0);
        neuralnetwork->SetInput(0, 2, &context, 5, 0 );
        neuralnetwork->SetOutput(4, &ds_out, false);
        neuralnetwork->Train();
         return 1;
}

vector<float> RecurrentNeuralNetwork::PredictNextStep(std::vector<float> object, std::vector<float> angles)
{
    passNr++;
    return neuralnetwork->RunOneTime(0, object, passNr, angles);
}

int RecurrentNeuralNetwork::Reset()
{
      return 1;
}
