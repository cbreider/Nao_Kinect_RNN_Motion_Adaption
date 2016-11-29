
/** \mainpage
 * <p>fnnlib (Flexible Neural Network Library) is an artificial neural networks library that lets you create recurrent neural networks (RNN) with traditional, gradient-based learning methods as well as Echo State Networks (ESN). fnnlib requires Armadillo (http://arma.sourceforge.net/), LAPACK (http://www.netlib.org/lapack/) and ATLAS (http://math-atlas.sourceforge.net/). This page provides a brief overview of how to use the library.</p>
 * \section rnn Creating, training and testing a recurrent neural network
 *
 * \subsection rnn_layers Creating the layers
 * <p>Before a neural network can be created, we first need to create an array containing pointers to layer objects.</p>
 * <pre>
 * Layer** layers = new Layer*[3];
 * </pre>
 * <p>The network will consist of one input layer, one hidden layer, and one output layer, so we allocate a pointer array of size 3. Next, we'll create the Layer objects representing the network's layers and store pointers to them in the array.</p>
 * <pre>
 * layers[0] = new Layer (1);
 * layers[1] = new Layer (8, ActivationFunction::TANH);
 * layers[2] = new Layer (1);
 * </pre>
 * <p>The first parameter to the Layer constructor specifies the number of units in the layer. The second parameter specifies the activation function to be used; if it is NULL, identity activation will be used. Constant, static pointers to objects representing some common activation functions are stored in the ActivationFunction class (SIGMOID = sigmoid activation, TANH = hyperbolic tangent activation, SOFTMAX = softmax activation, IDENTITY = identity activation). For information about the parameters that were omitted here, see the documentation for Layer.</p>
 *
 * \subsection rnn_training Creating the training algorithm object
 * <p>An object representing the training algorithm has to be passed to the constructor of the network. (In the case of an ESN, the object has to be passed to the constructor of the layer class ESNLayer.) Let's assume we want to use standard backpropagation through time (BPTT).</p>
 * <pre>
 * BPTT bptt_alg (10000, 10, 0.1, 0.0);
 * </pre>
 * <p>This will create a BPTT algorithm object that will train the network it is assigned to by running it 10000 times for epochs of 10 steps, calculating its gradients and adjusting the connection weights after each 10-step epoch using a learning rate of 0.1. For more information, see the documentation for BPTT.</p>
 *
 * \subsection rnn_net Creating the network
 * <pre>
 * RNN* net = new RNN (3, layers, &bptt_alg);
 * </pre>
 * <p>This line creates a recurrent neural network consisting of three layers, pointers to which are stored in layers. bptt_alg will be used to train the network.</p>
 *
 * \subsection rnn_connect Connecting the layers
 * <pre>
 * RandomInitialization rand_init (-0.1, 0.1);
 * net->ConnectLayerToLayer (0, 1, &rand_init, true);
 * net->ConnectLayerToLayer (1, 1, &rand_init, true);
 * net->ConnectLayerToLayer (1, 2, &rand_init, true);
 * </pre>
 * <p>Objects of any child class of Initialization can be used to initialize connection weights between two layers. In this case, random values x with -0.1 < x < 0.1 were used. The layers of our network are now fully connected (layer 0 to layer 1, 1 to 2), with all connections being marked trainable (as specified by the Boolean 'true' passed to RNN::ConnectLayerToLayer as its 4th argument). Note that every unit in layer 1 is recurrently connected to all units in the same layer (including itself). See the documentation for RNN for information about other ways to connect layers or units and to initialize the connections.</p>
 *
 * \subsection rnn_io Specifying input/desired-output data
 * <p>In order to provide a network with input data or desired-output data, an object of a child class of DataSource meeds to be created. In most cases, this will be an object of type FileDataSource, which reads data from a file:</p>
 * <pre>
 * FileDataSource ds_in (100, 1, "sin_input.txt");
 * FileDataSource ds_out (100, 1, "sin_output.txt");
 * </pre>
 * <p>This will read 100 data sets (i.e. sets of double values) of size 1 from the files "sin_input.txt" and "sin_output.txt", storing them in the objects ds_in and ds_out, respectively. The double values in the file have to be separated by whitespaces. For information on how to access the data manually, see the documentation for DataSource.</p>
 * <p>Now we'll tell our network to use ds_in as input for the first layer, and ds_out as desired output for the 3rd layer:</p>
 * <pre>
 * net->SetInput (0, ds_in);
 * net->SetOutput (2, ds_out, false);
 * </pre>
 * <p>The third argument to SetOutput tells the network not to use teacher forcing during training; see the documentation for RNN for more information about the parameters.</p>
 *
 * \subsection rnn_train Training the network
 * <p>Training the network is as easy as calling RNN::Train:</p>
 * <pre>
 * net->Train ();
 * </pre>
 *
 * \subsection rnn_test Testing the trained network's performance
 * <pre>
 * NetworkErrorData* ed = net->Test (2, 100, 1);
 * cout << "MSE: " << ed->GetLayerData (2)->GetMse () << endl;
 * </pre>
 * <p>The call to RNN::Test will run the network twice (as specified by the 1st argument) for 100 steps (2nd argument) each time, but it will only start collecting error data after the first 100-step epoch, as specified by argument 3. The error data will be returned in a NetworkErrorData object. Error data for individual layers can be accessed using NetworkErrorData::GetLayerData, which takes a zero-based layer index as its sole argument and returns a pointer to an ErrorData object. Note that only layers for which desired-output data was specified (layer 2 in our case) will have an ErrorData object associated with them. ErrorData::GetMse computes the network's mean squared error based on the desired output values found in layer 2's FileDataSource object and the actual output values that were computed during the test run.</p>
 *
 * \section esn Creating, training and testing an ESN
 * <p>ESN's can be created, trained and tested in a similar way as RNNs using gradient-based training methods, but there are some differences: The layers of an ESN are of type ESNLayer; different training methods may be specified for different layers in the same network; also, an Initialization object may be passed to ESNLayer's constructor in order to create recurrent connections between all units of this layer. These connections will not be trainable, though. First, we'll create the layer objects and store them in an array:</p>
 *
 * \subsection esn_layers Creating the layers
 * <pre>
 * PermutationInitialization res_init (-0.1, 0.1); // create initialization object to be used for the reservoir
 * ESNLayer** layers = new ESNLayer*[3];
 * layers[0] = new ESNLayer (1); // input layer
 * layers[1] = new ESNLayer (100, ActivationFunction::TANH, NULL, &res_init); // reservoir
 * layers[2] = new ESNLayer (1, NULL, ESNTrainingAlgorithm::PSEUDOINV); // output layer
 * </pre>
 * <p>Our reservoir will use the hyperbolic tangent activation function. Since we don't intend to train its incoming connections, we pass NULL as the training method to be used; if we did, however, call NeuralNetwork::SetOutput specifying this layer's index and a source of desired-output values, it would be trained using Armadillo's solve() method for solving systems of linear equations. The last argument tells ESNLayer to let the PermutationInitialization object res_init (see PermutationInitialization for more information) initialize the reservoir's internal connections.</p>
 * <p>The output layer will not use a non-linear activation function (as indicated by NULL); its incoming connections will be trained by the pseudoinverse algorithm, as indicated by ESNTrainingAlgorithm::PSEUDOINV.</p>
 *
 * \subsection esn_net Creating the ESN
 * <pre>
 * ESN* net = new ESN (3, layers);
 * </pre>
 * <p>This creates an ESN consisting of three layers, the pointers to which are stored in layers.</p>
 *
 * \subsection esn_connect Connecting the layers
 * <p>We'll connect layer 0 to layer 1, and layer 1 to layer 2.</p>
 * <pre>
 * RandomInitialization rand_init (-0.1, 0.1);
 * net->ConnectLayerToLayer (0, 1, &rand_init, false);
 * net->ConnectLayerToLayer (1, 2, &rand_init, true);
 * </pre>
 * <p>All connections will be initialized by the RandomInitialization object rand_init. The connections from 0 to 1 will not be trainable (4th argument = false), whereas the connections from our reservoir (layer 1) to the output layer (2) WILL be trainable.</p>
 *
 * \subsection esn_io Specifying input/desired-output data
 * <p>This works the same way it did for RNNs using gradient-based training methods (see above):</p>
 * <pre>
 * FileDataSource ds_in (100, 1, "sin_input.txt");
 * FileDataSource ds_out (100, 1, "sin_output.txt");
 * net->SetInput (0, ds_in);
 * net->SetOutput (2, ds_out, false);
 * </pre>
 *
 * \subsection esn_train Training the network
 * <pre>
 * net->Train (100, 100);
 * </pre>
 * <p>This will run the ESN for 100 steps (2nd argument), but no reservoir states will be collected yet. Afterwards, the ESN will be run for another 100 steps (1st argument) during which the reservoir states will be collected into a matrix that will be used for training.</p>
 *
 * \subsection esn_test Testing the network
 * <p>We want to collect 100 sets of error data (1st argument in the following line), but only after the network was run for 100 steps (2nd argument) during which no error data was collected.</p>
 * <pre>
 * NetworkErrorData* ed = net->Test (100, 100);
 * cout << "MSE: " << ed->GetLayerData (2)->GetMse () << endl;
 * </pre>
 * <p>After collecting the error data, we print the ESN's mean squared error.</p>
 */

#ifndef NNLIB_HPP_INCLUDED
#define NNLIB_HPP_INCLUDED

#define ARMA_USE_LAPACK

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <ctime>
#include <armadillo>

using namespace std;
using namespace arma;

void nnlib_assert (bool, char* = NULL);

//#define NNLIB_NO_ERROR_CHECKING

class Adaptation;

class ActivationFunction;
class SigmoidActivation;
class TanhActivation;
class SoftmaxActivation;
class IdentityActivation;
class UserActivation;

class DataSource;
class StaticDataSource;
class FileDataSource;

class LayerErrorData;
class NetworkErrorData;

class Initialization;
class RandomInitialization;
class RandomSparseInitialization;
class PermutationInitialization;

class Layer;
class ESNLayer;

class NeuralNetwork;
class RNN;
class ESN;

class RNNTrainingAlgorithm;
class BPTT;
class CTRNN;

class ESNTrainingAlgorithm;
class WienerHopfAlgorithm;
class PseudoInverseAlgorithm;
class LinearLeastSquaresAlgorithm;

#include "rnnlib/include/Adaptation.hpp"
#include "rnnlib/include/ActivationFunction.hpp"
#include "rnnlib/include/DataSource.hpp"
#include "rnnlib/include/LayerErrorData.hpp"
#include "rnnlib/include/ESNTrainingAlgorithm.hpp"
#include "rnnlib/include/Initialization.hpp"
#include "rnnlib/include/Layer.hpp"
#include "rnnlib/include/ESNLayer.hpp"
#include "rnnlib/include/NeuralNetwork.hpp"
#include "rnnlib/include/RNN.hpp"
#include "rnnlib/include/ESN.hpp"
#include "rnnlib/include/RNNTrainingAlgorithm.hpp"
#include "rnnlib/include/BPTT.hpp"
#include "rnnlib/include/CBPTT.hpp"
#include "rnnlib/include/NetworkErrorData.hpp"

#include "rnnlib/src/Adaptation.cpp"
#include "rnnlib/src/ActivationFunction.cpp"
#include "rnnlib/src/DataSource.cpp"
#include "rnnlib/src/LayerErrorData.cpp"
#include "rnnlib/src/ESNTrainingAlgorithm.cpp"
#include "rnnlib/src/Initialization.cpp"
#include "rnnlib/src/Layer.cpp"
#include "rnnlib/src/ESNLayer.cpp"
#include "rnnlib/src/NeuralNetwork.cpp"
#include "rnnlib/src/ESN.cpp"
#include "rnnlib/src/RNN.cpp"
#include "rnnlib/src/RNNTrainingAlgorithm.cpp"
#include "rnnlib/src/BPTT.cpp"
#include "rnnlib/src/CBPTT.cpp"
#include "rnnlib/src/NetworkErrorData.cpp"

inline void nnlib_assert (bool cond, char* err_str)
{
    if (!cond)
    {
        if (err_str)
            cout << err_str << endl << flush;

        exit (1);
    }
}

#endif // NNLIB_HPP_INCLUDED
