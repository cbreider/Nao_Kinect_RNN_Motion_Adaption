#ifndef ADAPTATION_HPP_INCLUDED
#define ADAPTATION_HPP_INCLUDED

/**
  * Performs intrinsic plasticity (IP) adaptation. If possible/practicable, this class should be modified to act as an abstract base class from which classes implementing adaptations such as IP can be derived.
  */
class Adaptation
{
    public:
        Adaptation (double, double);
        ~Adaptation ();
        void Resize (int);
        inline double GetA (int);
        inline double GetB (int);
        void Adapt (vec*, vec*);

    private:
        int size;
        double mu;
        double lr;
        double* a;
        double* b;
};

#endif // ADAPTATION_HPP_INCLUDED
