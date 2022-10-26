#include "UniformEnergyGenerator.h"
#include "Options.h" // in util

// ROOT includes
#include "TRandom.h"

// C++ includes
#include <cmath>

namespace gramssky {

  UniformEnergyGenerator::UniformEnergyGenerator()
    : EnergyGenerator()
  {
    // Get the parameters from the Options XML file.
    auto options = util::Options::GetInstance();
    options->GetOption("EnergyMin",m_energyMin);
    options->GetOption("EnergyMax",m_energyMax);

    // Prevent the users from creating zero-energy particles, which
    // would cause problems later in the simulation chain.
    m_energyMin = std::max( 1.0e-9, m_energyMin );
  }

  UniformEnergyGenerator::~UniformEnergyGenerator()
  {}

  double UniformEnergyGenerator::Generate()
  {
    // Generate a random value from a flat distribution.
    double energy = gRandom->Uniform(m_energyMin,m_energyMax);

    return energy;
  }

} // namespace gramssky
