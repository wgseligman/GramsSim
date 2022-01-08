// 07-Jan-2022 WGS 

// Generate energy according to black-body distribution.

#ifndef Grams_BlackBodyEnergyGenerator_h
#define Grams_BlackBodyEnergyGenerator_h

#include "EnergyGenerator.h"

// ROOT includes
#include "TF1.h"

namespace gramssky {

  class BlackBodyEnergyGenerator : public EnergyGenerator
  {
  public:

    // Constructor. 
    BlackBodyEnergyGenerator();

    // Destructor.
    virtual ~BlackBodyEnergyGenerator();

    // Generate an energy.
    virtual double Generate();

  private:
    // The parameters for the flat distribution.
    double m_radTemp;
    double m_energyMin;
    double m_energyMax;

    TF1* m_bbformula;
  };

} // namespace gramssky

#endif // Grams_BlackBodyEnergyGenerator_h
