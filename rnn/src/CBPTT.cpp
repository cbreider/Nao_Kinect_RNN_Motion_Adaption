
/**
  * Constructor; See RNNTrainingAlgorithm (this class's parent class) for information about the parameters.
  */
CBPTT::CBPTT (int nPasses, int nEpochSize, double fLearningRate, double fPbLearningRate) : RNNTrainingAlgorithm (nPasses, nEpochSize, fLearningRate, fPbLearningRate)
{

}

/**
  * Computes the gradients of a given RNN for one training epoch. If multiple patterns are to be learned, the method needs to be invoked or each pattern individually.
  *
  * @param net The RNN for which the gradients will be computed
  * @param nSeq The number of the pattern for which the gradients will be computed
  * @param nEpochSize Number of time-steps in the current training epoch
  */
void CBPTT::ComputeGradients (RNN* net, int nSeq, int nEpochSize)
{
    int src_layer, dst_layer, src_unit, dst_unit;
    vec vec_u, e, temp;

    // Iterate over all time-steps and layers
    for (int t = nEpochSize - 1; t >= 0; t--)
        for (src_layer = net->num_layers - 1; src_layer >= 0; src_layer--)
        {
            // Gradients are computed differently for output and non-output layers
            if (net->data_out[nSeq][src_layer])
            {
                g[nSeq][src_layer][t] = trans (net->s[nSeq][src_layer].row (t) - net->d[nSeq][src_layer].row (t));

                if (t < nEpochSize - 1)
                {
                    temp.ones (net->layers[src_layer]->GetSize ());
                    g[nSeq][src_layer][t] = (temp - (temp / net->layers[src_layer]->GetTimeConstant ())) % g[nSeq][src_layer][t+1];
                }
            }
            else
            {
                g[nSeq][src_layer][t].zeros ();

                for (dst_layer = 0; dst_layer < net->num_layers; dst_layer++)
                    if (net->w[dst_layer][src_layer])
                        for (src_unit = 0; src_unit < net->layers[src_layer]->GetSize (); src_unit++)
                            for (dst_unit = 0; dst_unit < net->layers[dst_layer]->GetSize (); dst_unit++)
                                if (net->synapses[dst_layer][dst_unit][src_layer][src_unit])
                                    g[nSeq][src_layer][t] (src_unit) += (t < nEpochSize-1 ? g[nSeq][dst_layer][t+1] (dst_unit) : 0.0) * (dst_layer == src_layer && dst_unit == src_unit ? 1.0 - 1.0 / net->layers[src_layer]->GetTimeConstant () : 0.0) + 1.0 / net->layers[dst_layer]->GetTimeConstant () * (*net->w[dst_layer][src_layer]) (dst_unit, src_unit);

                vec_u = net->u[nSeq][src_layer].col (t);
                net->layers[src_layer]->ComputeDerivatives (&vec_u, &y_d[src_layer]);
                g[nSeq][src_layer][t] = g[nSeq][src_layer][t] % y_d[src_layer];
            }
        }
}

/**
  * Updates the weights of a given RNN based on the gradients computed by ComputeGradients().
  *
  * @param net The RNN whose weights will be updated
  */
void CBPTT::UpdateWeights (RNN* net)
{
    int src_layer, dst_layer, src_unit, dst_unit, seq, t;
    double sum;

    // Iterate over all synapses and adjust the trainable weights
    for (src_layer = 0; src_layer < net->num_layers; src_layer++)
        for (dst_layer = 0; dst_layer < net->num_layers; dst_layer++)
            if (net->w[dst_layer][src_layer])
                for (src_unit = 0; src_unit < net->layers[src_layer]->GetSize (); src_unit++)
                    for (dst_unit = 0; dst_unit < net->layers[dst_layer]->GetSize (); dst_unit++)
                        if (net->trainable[dst_layer][dst_unit][src_layer][src_unit])
                        {
                            sum = 0.0;

                            for (seq = 0; seq < net->num_seq; seq++)
                                for (t = 1; t < epoch_size-1; t++)
                                    sum += g[seq][dst_layer][t] (dst_unit) * net->s[seq][src_layer] (src_layer < dst_layer ? t : t-1, src_unit) / net->layers[dst_layer]->GetTimeConstant ();

                            (*net->w[dst_layer][src_layer]) (dst_unit, src_unit) -= lr * sum / net->num_seq;
                        }
}

/**
  * Updates the PB values of a given RNN based on a learning rate parameter. Needs to be invoked individually for each pattern after each training epoch.
  *
  * @param The RNN whose PB values will be adjusted
  * @param nSeq The number of the pattern for which the PB values will be adjusted
  * @param nEpochSize The number of time-steps in the current training epoch
  * @param fPbLearningRate The learning rate to be used in the update equation
  */
void CBPTT::UpdatePbs (RNN* net, int nSeq, int nEpochSize, double fPbLearningRate)
{
    int t, unit;
    double sum;

    // Sum up the gradients for each PB layer and adjust the internal states
    for (int l = 0; l < net->num_layers; l++)
    	if (net->layers[l]->IsPbLayer () && !net->layers[l]->IsConst ())
            for (unit = 0; unit < net->layers[l]->GetSize (); unit++)
            {
                sum = 0.0;

                for (t = 0; t < nEpochSize - 1; t++)
                    sum += g[nSeq][l][t] (unit);

                net->u_def[nSeq][l] (unit) -= fPbLearningRate * sum / net->layers[l]->GetTimeConstant ();
            }
}
