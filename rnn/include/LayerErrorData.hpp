#ifndef LAYERERRORDATA_HPP_INCLUDED
#define LAYERERRORDATA_HPP_INCLUDED

/**
  * Represents the error data collected for one output layer during the testing phase of a neural network. There's no use in creating objects of this class yourself.
  */
class LayerErrorData
{
    public:
        LayerErrorData (int, int);

        void AddPair (vec*, vec*);
        void SetPairAt (int, vec*, vec*);
        void GetPairAt (int, vec*);
        void SetOutputAt (int, vec*);
        void GetOutputAt (int, vec*);
        void SetDesiredOutputAt (int, vec*);
        void GetDesiredOutputAt (int, vec*);

        inline int GetTimeSteps ();
        inline int GetDimension ();

        double GetMse (int);
        double GetMse ();
        double GetKld (int);
        double GetKld ();

    private:
        int count;

        mat d;
        mat y;
};

#endif // LAYERERRORDATA_HPP_INCLUDED
