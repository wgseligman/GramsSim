// 06-Jan-2022 WGS 

// Generate energy according to a gaussian distribution.

#ifndef Grams_GaussianEnergyGenerator_h
#define Grams_GaussianEnergyGenerator_h

#include "EnergyGenerator.h"

namespace gramssky {

  class GaussianEnergyGenerator : public EnergyGenerator
  {
  public:

    // Constructor. 
    GaussianEnergyGenerator();

    // Destructor.
    virtual ~GaussianEnergyGenerator();

    // Generate an energy.
    virtual double Generate();

  private:
    // The parameters for the gaussian distribution.
    double m_mean;
    double m_width;
    double m_energyMin;
    double m_energyMax;
  };

} // namespace gramssky

#endif // Grams_GaussianEnergyGenerator_h
