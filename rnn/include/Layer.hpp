#ifndef LAYER_HPP_INCLUDED
#define LAYER_HPP_INCLUDED

/**
  * Represents the layer of a recurrent neural network.
  */
class Layer
{
    public:
        Layer (int, ActivationFunction* = NULL, bool = false, bool = false, double = 1.0, vec* = NULL);

        inline int GetSize ();
        inline double GetTimeConstant ();
        inline void SetDefaultStates (const vec*);
        inline void GetDefaultStates (vec*);
        inline void SetDefaultState (int, double);
        inline double GetDefaultState (int);
        inline bool IsPbLayer ();
        inline bool IsConst ();
        virtual inline void ComputeActivations (vec*, vec*);
        inline void ComputeDerivatives (vec*, vec*);

    protected:
        bool is_pb;
        bool is_const_pb;
        int num_units;
        ActivationFunction* act_func;
        double tau;
        vec u;
};

#endif // LAYER_HPP_INCLUDED
