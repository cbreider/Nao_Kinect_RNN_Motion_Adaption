#ifndef CTRNNALGORITHM_HPP_INCLUDED
#define CTRNNALGORITHM_HPP_INCLUDED

class CtrnnAlgorithm : TrainingAlgorithm
{
    public:
        CtrnnAlgorithm (NeuralNetwork*, int*, int, double (*) (double), double (*) (double));
        ~CtrnnAlgorithm ();

        void TrainNetwork (DataSource*, int, int, double, double*);
        ErrorData* TestNetwork (DataSource*, int, bool, int);

    private:
        int epoch_size;
        int cur_data_index;
        int size_input_bytes;
        int size_output_bytes;
        int* tau;

        double (*act_func) (double);
        double (*act_func_der) (double);
        double** u;
        double** y;
        double** y_d;
        double** g;
        double* d;

        void SetNewEpochSize (int, double*);
        void ForwardPass (DataSource*);
        void BackwardPass (DataSource*);
        void UpdateWeights (double);
};

CtrnnAlgorithm::CtrnnAlgorithm (NeuralNetwork* nnNet, int* nTau, int nActivationFunctionID, double (*ActivationFunction) (double) = NULL, double (*ActivationFunctionDerivative) (double) = NULL) : TrainingAlgorithm (nnNet)
{
    epoch_size = 0;
    size_input_bytes = sizeof (double) * nn->GetLayerSize (0);
    size_output_bytes = sizeof (double) * nn->GetLayerSize (nn->GetNumberOfLayers () - 1);

    tau = new int[nn->GetNumberOfLayers ()];
    memcpy (tau, nTau, nn->GetNumberOfLayers () * sizeof (int));

    switch (nActivationFunctionID)
    {
        case ActivationFunction::ACTIVATION_TANH:
            act_func = tanh;
            act_func_der = tanh_dash;
            break;

        case ActivationFunction::ACTIVATION_IDENTITY:
            act_func = id;
            act_func_der = id_dash;
            break;

        case ActivationFunction::ACTIVATION_USER:
            act_func = ActivationFunction;
            act_func_der = ActivationFunctionDerivative;
            break;

        case ActivationFunction::ACTIVATION_SIGMOID:
        default:
            act_func = sigmoid;
            act_func_der = sigmoid_dash;
    }
}

void CtrnnAlgorithm::SetNewEpochSize (int new_epoch_size, double* u_init = NULL)
{
    if (epoch_size > 0)
    {
        for (int i = 0; i < epoch_size; i++)
        {
            delete [] y[i];
            delete [] y_d[i];
            delete [] g[i];
            delete [] u[i];
        }

        delete [] y;
        delete [] y_d;
        delete [] g;
        delete [] u;
    }

    cur_data_index = 0;
    epoch_size = new_epoch_size;

    y = new double*[epoch_size];
    y_d = new double*[epoch_size];
    g = new double*[epoch_size];
    u = new double*[epoch_size];

    int num_units = nn->GetNumberOfUnits ();

    for (int i = 0; i < epoch_size; i++)
    {
        y[i] = new double[num_units];
        y_d[i] = new double[num_units];
        g[i] = new double[num_units];
        u[i] = new double[num_units];
        memset (y[i], 0, num_units * sizeof (double));
        memset (g[i], 0, num_units * sizeof (double));
        memset (u[0], 0, num_units * sizeof (double));
    }

    if (u_init)
        memcpy (u[0], u_init, num_units * sizeof (double));
}

CtrnnAlgorithm::~CtrnnAlgorithm ()
{
    for (int i = 0; i < epoch_size; i++)
    {
        delete [] y[i];
        delete [] y_d[i];
        delete [] g[i];
        delete [] u[i];
    }

    delete [] y;
    delete [] y_d;
    delete [] g;
    delete [] u;
    delete [] tau;
}

void CtrnnAlgorithm::ForwardPass (DataSource* ds)
{
    double ilf, tau_quot, sum;
    int t, src_unit, dst_unit;//, cur_unit;
    int t_last = epoch_size - 1;

    for (t = 0; t < epoch_size; t++)
    {
    	memcpy (y[t], ds->GetSetAt (cur_data_index, io_data_set), size_input_bytes);

        sum = 0.0;

        for (dst_unit = nn->GetLayerBase (1); dst_unit < nn->GetNumberOfUnits (); dst_unit++)
        {
            ilf = 0.0;

            for (src_unit = 0; src_unit < nn->GetNumberOfUnits (); src_unit++)
                if (nn->IsConnected (src_unit, dst_unit))
                {
                    #ifdef VERBOSE
                    cout << "ILF("<<dst_unit<<") += w["<<src_unit<<"]["<<dst_unit<<"] ("<<nn->GetWeight (src_unit, dst_unit)<<") * ";

                    if (nn->GetLayer (src_unit) < nn->GetLayer (dst_unit))
                    {
                        cout << "y["<<t<<"]["<<src_unit<<"] (" << y[t][src_unit] << ")";
                    }
                    else
                    {
                        if(t)
                            cout << "y["<<t-1<<"]["<<src_unit<<"] (" << y[t-1][src_unit] << ")";
                        else
                            cout << "y["<<t_last<<"]["<<src_unit<<"] (" << y[t_last][src_unit] << ")";
                    }
                    #endif

                    ilf += nn->GetWeight (src_unit, dst_unit) * (nn->GetLayer (src_unit) < nn->GetLayer (dst_unit) ? y[t][src_unit] : (t ? y[t-1][src_unit] : y[t_last][src_unit]));

                    #ifdef VERBOSE
                    cout << " = " << ilf << endl;
                    #endif
                }

            #ifdef VERBOSE
            cout << "tau_quot = 1.0 / tau[" << nn->GetLayer (dst_unit) << "] (" << tau[nn->GetLayer (dst_unit)] << ") = ";
            #endif

            tau_quot = 1.0 / tau[nn->GetLayer (dst_unit)];

            #ifdef VERBOSE
            cout << tau_quot << endl;

            cout << "u["<<t<<"]["<<dst_unit<<"] = (1.0 - tau_quot (" << tau_quot << ")) * ";
            if (cur_data_index == 0)
            {
                cout << "u["<<t<<"]["<<dst_unit<<"] (" << u[t][dst_unit] << ")";
            }
            else
            {
                if (t>0)
                    cout << "u["<<t-1<<"]["<<dst_unit<<"] (" << u[t-1][dst_unit] << ")";
                else
                    cout << "u["<<epoch_size-1<<"]["<<dst_unit<<"] ("<<u[epoch_size-1][dst_unit] << ")";
            }
            cout << " + tau_quot (" << tau_quot << ") * ILF("<<dst_unit<<") (" << ilf << ")";
            #endif

            u[t][dst_unit] = (1.0 - tau_quot) * (cur_data_index == 0 ? u[t][dst_unit] : (t > 0 ? u[t-1][dst_unit] : u[epoch_size-1][dst_unit])) + tau_quot * ilf;

            #ifdef VERBOSE
            cout << " = " << u[t][dst_unit] << endl;
            #endif

            if (nn->GetLayer (dst_unit) != -1)//nn->GetNumberOfLayers () - 1)
            {
                y[t][dst_unit] = act_func (u[t][dst_unit]);
                y_d[t][dst_unit] = act_func_der (u[t][dst_unit]);

                #ifdef VERBOSE
                cout << "y["<<t<<"]["<<dst_unit<<"] = " << y[t][dst_unit] << endl;
                cout << "y_d["<<t<<"]["<<dst_unit<<"] = " << y_d[t][dst_unit] << endl;
                #endif
            }
            else
                sum += exp (u[t][dst_unit]);
        }

        /*for (cur_unit = nn->GetLayerBase (nn->GetNumberOfLayers () - 1); cur_unit < nn->GetNumberOfUnits (); cur_unit++)
        {
            #ifdef VERBOSE
            cout << "y["<<t<<"]["<<cur_unit<<"] = exp (u["<<t<<"]["<<cur_unit<<"]="<<u[t][cur_unit]<<") / sum ("<<sum<<")";
            #endif

            y[t][cur_unit] = exp (u[t][cur_unit]) / sum;

            #ifdef VERBOSE
            cout << " = " << y[t][cur_unit] << endl;
            #endif
        }*/

        cur_data_index++;
    }
}

void CtrnnAlgorithm::BackwardPass (DataSource* ds)
{
    int t, src_unit, dst_unit;
    double sum;

    for (t = epoch_size-1; t > 0; t--)
    {
    	ds->GetSetAt (cur_data_index - (epoch_size - t), io_data_set);

        for (src_unit = nn->GetNumberOfUnits () - 1; src_unit > 0; src_unit--)
        {
            if (nn->GetLayer (src_unit) == nn->GetNumberOfLayers () - 1)
            {
                #ifdef VERBOSE
                cout << "g["<<t<<"]["<<src_unit<<"] = y["<<t<<"]["<<src_unit<<"] ("<<y[t][src_unit]<<") - d["<<t<<"]["<<nn->GetRelativeIndex (src_unit)<<"] ("<<temp_data_set[nn->GetLayerSize (0) + nn->GetRelativeIndex (src_unit)]<<") + (1 - 1 / tau["<<nn->GetLayer (src_unit)<<"] (" << tau[nn->GetLayer (src_unit)] << ")) * ";
                if (t < epoch_size-1)
                    cout << "g["<<t+1<<"]["<<src_unit<<"] ("<<g[t+1][src_unit]<<")";
                else
                    cout << "g_prev (0.0)";
                #endif

                g[t][src_unit] = y[t][src_unit] - io_data_set[nn->GetLayerSize (0) + nn->GetRelativeIndex (src_unit)] + (1 - 1 / tau[nn->GetLayer (src_unit)]) * (t < epoch_size-1 ? g[t+1][src_unit] : 0.0);

                #ifdef VERBOSE
                cout << " = " << g[t][src_unit] << endl;
                #endif
            }
            else
            {
                sum = (nn->IsConnected (src_unit, src_unit) ? 1 - 1 / tau[nn->GetLayer (src_unit)] : 0.0);

                #ifdef VERBOSE
                cout << "sum = ";
                if (nn->IsConnected (src_unit, src_unit))
                    cout << "1 - 1 / tau["<<nn->GetLayer (src_unit)<<"] ("<<tau[nn->GetLayer (src_unit)]<<") = " << sum << " (IsConnected ("<<src_unit<<", "<<src_unit<<")"<<endl;
                else
                    cout << "0.0" << endl;
                #endif

                for (dst_unit = nn->GetLayerBase (1); dst_unit < nn->GetNumberOfUnits (); dst_unit++)
                    if (nn->IsConnected (src_unit, dst_unit))
                    {
                        sum += nn->GetWeight (src_unit, dst_unit) * y_d[t][src_unit] / tau[nn->GetLayer (dst_unit)];

                        #ifdef VERBOSE
                        cout << "sum += w["<<src_unit<<"]["<<dst_unit<<"] ("<<nn->GetWeight (src_unit, dst_unit)<<") * y_d["<<t<<"]["<<src_unit<<"] ("<<y_d[t][src_unit]<<") / tau["<<nn->GetLayer (dst_unit)<<"] ("<<tau[nn->GetLayer (dst_unit)]<<") = " << sum << endl;
                        #endif
                    }

                g[t][src_unit] = (t < epoch_size-1 ? g[t+1][src_unit] : 0.0) * sum;

                #ifdef VERBOSE
                cout << "g["<<t<<"]["<<src_unit<<"] = ";
                if (t < epoch_size-1)
                    cout << "g["<<t+1<<"]["<<src_unit<<"] ("<<g[t+1][src_unit]<<")";
                else
                    cout << "g_prev ("<<0<<")";
                cout << " * sum (" << sum << ") = " << g[t][src_unit] << endl;
                #endif
            }
        }
    }
}

void CtrnnAlgorithm::UpdateWeights (double lr)
{
    int t, src_unit, dst_unit;
    double sum;

    for (src_unit = 0; src_unit < nn->GetNumberOfUnits (); src_unit++)
        for (dst_unit = nn->GetLayerBase (1); dst_unit < nn->GetNumberOfUnits (); dst_unit++)
            if (nn->IsConnected (src_unit, dst_unit))
            {
                sum = 0.0;

                #ifdef VERBOSE
                cout << "sum = 0.0" << endl;
                #endif

                for (t = 0; t < epoch_size; t++)
                {
                	sum += g[t][dst_unit] * (nn->GetLayer (src_unit) < nn->GetLayer (dst_unit) ? y[t][src_unit] : (t ? y[t-1][src_unit] : 0.0)) / tau[nn->GetLayer (dst_unit)];

                	#ifdef VERBOSE
                	cout << "sum += g["<<t<<"]["<<dst_unit<<"] ("<<g[t][dst_unit]<<") * ";
                	if (nn->GetLayer (src_unit) < nn->GetLayer (dst_unit))
                	{
                	    cout << "y["<<t<<"]["<<src_unit<<"] (" << y[t][src_unit] << ")";
                	}
                	else
                	{
                	    if(t)
                	    {
                	        cout << "y["<<t-1<<"]["<<src_unit<<"] (" << y[t-1][src_unit] << ")";
                	    }
                	    else
                	    {
                	        cout << "0.0";
                	    }
                	}
                	cout << " / tau["<<nn->GetLayer (dst_unit)<<"] (" <<  tau[nn->GetLayer (dst_unit)] << ") = " << sum << endl;
                	#endif
                }

                nn->SetWeight (src_unit, dst_unit, nn->GetWeight (src_unit, dst_unit) - lr * sum);

                #ifdef VERBOSE
                cout << "w["<<src_unit<<"]["<<dst_unit<<"] -= lr (" << lr << ") * sum (" << sum << ") = " << nn->GetWeight (src_unit, dst_unit) << endl;
                #endif
            }
}

void CtrnnAlgorithm::TrainNetwork (DataSource* dsDataSource, int nEpochSize, int nPasses, double fLearningRate, double* fInitialStates = NULL)
{
    SetNewEpochSize (nEpochSize, fInitialStates);

    for (int i = 0; i < nPasses; i++)
    {
        ForwardPass (dsDataSource);
        BackwardPass (dsDataSource);
        UpdateWeights (fLearningRate);
    }
}

ErrorData* CtrnnAlgorithm::TestNetwork (DataSource* dsDataSource, int nPasses, bool bCollectErrorData = true, int nErrorDataStartIndex = 0)
{
    int num_outputs = nn->GetLayerSize (nn->GetNumberOfLayers () - 1);
    int first_output_index = nn->GetLayerBase (nn->GetNumberOfLayers () - 1);
    int error_data_index = 0;

    SetNewEpochSize (dsDataSource->GetNumberOfSets ());

    ErrorData* ed = NULL;

    if (bCollectErrorData)
        ed = new ErrorData ((nPasses - nErrorDataStartIndex) * epoch_size, num_outputs);

    for (int i = 0; i < nPasses; i++)
    {
        ForwardPass (dsDataSource);

        if (bCollectErrorData && i >= nErrorDataStartIndex)
            for (int j = 0; j < epoch_size; j++)
            	ed->AddPair (&dsDataSource->GetSetAt (error_data_index++, io_data_set)[num_outputs], &y[j][first_output_index]);
    }

    return ed;
}

#endif // CTRNNALGORITHM_HPP_INCLUDED
