#include <vector>
#include <cmath>

BPTT::BPTT (int nPasses, int nEpochSize, double fLearningRate, double fPbLearningRate) : RNNTrainingAlgorithm (nPasses, nEpochSize, fLearningRate, fPbLearningRate)
{

}


void BPTT::ComputeGradients (RNN* net, int nSeq, int nEpochSize)
{
    vec vec_u, sum, e, e_tmp;

    // Iterate over all time-steps and layers
    for (int t = nEpochSize - 1; t >= 0; t--)
        for (int src_layer = net->num_layers - 1; src_layer >= 0; src_layer--)
        {
            vec_u = net->u[nSeq][src_layer].col (t);
            net->layers[src_layer]->ComputeDerivatives (&vec_u, &y_d[src_layer]);   // Compute the derivatives of the activation function

            // Compute the error; if the layer is not an output layer, set the error to zero
            if (net->data_out[nSeq][src_layer])
            {
                //if(nSeq== 0)
                  // std::cout << net->d[nSeq][src_layer].row (t)<< endl;

                e= trans (net->d[nSeq][src_layer].row (t) - net->s[nSeq][src_layer].row (t));
                sum_e[nSeq] += std::abs(e[0])  + std::abs(e[1])   + std::abs(e[2])+  std::abs(e[3]);
            }
            else
                e.zeros (net->layers[src_layer]->GetSize ());

            sum.zeros (net->layers[src_layer]->GetSize ());

            // Sum up all the products w*g, where g denotes the gradient of a neuron connectd to the current neuron
            // by a connection with weight w
            for (int dst_layer = 0; dst_layer < net->num_layers; dst_layer++)
                if (net->w[dst_layer][src_layer])
                    for (int src_unit = 0; src_unit < net->layers[src_layer]->GetSize (); src_unit++)
                    	for (int dst_unit = 0; dst_unit < net->layers[dst_layer]->GetSize (); dst_unit++)
                    		if (net->synapses[dst_layer][dst_unit][src_layer][src_unit])
                                sum (src_unit) += (*net->w[dst_layer][src_layer]) (dst_unit, src_unit) * (src_layer < dst_layer ? g[nSeq][dst_layer][t] (dst_unit) : (t < nEpochSize - 1 ? g[nSeq][dst_layer][t+1] (dst_unit) : 0.0));

            g[nSeq][src_layer][t] = y_d[src_layer] % (e + sum); // Compute the gradients for the current layer; '%' means element-wise multiplication
        }
}


void BPTT::UpdateWeights (RNN* net)
{
    int src_layer, dst_layer, src_unit, dst_unit, seq, t;
    double sum;
    double tmp_lr;
    // Iterate over all synapses and adjust the trainable weights
    for (src_layer = 0; src_layer < net->num_layers; src_layer++)
        for (dst_layer = 0; dst_layer < net->num_layers; dst_layer++)
            if (net->w[dst_layer][src_layer]) // skip the following steps if the two layers aren't connected to begin with; this should make the program faster in almost all cases
                for (src_unit = 0; src_unit < net->layers[src_layer]->GetSize (); src_unit++)
                    for (dst_unit = 0; dst_unit < net->layers[dst_layer]->GetSize (); dst_unit++)
                        if (net->trainable[dst_layer][dst_unit][src_layer][src_unit])
                        {
                            sum = 0.0;

                            // Iterate over all sequences and time-steps, summing up the products g*y_d, where g is the gradient of the destination neuron
                            // of a synapse and y_d is the derivative of the activation function of the source neuron
                            for (seq = 0; seq < net->num_seq; seq++)
                                for (t = 1; t < epoch_size-1; t++)
                                    sum += g[seq][dst_layer][t] (dst_unit) * net->s[seq][src_layer] (src_layer < dst_layer ? t : t-1, src_unit);
                            if(net->data_in[0][src_layer])
                             {
                                tmp_lr = lr;
                            }
                            else
                            {
                                tmp_lr =lr;
                            }
                            (*net->w[dst_layer][src_layer]) (dst_unit, src_unit) += tmp_lr * sum / net->num_seq; // adjust the weights using the learning rate lr
                        }


}

void BPTT::UpdatePbs (RNN* net, int nSeq, int nEpochSize, double fPbLearningRate)
{
    int t, unit;
    double sum;

    // Sum up the gradients for each PB layer and update the internal states
    for (int l = 0; l < net->num_layers; l++)
    	if (net->layers[l]->IsPbLayer () && !net->layers[l]->IsConst ())
            for (unit = 0; unit < net->layers[l]->GetSize (); unit++)
            {
                sum = 0.0;

                for (t = 0; t < nEpochSize - 1; t++)
                    sum += g[nSeq][l][t] (unit);

              net->u_def[nSeq][l] (unit) = fPbLearningRate * sum + net->u_def[nSeq][l] (unit) * (1);



            }
}
