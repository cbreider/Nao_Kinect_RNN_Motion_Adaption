
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "Utilities.hpp";

/**
  * Creates a new neural network consisting of nLayers layers stored in layerArray; the network will be able to learn nSequences trajectories.
  *
  * @param nLayers The number of layers in the network
  * @param layerArray An array containing pointers to the Layer objects which will be used as the network's layers
  * @param nSequences The number of a trajectories the network will be able to learn
  */
NeuralNetwork::NeuralNetwork (int nLayers, Layer** layerArray, int nSequences)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nLayers > 0, (char*) "ERROR: nLayers must be at least 1 in NeuralNetwork::NeuralNetwork!");
    Utilities::Assert(nSequences > 0, (char*) "ERROR: nSequences must be at least 1 in NeuralNetwork::NeuralNetwork!");
    #endif

    num_layers = nLayers;
    num_seq = nSequences;
    layers = new Layer*[num_layers];
    w = new mat**[num_layers];
    temp = new vec[num_layers];
    copy_output = new int[num_layers];
    copy_output_after = new int[num_layers];

    data_in = new DataSource**[num_seq];
    data_out = new DataSource**[num_seq];
    data_in_index = new int*[num_seq];
    data_out_index = new int*[num_seq];
    use_tf = new bool*[num_seq];
    u = new mat*[num_seq];
    s = new mat*[num_seq];
    d = new mat*[num_seq];
    prv_u = new vec*[num_seq];
    cur_u = new vec*[num_seq];
    u_def = new vec*[num_seq];
    y = new vec*[num_seq];

    for (int seq = 0; seq < num_seq; seq++)
    {
    	data_in[seq] = new DataSource*[num_layers];
        data_out[seq] = new DataSource*[num_layers];
        data_in_index[seq] = new int[num_layers];
        data_out_index[seq] = new int[num_layers];
        use_tf[seq] = new bool[num_layers];
        u[seq] = new mat[num_layers];
        s[seq] = new mat[num_layers];
        d[seq] = new mat[num_layers];
        prv_u[seq] = new vec[num_layers];
        cur_u[seq] = new vec[num_layers];
        u_def[seq] = new vec[num_layers];
        y[seq] = new vec[num_layers];

        for (int l = 0; l < num_layers; l++)
        {
            data_in[seq][l] = NULL;
            data_out[seq][l] = NULL;
            use_tf[seq][l] = false;
            data_in_index[seq][l] = 0;
            data_out_index[seq][l] = 0;
        }
    }

    for (int l = 0; l < num_layers; l++)
    {
    	layers[l] = layerArray[l];

    	w[l] = new mat*[num_layers];
    	memset (w[l], 0, num_layers * sizeof (mat*));

    	copy_output[l] = -1;
    }
}

/**
  * Destructor; frees allocated memory.
  */
NeuralNetwork::~NeuralNetwork ()
{
    for (int seq = 0; seq < num_seq; seq++)
    {
    	delete [] u[seq];
        delete [] s[seq];
        delete [] prv_u[seq];
        delete [] cur_u[seq];
        delete [] d[seq];
        delete [] use_tf[seq];
        delete [] data_in[seq];
        delete [] data_out[seq];
        delete [] data_in_index[seq];
        delete [] data_out_index[seq];
    }

    for (int i = 0; i < num_layers; i++)
    {
        for (int j = 0; j < num_layers; j++)
        	delete [] w[i][j];

        delete [] w[i];
    }

    delete [] u;
    delete [] s;
    delete [] temp;
    delete [] prv_u;
    delete [] cur_u;
    delete [] d;
    delete [] layers;
    delete [] w;
    delete [] use_tf;
    delete [] data_in;
    delete [] data_out;
    delete [] data_in_index;
    delete [] data_out_index;
}

/**
  * Designates a layer as input layer. The input may be read from a DataSource object and/or another layer.
  *
  * @param nSeq Index of the trajectory whose data is to be fed to the layer
  * @param nLayer The layer's zero-based index
  * @param dataSource The DataSource object from which the input is to be read
  * @param nSrcLayer [optional] The index of the layer whose output is to be used as input
  * @param nStartCopyingAfter [optional] Number of steps after which nSrcLayer's output rather than dataSource's data is to be used as input
  */
void NeuralNetwork::SetInput (int nSeq, int nLayer, DataSource* dataSource, int nSrcLayer, int nStartCopyingAfter)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSeq >= 0 && nSeq < num_seq, (char*) "ERROR: nSeq out of range in NeuralNetwork::SetInput!");
    Utilities::Assert(nLayer >= 0 && nLayer < num_layers, (char*) "ERROR: nLayer out of range in NeuralNetwork::SetInput!");
    int x = layers[nLayer]->GetSize ();
    if(nSrcLayer < 0) Utilities::Assert(layers[nLayer]->GetSize () == dataSource->GetSetSize (), (char*) "ERROR: Layer size must be the same as size of DataSource data sets in NeuralNetwork::SetInput!");
    Utilities::Assert(nSrcLayer >= -1 && nSrcLayer < num_layers, (char*) "ERROR: nSrcLayer out of range in NeuralNetwork::SetInput!");
    Utilities::Assert(nSrcLayer != nLayer, (char*) "ERROR: Cannot copy layer's output to itself! [NeuralNetwork::SetInput]");
    if (nSrcLayer > -1)
        Utilities::Assert(layers[nSrcLayer]->GetSize () == layers[nLayer]->GetSize (), (char*) "ERROR: Size of receiving layer must be the same as size of source layer! [NeuralNetwork::SetInput]");
    Utilities::Assert(nStartCopyingAfter >= 0, (char*) "ERROR: nStartCopyingAfter out of range in NeuralNetwork::SetInput!");
    #endif

    data_in[nSeq][nLayer] = dataSource;
    copy_output[nLayer] = nSrcLayer;
    copy_output_after[nLayer] = nStartCopyingAfter;
}

/**
  * Calls SetInput (int nSeq, int nLayer, DataSource* dataSource, int nSrcLayer, int nStartCopyingAfter) with nSeq=0. Calling this method is sufficient if the network is supposed to learn only one trajectory. See SetInput (int nSeq, int nLayer, DataSource* dataSource, int nSrcLayer, int nStartCopyingAfter) for information about the parameters.
  */
void NeuralNetwork::SetInput (int nLayer, DataSource* dataSource, int nSrcLayer, int nStartCopyingAfter)
{
    SetInput (0, nLayer, dataSource, nSrcLayer, nStartCopyingAfter);
}

/**
  * Designates a layer as output layer. The desired-output data will be read from a DataSource object. Teacher forcing may be used during the training phase.
  *
  * @param nSeq Index of the trajectory whose desired output data is to be read
  * @param nLayer The layer's zero-based index
  * @param dataSource The DataSource object from which the desired output is to be read
  * @param bUseTeacherForcing [optional] Specifies whether the desired output data from dataSource should be copied to the layer's output during the teaching phase (true) or not (false)
  */
void NeuralNetwork::SetOutput (int nSeq, int nLayer, DataSource* dataSource, bool bUseTeacherForcing)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSeq >= 0 && nSeq < num_seq, (char*) "ERROR: nSeq out of range in NeuralNetwork::SetInput!");
    Utilities::Assert(nLayer >= 0 && nLayer < num_layers, (char*) "ERROR: nLayer out of range in NeuralNetwork::SetOutput!");
    Utilities::Assert(layers[nLayer]->GetSize () == dataSource->GetSetSize (), (char*) "ERROR: Layer size must be the same as size of DataSource data sets in NeuralNetwork::SetOutput!");
    #endif

    data_out[nSeq][nLayer] = dataSource;
    use_tf[nSeq][nLayer] = bUseTeacherForcing;
}

/**
  * Calls SetOutput (int nSeq, int nLayer, DataSource* dataSource, bool bUseTeacherForcing) with nSeq set to zero. Calling this method is sufficient if the network is supposed to learn only one trajectory. See SetOutput (int nSeq, int nLayer, DataSource* dataSource, bool bUseTeacherForcing) for information about the parameters.
  */
void NeuralNetwork::SetOutput (int nLayer, DataSource* dataSource, bool bUseTeacherForcing)
{
    SetOutput (0, nLayer, dataSource, bUseTeacherForcing);
}

/**
  * Sets the default state of a single unit; this new default state will be used when the network is tested on the trajectory specified by nSeq.
  *
  * @param nSeq The trajectory during the learning/testing of which the default value will be used
  * @param nLayer The unit's layer's index
  * @param nUnit The unit's index inside the layer specified by nLayer
  * @param fValue The unit's new default value
  */
void NeuralNetwork::SetDefState (int nSeq, int nLayer, int nUnit, double fValue)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSeq >= 0 && nSeq < num_seq, (char*) "ERROR: nSeq out of range in NeuralNetwork::SetDefState!");
    Utilities::Assert(nLayer >= 0 && nLayer < num_layers, (char*) "ERROR: nLayer out of range in NeuralNetwork::SetDefState!");
    Utilities::Assert(nUnit >= 0 && nUnit < layers[nLayer]->GetSize (), (char*) "ERROR: nUnit out of range in NeuralNetwork::SetDefState!");
    #endif

    u_def[nSeq][nLayer] (nUnit) = fValue;
}

/**
  * Sets the default states of a layer's units; these new default states will be used when the network is tested on the trajectory specified by nSeq.
  *
  * @param nSeq The trajectory during the testing of which the default values will be used
  * @param nLayer The layer's index
  * @param fValue The layer's unit's new default values
  */
void NeuralNetwork::SetDefStates (int nSeq, int nLayer, vec* fValues)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSeq >= 0 && nSeq < num_seq, (char*) "ERROR: nSeq out of range in NeuralNetwork::SetDefStates!");
    Utilities::Assert(nLayer >= 0 && nLayer < num_layers, (char*) "ERROR: nLayer out of range in NeuralNetwork::SetDefStates!");
    Utilities::Assert((int) fValues->n_rows == layers[nLayer]->GetSize (), (char*) "ERROR: fValues has wrong dimension in NeuralNetwork::SetDefStates!");
    #endif

    u_def[nSeq][nLayer] = *fValues;
}

/**
  * Runs the network for a nPasses steps using the input/output data of the trajectory specified by nSeq. Data relevant for training will only be collected after nSkip steps.
  *
  * @param nSeq The trajectory whose input/output is to be used
  * @param nPasses The number of steps during which to collect data relevant for training
  * @param nSkip The number of steps to skip before data relevant for training is collected
  * @param bIsTest Specifies whether the network is being tested (true) or not (false)
  * @param bReset Specifies whether the network's internal state should be reset
  * @param bResetPbs Specifies whether the network's Parametric Bias units should be reset as well
  */
void NeuralNetwork::Run (int nSeq, int nPasses, int nSkip, bool bIsTest, bool bReset, bool bResetPbs)
{
    int dst_layer, src_layer;

    if (bReset)
        Reset (nSeq, nPasses, bResetPbs);

    for (int t = 0; t < nSkip+nPasses; t++)
    {
    	for (dst_layer = 0; dst_layer < num_layers; dst_layer++)
    	{
    	    // cur_u denotes the induced local fields of the current layer at the current time-step
    		cur_u[nSeq][dst_layer].zeros ();

            if (data_out[nSeq][dst_layer] && use_tf[nSeq][dst_layer]) // If this is an outout layer for which teacher forcing is to be used, write the desired outputs into the layer
                data_out[nSeq][dst_layer]->GetSetAt (data_out_index[nSeq][dst_layer]++, &y[nSeq][dst_layer]);
            else
            {
                if (data_in[nSeq][dst_layer]) // If this is an input layer...
                {
                    // If the outout of another layer is to be used as input, copy the output into the induced local fields
                    // Otherwise, read data from the specified data source

                    if (copy_output[dst_layer] > -1  && bIsTest && data_in_index[nSeq][dst_layer] >= copy_output_after[dst_layer])
                        cur_u[nSeq][dst_layer] = y[nSeq][copy_output[dst_layer]];
                    else
                        data_in[nSeq][dst_layer]->GetSetAt (data_in_index[nSeq][dst_layer]++, &cur_u[nSeq][dst_layer]);
                }

                // If this is an output layer, write the desired output into matrix d after the initial network states were washed out
                if (data_out[nSeq][dst_layer])
                {
                    data_out[nSeq][dst_layer]->GetSetAt (data_out_index[nSeq][dst_layer]++, &temp[dst_layer]);

                    if (t >= nSkip)
                        d[nSeq][dst_layer].row (t - nSkip) = trans (temp[dst_layer]);
                }

                // If this is a PB layer, use the PB units' internal states as the induced local fields
                if (layers[dst_layer]->IsPbLayer ())
                    cur_u[nSeq][dst_layer] = u_def[nSeq][dst_layer];

                // Multiply the weight matrices by the outputs to compute the inouts applied to the neurons
                for (src_layer = 0; src_layer < num_layers; src_layer++)
                    if (w[dst_layer][src_layer])
                    {
                        temp[dst_layer] = (*w[dst_layer][src_layer]) * y[nSeq][src_layer];
                        cur_u[nSeq][dst_layer] += temp[dst_layer];  // doesn't work if the right-hand side of the above assignment is added to cur_u directly
                    }

                // If a time constant greather than 1 is used, 5the previous induced local fields will affect the current output
                cur_u[nSeq][dst_layer] /= layers[dst_layer]->GetTimeConstant ();
                cur_u[nSeq][dst_layer] += prv_u[nSeq][dst_layer] * (1.0 - 1.0 / layers[dst_layer]->GetTimeConstant ());

                prv_u[nSeq][dst_layer] = cur_u[nSeq][dst_layer];
                layers[dst_layer]->ComputeActivations (&cur_u[nSeq][dst_layer], &y[nSeq][dst_layer]);
            }

            // Collect the activations and induced local fields after the initial washout
            if (t >= nSkip)
            {
                s[nSeq][dst_layer].row (t - nSkip) = trans (y[nSeq][dst_layer]);
                u[nSeq][dst_layer].col (t - nSkip) = cur_u[nSeq][dst_layer];
            }
    	}
    }
}

void NeuralNetwork::RunWithRecurrentConnections (int nSeq, int nPasses, int nSkip, bool bReset, bool bResetPbs, bool bFirstpass)
{
    int dst_layer, src_layer;

    if (bReset)
        Reset (nSeq, nPasses, bResetPbs);

    for (int t = 0; t < nSkip+nPasses; t++)
    {
        for (dst_layer = 0; dst_layer < num_layers; dst_layer++)
        {
            // cur_u denotes the induced local fields of the current layer at the current time-step
            cur_u[nSeq][dst_layer].zeros ();

            if (data_out[nSeq][dst_layer] && use_tf[nSeq][dst_layer]) // If this is an outout layer for which teacher forcing is to be used, write the desired outputs into the layer
                data_out[nSeq][dst_layer]->GetSetAt (data_out_index[nSeq][dst_layer]++, &y[nSeq][dst_layer]);
            else
            {
                if (data_in[nSeq][dst_layer]) // If this is an input layer...
                {
                    // If the outout of another layer is to be used as input, copy the output into the induced local fields
                    // Otherwise, read data from the specified data source
                    int foo = copy_output[dst_layer];
                    int foo2 =  data_in_index[nSeq][dst_layer];
                    int foo3 = copy_output_after[dst_layer];
                    if (copy_output[dst_layer] > -1  && data_in_index[nSeq][dst_layer] >= copy_output_after[dst_layer])
                        if(!bFirstpass)
                        {
                            cur_u[nSeq][dst_layer] = y[nSeq][copy_output[dst_layer]];
                        }
                        else
                        {
                            cur_u[nSeq][dst_layer][0] = 1;
                            cur_u[nSeq][dst_layer][1] = 1;
                            cur_u[nSeq][dst_layer][2] = 1;
                            cur_u[nSeq][dst_layer][3] = 1;
                        }
                    else
                        data_in[nSeq][dst_layer]->GetSetAt (data_in_index[nSeq][dst_layer]++, &cur_u[nSeq][dst_layer]);
                }

                // If this is an output layer, write the desired output into matrix d after the initial network states were washed out
                if (data_out[nSeq][dst_layer])
                {
                    data_out[nSeq][dst_layer]->GetSetAt (data_out_index[nSeq][dst_layer]++, &temp[dst_layer]);

                    if (t >= nSkip)
                        d[nSeq][dst_layer].row (t - nSkip) = trans (temp[dst_layer]);
                }

                // If this is a PB layer, use the PB units' internal states as the induced local fields
                if (layers[dst_layer]->IsPbLayer ())
                    cur_u[nSeq][dst_layer] = u_def[nSeq][dst_layer];

                // Multiply the weight matrices by the outputs to compute the inouts applied to the neurons
                for (src_layer = 0; src_layer < num_layers; src_layer++)
                    if (w[dst_layer][src_layer])
                    {
                        temp[dst_layer] = (*w[dst_layer][src_layer]) * y[nSeq][src_layer];
                        cur_u[nSeq][dst_layer] += temp[dst_layer];  // doesn't work if the right-hand side of the above assignment is added to cur_u directly
                    }

                // If a time constant greather than 1 is used, the previous induced local fields will affect the current output
                cur_u[nSeq][dst_layer] /= layers[dst_layer]->GetTimeConstant ();
                cur_u[nSeq][dst_layer] += prv_u[nSeq][dst_layer] * (1.0 - 1.0 / layers[dst_layer]->GetTimeConstant ());

                prv_u[nSeq][dst_layer] = cur_u[nSeq][dst_layer];
                layers[dst_layer]->ComputeActivations (&cur_u[nSeq][dst_layer], &y[nSeq][dst_layer]);
            }

            // Collect the activations and induced local fields after the initial washout
            if (t >= nSkip)
            {
                //if( dst_layer == 4) std::cout << y[nSeq][dst_layer] << endl;


                s[nSeq][dst_layer].row (t - nSkip) = trans (y[nSeq][dst_layer]);
                u[nSeq][dst_layer].col (t - nSkip) = cur_u[nSeq][dst_layer];
            }
        }
    }
}

/**
  * Runs the network for a nPasses steps using the input/output data of the trajectory specified by nSeq. Data relevant for training will only be collected after nSkip steps.
  *
  * @param nSeq The trajectory whose input/output is to be used
  * @param nPasses The number of steps during which to collect data relevant for training
  * @param nSkip The number of steps to skip before data relevant for training is collected
  * @param bIsTest Specifies whether the network is being tested (true) or not (false)
  * @param bReset Specifies whether the network's internal state should be reset
  * @param bResetPbs Specifies whether the network's Parametric Bias units should be reset as well
  */
std::vector<float> NeuralNetwork::RunOneTime (int nSeq, std::vector<float> object, int passNr, vector<float> firstangles)
{
    if(passNr == 0) Reset(0, 1, false );

    int dst_layer, src_layer;

    std::vector<float> ret(4);
        for (dst_layer = 0; dst_layer < num_layers; dst_layer++)
        {
            // cur_u denotes the induced local fields of the current layer at the current time-step
            cur_u[nSeq][dst_layer].zeros ();


                if (data_in[nSeq][dst_layer]) // If this is an input layer...
                {
                    // If the outout of another layer is to be used as input, copy the output into the induced local fields
                    // Otherwise, read data from the specified data source
                    if (copy_output[dst_layer] > -1 && data_in_index[nSeq][dst_layer] >= copy_output_after[dst_layer])
                    {
                        if(passNr > 0)
                        {
                            cur_u[nSeq][dst_layer] = y[nSeq][copy_output[dst_layer]];
                        }
                        else
                        {
                               if(dst_layer == 0)
                               {
                                    cur_u[nSeq][dst_layer][0] = firstangles[0];
                                    cur_u[nSeq][dst_layer][1] = firstangles[1];
                                    cur_u[nSeq][dst_layer][2] = firstangles[2];
                                    cur_u[nSeq][dst_layer][3] = firstangles[3];
                               }
                               else
                               {
                                   cur_u[nSeq][dst_layer][0] = 1;
                                   cur_u[nSeq][dst_layer][1] = 1;
                                   cur_u[nSeq][dst_layer][2] = 1;
                                   cur_u[nSeq][dst_layer][3] = 1;
                               }
                        }
                    }
                    else
                    {
                        cur_u[nSeq][dst_layer][0] = object[0];
                        cur_u[nSeq][dst_layer][1] = object[1];
                        cur_u[nSeq][dst_layer][2] = object[2];
                        //std::cout <<  object[0] << "   " << object[1] << "   " <<  object[2] <<endl;
                    }
                }

                // If this is a PB layer, use the PB units' internal states as the induced local fields
                if (layers[dst_layer]->IsPbLayer ())
                    cur_u[nSeq][dst_layer] = u_def[nSeq][dst_layer];

                // Multiply the weight matrices by the outputs to compute the inouts applied to the neurons
                for (src_layer = 0; src_layer < num_layers; src_layer++)
                    if (w[dst_layer][src_layer])
                    {
                        temp[dst_layer] = (*w[dst_layer][src_layer]) * y[nSeq][src_layer];
                        cur_u[nSeq][dst_layer] += temp[dst_layer];  // doesn't work if the right-hand side of the above assignment is added to cur_u directly
                    }

                // If a time constant greather than 1 is used, the previous induced local fields will affect the current output
                cur_u[nSeq][dst_layer] /= layers[dst_layer]->GetTimeConstant ();
                cur_u[nSeq][dst_layer] += prv_u[nSeq][dst_layer] * (1.0 - 1.0 / layers[dst_layer]->GetTimeConstant ());

                prv_u[nSeq][dst_layer] = cur_u[nSeq][dst_layer];
                layers[dst_layer]->ComputeActivations (&cur_u[nSeq][dst_layer], &y[nSeq][dst_layer]);


            // Collect the activations and induced local fields after the initial washout

                s[nSeq][dst_layer].row (0 )= trans (y[nSeq][dst_layer]);
                u[nSeq][dst_layer].col (0 ) = cur_u[nSeq][dst_layer];
                if(dst_layer == 4)
                {
                    ret[0] = y[nSeq][dst_layer][0];
                    ret[1] = y[nSeq][dst_layer][1];
                    ret[2] = y[nSeq][dst_layer][2];
                    ret[3] = y[nSeq][dst_layer][3];

                   // std::cout <<  y[nSeq][dst_layer] <<endl;
                    /*ret[0] = s[nSeq][dst_layer].row (0 )[0];
                    ret[1] = s[nSeq][dst_layer].row (0 )[1];
                    ret[2] = s[nSeq][dst_layer].row (0 )[2];
                    ret[3] = s[nSeq][dst_layer].row (0 )[3];*/
                }

        }
        return ret;
}


/**
  * Resets the network, resizing memory structures and assigning default values.
  *
  * @param nSeq The trajectory whose memory structures are to be reset
  * @param nPasses The number of passes for which data relevant for training will be collected
  * @param bResetPbs Specifies whether the network's Parametric Bias units should be reset
  */
void NeuralNetwork::Reset (int nSeq, int nPasses, bool bResetPbs)
{
    for (int l = 0; l < num_layers; l++)
    {
        u[nSeq][l].zeros (layers[l]->GetSize (), nPasses);
        s[nSeq][l].zeros (nPasses, layers[l]->GetSize ());
        temp[l].zeros (layers[l]->GetSize ());
        prv_u[nSeq][l].zeros (layers[l]->GetSize ());
        cur_u[nSeq][l].zeros (layers[l]->GetSize ());
        y[nSeq][l].zeros (layers[l]->GetSize ());
        layers[l]->GetDefaultStates (&prv_u[nSeq][l]);

        if (bResetPbs || u_def[nSeq][l].n_rows == 0)
            layers[l]->GetDefaultStates (&u_def[nSeq][l]);

        if (data_out[nSeq][l])
            d[nSeq][l].zeros (nPasses, layers[l]->GetSize ());
    }
}

/**
  * Resets the pointers used to access the data in the DataSource objects assigned to the layers.
  */
void NeuralNetwork::ResetDataSources ()
{
    for (int seq = 0; seq < num_seq; seq++)
        for (int l = 0; l < num_layers; l++)
        {
            data_in_index[seq][l] = 0;
            data_out_index[seq][l] = 0;
        }
}

/**
  * Retrieves error data collected during a test on trajectory nSequence (if a test has been performed).
  *
  * @param nSeq The trajectory whose error data is to be retrieved
  * @param ed [out] The NetworkErrorData object into which te error data will be written
  */
void NeuralNetwork::CollectErrorData (int nSeq, NetworkErrorData* ed)
{
    vec vec_d, vec_y;

    for (int l = 0; l < num_layers; l++)
        if (data_out[nSeq][l])
            for (int t = 0; t < (int) s[nSeq][l].n_rows; t++)
            {
                vec_d = trans (d[nSeq][l].row (t));
                vec_y = trans (s[nSeq][l].row (t));
                ed->GetLayerData (l)->AddPair (&vec_d, &vec_y);
            }
}

/**
  * Connects one layer to another layer; the connections will be trainable.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nDstLayer The zero-based index of the destination layer
  */
void NeuralNetwork::ConnectLayerToLayer (int nSrcLayer, int nDstLayer)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(nSrcLayer >= 0 && nSrcLayer < num_layers, (char*) "ERROR: nSrcLayer out of range in NeuralNetwork::SetLayerToLayerWeights!");
    Utilities::Assert(nDstLayer >= 0 && nDstLayer < num_layers, (char*) "ERROR: nDstLayer out of range in NeuralNetwork::SetLayerToLayerWeights!");
    #endif

    if (!w[nDstLayer][nSrcLayer])
        w[nDstLayer][nSrcLayer] = new mat (layers[nDstLayer]->GetSize (), layers[nSrcLayer]->GetSize ());
}

/**
  * Connects one layer to another layer, initializing the connection weights and specifying whether they will be trainable or not.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nDstLayer The zero-based index of the destination layer
  * @param initFunc The initialization method to be used for initializing the connections
  * @param bTrainable Specifies whether the connections will be trainable (true) or not (false)
  */
void NeuralNetwork::ConnectLayerToLayer (int nSrcLayer, int nDstLayer, Initialization* initFunc, bool bTrainable)
{
    ConnectLayerToLayer (nSrcLayer, nDstLayer);

    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(initFunc != NULL, (char*) "ERROR: initFunc is NULL in NeuralNetwork::SetLayerToLayerWeights!");
    #endif

    initFunc->Initialize (w[nDstLayer][nSrcLayer]);
}

/**
  * Connects one layer (nSrcLayer) to another layer (nDstLayer), initializing the connection weights and specifying whether they will be trainable or not.
  *
  * @param nSrcLayer The zero-based index of the source layer
  * @param nDstLayer The zero-based index of the destination layer
  * @param weights A matrix containing the connection weights; entry (x, y) specifies the weight to be used for the connection from unit x in nSrcLayer to unit y in nDstLayer
  * @param bTrainable Specifies whether the connections will be trainable (true) or not (false)
  */
void NeuralNetwork::ConnectLayerToLayer (int nSrcLayer, int nDstLayer, mat* weights, bool bTrainable)
{
    ConnectLayerToLayer (nSrcLayer, nDstLayer);

    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(weights != NULL, (char*) "ERROR: weights is NULL in NeuralNetwork::SetLayerToLayerWeights!");
    #endif

    (*w[nDstLayer][nSrcLayer]) = trans (*weights);
}


void NeuralNetwork::ExportWeights(bool afterTraining, std::string path)
{
    std::string br = "\n";
    std::string filename= path;
    if(afterTraining) filename += Utilities::NNfiles.weightsFile;
    else filename += Utilities::NNfiles.initweightsFile;

    std::ofstream io;
    io.open(filename.c_str());

    int src_layer, dst_layer, src_unit, dst_unit;

    // Iterate over all synapses and adjust the trainable weights
    for (src_layer = 0; src_layer < num_layers; src_layer++)
    {
        io << br;
        for (dst_layer = 0; dst_layer < num_layers; dst_layer++)
            if (w[dst_layer][src_layer]) // skip the following steps if the two layers aren't connected to begin with; this should make the program faster in almost all cases
                for (src_unit = 0; src_unit < layers[src_layer]->GetSize (); src_unit++)
                    for (dst_unit = 0; dst_unit < layers[dst_layer]->GetSize (); dst_unit++)
                    {
                        std::stringstream ss;
                        if(afterTraining)
                        {
                            double x = (*this->w[dst_layer][src_layer]) (dst_unit, src_unit);
                            int xx =0;
                        }
                        ss << (*this->w[dst_layer][src_layer]) (dst_unit, src_unit);

                        io << ss.str();
                        io << " ";
                    }
    }
    io.close();

}

void NeuralNetwork::ImportWeights(std::string path)
{
    std::ifstream io;
    io.open((path + Utilities::NNfiles.weightsFile).c_str());

    int src_layer, dst_layer, src_unit, dst_unit;

    // Iterate over all synapses and adjust the trainable weights
    for (src_layer = 0; src_layer < num_layers; src_layer++)
    {
        for (dst_layer = 0; dst_layer < num_layers; dst_layer++)
            if (w[dst_layer][src_layer]) // skip the following steps if the two layers aren't connected to begin with; this should make the program faster in almost all cases
                for (src_unit = 0; src_unit < layers[src_layer]->GetSize (); src_unit++)
                    for (dst_unit = 0; dst_unit < layers[dst_layer]->GetSize (); dst_unit++)
                    {
                        double x;
                        io >> x;

                        (*this->w[dst_layer][src_layer]) (dst_unit, src_unit) = x;
                    }
    }
    io.close();
}
