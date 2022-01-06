// 06-Jan-2022 WGS 

// A simple energy generator: A fixed value

#ifndef Grams_FixedEnergyGenerator_h
#define Grams_FixedEnergyGenerator_h

#include "EnergyGenerator.h"

namespace gramssky {

  class FixedEnergyGenerator : public EnergyGenerator
  {
  public:

    // Constructor. 
    FixedEnergyGenerator();

    // Destructor.
    virtual ~FixedEnergyGenerator();

    // Generate an energy.
    virtual double Generate();

  private:
    // The energy we'll return.
    double m_energy;
  };

} // namespace gramssky

#endif // Grams_FixedEnergyGenerator_h
