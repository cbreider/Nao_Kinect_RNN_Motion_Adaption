
/**
  * Returns the object's ID, which depends on the type of the object (any class derived from ActivationFunction).
  *
  * @return The object's ID
  */
inline int ActivationFunction::GetId ()
{
    return act_id;
};

SigmoidActivation* ActivationFunction::SIGMOID = new SigmoidActivation ();
TanhActivation* ActivationFunction::TANH = new TanhActivation ();
SoftmaxActivation* ActivationFunction::SOFTMAX = new SoftmaxActivation ();
IdentityActivation* ActivationFunction::IDENTITY = new IdentityActivation ();

/**
  * Computes a layer's sigmoid activation.
  *
  * @param x [in]The layer's induced local fields
  * @param y [out] The layer's outputs, computed by application of the sigmoid function
  */
void SigmoidActivation::ComputeActivations (const vec* x, vec* y)
{
    for (int i = 0; i < (int) (*x).n_rows; i++)
        (*y) (i) = 1 / (1 + exp (-(*x) (i)));
}

/**
  * Computes a layer's adapted sigmoid activation.
  *
  * @param x [in] The layer's induced local fields
  * @param adapt The adaptation method to be used; this may only make sense if the adaptation method is Intrinsic Plasticity (IP)
  * @param y [out] The layer's outputs after application of the adapted sigmoid function
  */
void SigmoidActivation::ComputeActivations (const vec* x, Adaptation* adapt, vec* y)
{
    for (int i = 0; i < (int) x->n_rows; i++)
    	(*y) (i) = 1 / (1 + exp (-adapt->GetA (i) * (*x) (i) - adapt->GetB (i)));
}

/**
  * Computes the derivatives of a layer's sigmoid activation.
  *
  * @param x [in] The layer's induced local fields
  * @param y_d [out] The layer's derived outputs, computed by application of the first derivative of the sigmoid function
  */
void SigmoidActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    double temp;

    for (int i = 0; i < (int) x->n_rows; i++)
    {
        temp = 1 / (1 + exp (-(*x) (i)));
    	(*y_d) (i) = temp * (1.0 - temp);
    }
}

/**
  * Computes a layer's hyperbole tangent (tanh) activation.
  *
  * @param x [in] The layer's induced local fields
  * @param y [out] The layer's outputs after application of the hyperbole tangent function
  */
void TanhActivation::ComputeActivations (const vec* x, vec* y)
{
    *y = tanh (*x);
}

/**
  * Computes the derivatives of a layer's hyperbole tangent (tanh) activation.
  *
  * @param x [in] The layer's induced local fields
  * @param y_d [out] The layer's derived outputs, computed by application of the first derivative of the hyperbole tangent function
  */
void TanhActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    for (int i = 0; i < (int) x->n_rows; i++)
    	(*y_d) (i) = 1.0 - tanh ((*x) (i)) * tanh ((*x) (i));
}

/**
  * Computes a layer's softmax activation.
  *
  * @param x The layer's induced local fields
  * @param y The layer's outputs, computed by application of the softmax function
  */
void SoftmaxActivation::ComputeActivations (const vec* x, vec* y)
{
    vec exp_ilfs = exp (*x);
    double exp_sum = sum (exp_ilfs);
    *y = exp_ilfs / exp_sum;
}

/**
  * Causes the program to terminate with an error since the softmax function has no derivative.
  *
  * @param x [in] Ignored
  * @param y_d [out] Is left untouched
  */
void SoftmaxActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    Utilities::Assert(false, (char*) "ERROR: Cannot compute derivatices of softmax activation function!");
}

/**
  * Copies a layer's induced local fields into its output
  *
  * @param x [in] The layer's induced local fields
  * @param y [out] The layer's output (i.e. copies of the values in x)
  */
void IdentityActivation::ComputeActivations (const vec* x, vec* y)
{
    *y = *x;
}

/**
  * Computes the derivatives - they are all 1 - of a layer's identity activation.
  *
  * @param x [in] The layer's induced local fields
  * @param y [out] A vector full of 1s, with its dimension identical to x's dimension
  */
void IdentityActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    y_d->ones (x->n_rows);
}

/**
  * Constructor; stores pointers to a user-defined activation function and its first derivative.
  *
  * @param UserDefActFunc Pointer to a user-defined activation function (type: double->double)
  * @param UserDefDerFunc Pointer to the first derivative of the above function (type: double->double)
  */
UserActivation::UserActivation (double (*UserDefActFunc) (double), double (*UserDefDerFunc) (double))
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(UserDefActFunc != NULL, (char*) "ERROR: UserDefActFunc must not be NULL in UserActivation::UserActivation!");
    #endif

    act_id = ID_USER;
    act_func = UserDefActFunc;
    act_func_der = UserDefDerFunc;
}

/**
  * Computes a layer's activation using the user-defined activation function passed to the objects constructor.
  *
  * @param x [in] The layer's induced local fields
  * @param y [out] The layer's output, computed by application of the user-defined activation function passed to the constructor
  */
void UserActivation::ComputeActivations (const vec* x, vec* y)
{
    for (int i = 0; i < (int) x->n_rows; i++)
    	(*y) (i) = act_func ((*x) (i));
}

/**
  * Computes the derivatives of a layer's activation using the user-defined derivative of the activation function passed to the objects constructor. Causes the program to terminate with an error if the pointer passed to the constructor was NULL.
  *
  * @param x [in] The layer's induced local fields
  * @param y [out] The layer's output, computed by application of the user-defined first derivative of the user-defined activation function passed to the constructor
  */
void UserActivation::ComputeDerivatives (const vec* x, vec* y_d)
{
    #ifndef NNLIB_NO_ERROR_CHECKING
    Utilities::Assert(act_func_der != NULL, (char*) "ERROR: Attempt to call unspecified derivative of user-defined activation function!");
    #endif

    for (int i = 0; i < (int) x->n_rows; i++)
    	(*y_d) (i) = act_func_der ((*x) (i));
}
