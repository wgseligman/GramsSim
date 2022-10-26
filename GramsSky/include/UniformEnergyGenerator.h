// 06-Jan-2022 WGS 

// Generate energy uniformly between two values.

#ifndef Grams_UniformEnergyGenerator_h
#define Grams_UniformEnergyGenerator_h

#include "EnergyGenerator.h"

namespace gramssky {

  class UniformEnergyGenerator : public EnergyGenerator
  {
  public:

    // Constructor. 
    UniformEnergyGenerator();

    // Destructor.
    virtual ~UniformEnergyGenerator();

    // Generate an energy.
    virtual double Generate();

  private:
    // The parameters for the flat distribution.
    double m_energyMin;
    double m_energyMax;
  };

} // namespace gramssky

#endif // Grams_UniformEnergyGenerator_h
