#include "FixedEnergyGenerator.h"
#include "Options.h" // in util

namespace gramssky {

  FixedEnergyGenerator::FixedEnergyGenerator()
    : EnergyGenerator()
  {
    // Get the energy from the Options XML file.
    auto options = util::Options::GetInstance();
    options->GetOption("FixedEnergy",m_energy);
  }

  FixedEnergyGenerator::~FixedEnergyGenerator()
  {}

  double FixedEnergyGenerator::Generate()
  {
    // Generate the fixed energy value.
    return m_energy;
  }

} // namespace gramssky
