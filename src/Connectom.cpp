#include "../include/Connectom.hpp"
#include "../fnnlib.hpp"
#include <iostream>
#include "UserInterface.hpp"

using namespace std;

int Connectom::InitRNNPB()
{

    UserMessage::WriteMessage("Settng ip Neural network...", UserMessage::NewProcedure);
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

       UserMessage::WriteMessage("Neural netork successfully created", UserMessage::OK);
       UserMessage::WriteBlankLine();
      return 0;
}

int Connectom::InitCTRNN()
{

    UserMessage::WriteMessage("Settng ip Neural network...", UserMessage::NewProcedure);
    //Creating Layers
      layers = new Layer*[5];
      // input Layer
      layers[0] = new Layer(7);  // input

      //layers[1] = new Layer(2, ActivationFunction::IDENTITY, true); // parametric bias

      layers[1] = new Layer(4); //context loop x

      //hidden Layer
      layers[2] = new Layer(12, ActivationFunction::SIGMOID);

      //output Layer
      layers[3] = new Layer(4); // output - 4DOF
      layers[4] = new Layer(4); //context loop x+1

      //Set Trainalgorithm
       cbpt_alg = new BPTT(1000, 10, 0.1, 0.0);

     //Init network
      neuralnetwork = new RNN(6, layers, cbpt_alg);

      RandomInitialization rand_init(-0.1, 0.1);

      //Build synapsis (connect neurons/layers)
        //1. hidden layer
      neuralnetwork->ConnectLayerToLayer(0, 3, &rand_init, true); // connect input to hidden layer
      neuralnetwork->ConnectLayerToLayer(1, 3, &rand_init, true); //connect PB-Layer to hidden layer
      //neuralnetwork->ConnectLayerToLayer(2, 3, &rand_init, true); // connect context loop x to hidden layer

      neuralnetwork->ConnectLayerToLayer(2, 4, &rand_init, true); // connect hidden layer to output layer
      neuralnetwork->ConnectLayerToLayer(3, 5, &rand_init, true); // connect hidden layer to context loop x+1

      neuralnetwork->ConnectUnitToUnit(5, 0, 2, 0, 1, false); //connect context loop x+1 (1) to context loop x (1)
      neuralnetwork->ConnectUnitToUnit(5, 1, 2, 1, 1, false); //connect context loop x+1 (2) to context loop x (2)

       UserMessage::WriteMessage("Neural netork successfully created", UserMessage::OK);
       UserMessage::WriteBlankLine();
      return 0;
}
int Connectom::StartNewTrainCyclus()
{
      FileDataSource ds_in (700, 3, "output/Sample_object_1.txt");
      FileDataSource ds_out (700, 4, "output/Sample_angle_1.txt");
      neuralnetwork->SetInput(0, 0, &ds_in, -1, 0);
      neuralnetwork->SetOutput(4, &ds_out, false);
      neuralnetwork->Train();
      NetworkErrorData* ed = neuralnetwork->Test (100, 2);
      cout << "MSE: " << ed->GetLayerData (4)->GetMse () << endl;
      return 1;
}

int Connectom::Reset()
{
      return 1;
}
