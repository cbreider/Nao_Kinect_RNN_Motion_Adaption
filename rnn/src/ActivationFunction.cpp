

inline int ActivationFunction::GetId ()
{
    return act_id;
};

SigmoidActivation* ActivationFunction::SIGMOID = new SigmoidActivation ();
TanhActivation* ActivationFunction::TANH = new TanhActivation ();
SoftmaxActivation* ActivationFunction::SOFTMAX = new SoftmaxActivation ();
IdentityActivation* ActivationFunction::IDENTITY = new IdentityActivation ();


void SigmoidActivation::ComputeActivations (const vec* x, vec* y)
{
    for (int i = 0; i < (int) (*x).n_rows; i++)
        (*y) (i) = 1 / (1 + exp (-(*x) (i)));
}


void SigmoidActivation::ComputeActivations (const vec* x, Adaptation* adapt, vec* y)
{
    for (int i = 0; i < (int) x->n_rows; i++)
    	(*y) (i) = 1 / (1 + exp (-adapt->GetA (i) * (*x) (i) - adapt->GetB (i)));
}


void SigmoidActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    double temp;

    for (int i = 0; i < (int) x->n_rows; i++)
    {
        temp = 1 / (1 + exp (-(*x) (i)));
    	(*y_d) (i) = temp * (1.0 - temp);
    }
}


void TanhActivation::ComputeActivations (const vec* x, vec* y)
{
    *y = tanh (*x);
}


void TanhActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    for (int i = 0; i < (int) x->n_rows; i++)
    	(*y_d) (i) = 1.0 - tanh ((*x) (i)) * tanh ((*x) (i));
}


void SoftmaxActivation::ComputeActivations (const vec* x, vec* y)
{
    vec exp_ilfs = exp (*x);
    double exp_sum = sum (exp_ilfs);
    *y = exp_ilfs / exp_sum;
}


void SoftmaxActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    Utilities::Assert(false, (char*) "ERROR: Cannot compute derivatices of softmax activation function!");
}


void IdentityActivation::ComputeActivations (const vec* x, vec* y)
{
    *y = *x;
}


void IdentityActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    y_d->ones (x->n_rows);
}


UserActivation::UserActivation (double (*UserDefActFunc) (double), double (*UserDefDerFunc) (double))
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(UserDefActFunc != NULL, (char*) "ERROR: UserDefActFunc must not be NULL in UserActivation::UserActivation!");
    #endif

    act_id = ID_USER;
    act_func = UserDefActFunc;
    act_func_der = UserDefDerFunc;
}


void UserActivation::ComputeActivations (const vec* x, vec* y)
{
    for (int i = 0; i < (int) x->n_rows; i++)
    	(*y) (i) = act_func ((*x) (i));
}


void UserActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(act_func_der != NULL, (char*) "ERROR: Attempt to call unspecified derivative of user-defined activation function!");
    #endif

    for (int i = 0; i < (int) x->n_rows; i++)
    	(*y_d) (i) = act_func_der ((*x) (i));
}
