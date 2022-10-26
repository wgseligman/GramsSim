#include "SampleFromPowerLaw.h"

// ROOT includes
#include "TRandom.h"

// C++ includes
#include <cmath>

// Randomly sample from a function of the form f(E) = std::pow(E,a)
// where "E" is the energy and "a" is the "photon index".

namespace gramssky {

  double SampleFromPowerLaw(const double photonIndex, 
			    const double energyMin,
			    const double energyMax)
  {
    // Copied from
    // https://github.com/odakahirokazu/ComptonSoft/blob/master/anlgeant4/src/BasicPrimaryGen.cc
    
    // Generate a random value from a power-law spectrum.
    double energy = 0.0;
    
    // If the photon index is too close to 1, the exponent will blow
    // up. Treat that as a special case.
    if ( photonIndex > 0.999 && photonIndex < 1.001 ) {
      energy = energyMin * std::pow(energyMax/energyMin, gRandom->Uniform());
    }
    else {
      const double s = 1.0 - photonIndex;
      const double a0 = std::pow(energyMin, s);
      const double a1 = std::pow(energyMax, s);
      const double a = a0 + gRandom->Uniform()*(a1-a0);
      energy = std::pow(a, 1./s);
    }
    
    return energy;
  }
} // namespace gramssky

  
