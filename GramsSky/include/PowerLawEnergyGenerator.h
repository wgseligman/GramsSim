// 07-Jan-2022 WGS 

// Generate energy according to black-body distribution.

#ifndef Grams_PowerLawEnergyGenerator_h
#define Grams_PowerLawEnergyGenerator_h

#include "EnergyGenerator.h"

namespace gramssky {

  class PowerLawEnergyGenerator : public EnergyGenerator
  {
  public:

    // Constructor. 
    PowerLawEnergyGenerator();

    // Destructor.
    virtual ~PowerLawEnergyGenerator();

    // Generate an energy.
    virtual double Generate();

  private:
    // The parameters for the power-law distribution.
    double m_photonIndex;
    double m_energyMin;
    double m_energyMax;
  };

} // namespace gramssky

#endif // Grams_PowerLawEnergyGenerator_h
