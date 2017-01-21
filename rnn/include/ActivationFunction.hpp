#ifndef ACTIVATIONFUNCTION_HPP_INCLUDED
#define ACTIVATIONFUNCTION_HPP_INCLUDED

/**
  * Abstract base class from which classes implementing activation functions can be derived. Those classes must define ComputeActivations() and ComputeDerivatives(). Objects of the derived classes can be passed to Layer::Layer().
  */
class ActivationFunction
{
    public:
        virtual void ComputeActivations (const vec*, vec*) = 0;
        virtual void ComputeDerivatives (const vec*, vec*) = 0;

        inline int GetId ();

        static const int ID_SIGMOID = 0;
        static const int ID_TANH = 1;
        static const int ID_IDENTITY = 2;
        static const int ID_SOFTMAX = 3;
        static const int ID_USER = 4;

        static SigmoidActivation* SIGMOID;
        static TanhActivation* TANH;
        static SoftmaxActivation* SOFTMAX;
        static IdentityActivation* IDENTITY;

    protected:
        int act_id;
};

/**
  * Implements sidmoid activation, i.e. f(x) = 1 / (1 + e^x).
  */
class SigmoidActivation : public ActivationFunction
{
    public:
        /**
          * Constructor; sets the objects ID to ACTIVATION_SIGMOID.
          */
        SigmoidActivation () { act_id = ID_SIGMOID; };
        void ComputeActivations (const vec*, vec*);
        void ComputeActivations (const vec*, Adaptation*, vec*);
        void ComputeDerivatives (const vec*, vec*);
};

/**
  * Implements hyperbolic tangent activation.
  */
class TanhActivation : public ActivationFunction
{
    public:
        /**
          * Constructor; sets the objects ID to ACTIVATION_TANH.
          */
        TanhActivation () { act_id = ID_TANH; };
        void ComputeActivations (const vec*, vec*);
        void ComputeDerivatives (const vec*, vec*);
};

/**
  * Implements softmax activation, i.e. f(x) = e^x / (sum_z e^z) where sum_z iterates over all units z in x's layer and sums up e^z.
  */
class SoftmaxActivation : public ActivationFunction
{
    public:
        /**
          * Constructor; sets the objects ID to ACTIVATION_SOFTMAX.
          */
        SoftmaxActivation () { act_id = ID_SOFTMAX; };
        void ComputeActivations (const vec*, vec*);
        void ComputeDerivatives (const vec*, vec*);
};

/**
  * Implements identity activation, i.e. f(x) = x.
  */
class IdentityActivation : public ActivationFunction
{
    public:
        /**
          * Constructor; sets the objects ID to ACTIVATION_IDENTITY.
          */
        IdentityActivation () { act_id = ID_IDENTITY; };
        void ComputeActivations (const vec*, vec*);
        void ComputeDerivatives (const vec*, vec*);
};

/**
  * Calls user-defined activation functions which are passed to the constructor.
  */
class UserActivation : public ActivationFunction
{
    public:
        UserActivation (double (*) (double), double (*) (double));
        void ComputeActivations (const vec*, vec*);
        void ComputeDerivatives (const vec*, vec*);

    protected:
        double (*act_func) (double);
        double (*act_func_der) (double);
};

#endif // ACTIVATIONFUNCTION_HPP_INCLUDED
