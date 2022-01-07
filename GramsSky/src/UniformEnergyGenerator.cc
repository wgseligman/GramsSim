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
    options->GetOption("FlatMin",m_energyMin);
    options->GetOption("FlatMax",m_energyMax);
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
