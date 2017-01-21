

#ifndef RNN_HPP_INCLUDED
#define RNN_HPP_INCLUDED

#define ARMA_USE_LAPACK

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <ctime>
#include <armadillo>

using namespace std;
using namespace arma;



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

#include "include/Adaptation.hpp"
#include "include/ActivationFunction.hpp"
#include "include/DataSource.hpp"
#include "include/LayerErrorData.hpp"
#include "include/ESNTrainingAlgorithm.hpp"
#include "include/Initialization.hpp"
#include "include/Layer.hpp"
#include "include/ESNLayer.hpp"
#include "include/NeuralNetwork.hpp"
#include "include/RNN.hpp"
#include "include/ESN.hpp"
#include "include/RNNTrainingAlgorithm.hpp"
#include "include/BPTT.hpp"
#include "include/CBPTT.hpp"
#include "include/NetworkErrorData.hpp"

#include "src/Adaptation.cpp"
#include "src/ActivationFunction.cpp"
#include "src/DataSource.cpp"
#include "src/LayerErrorData.cpp"
#include "src/ESNTrainingAlgorithm.cpp"
#include "src/Initialization.cpp"
#include "src/Layer.cpp"
#include "src/ESNLayer.cpp"
#include "src/NeuralNetwork.cpp"
#include "src/ESN.cpp"
#include "src/RNN.cpp"
#include "src/RNNTrainingAlgorithm.cpp"
#include "src/BPTT.cpp"
#include "src/CBPTT.cpp"
#include "src/NetworkErrorData.cpp"


#endif // RNN_HPP_INCLUDED
