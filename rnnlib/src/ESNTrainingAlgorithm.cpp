
PseudoInverseAlgorithm* ESNTrainingAlgorithm::PSEUDOINV = new PseudoInverseAlgorithm ();
WienerHopfAlgorithm* ESNTrainingAlgorithm::WIENERHOPF = new WienerHopfAlgorithm ();
LinearLeastSquaresAlgorithm* ESNTrainingAlgorithm::SOLVE = new LinearLeastSquaresAlgorithm ();

/**
  * Computes the new weight matrix by taking the pseudoinverse of the state matrix and multiplying it by the matrix of desired outputs.
  *
  * @param matStates The state matrix
  * @param matDesiredOutput The desired output matrix
  */
mat PseudoInverseAlgorithm::ComputeWeightMatrix (mat* matStates, mat* matDesiredOutput)
{
    return trans (pinv (*matStates) * *matDesiredOutput);
}

/**
  * Computes the new weight matrix using the Wiener-Hopf method.
  *
  * @param matStates The state matrix
  * @param matDesiredOutput The desired output matrix
  */
mat WienerHopfAlgorithm::ComputeWeightMatrix (mat* matStates, mat* matDesiredOutput)
{
    return trans (inv (trans (*matStates) * *matStates) * (trans (*matStates) * *matDesiredOutput));
}

/**
  * Computes the new weight matrix using Armadillo's solve() function.
  *
  * @param matStates The state matrix
  * @param matDesiredOutput The desired output matrix
  */
mat LinearLeastSquaresAlgorithm::ComputeWeightMatrix (mat* matStates, mat* matDesiredOutput)
{
    return trans (solve (*matStates, *matDesiredOutput));
}
