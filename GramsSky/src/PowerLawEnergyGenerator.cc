#include "PowerLawEnergyGenerator.h"
#include "SampleFromPowerLaw.h"
#include "Options.h" // in util

// C++ includes
#include <cmath>

namespace gramssky {

  PowerLawEnergyGenerator::PowerLawEnergyGenerator()
    : EnergyGenerator()
  {
    // Get the parameters from the Options XML file.
    auto options = util::Options::GetInstance();
    options->GetOption("PhotonIndex",m_photonIndex);
    options->GetOption("EnergyMin",m_energyMin);
    options->GetOption("EnergyMax",m_energyMax);

    // Prevent the users from creating zero-energy particles, which
    // would cause problems later in the simulation chain.
    m_energyMin = std::max( 1.0e-9, m_energyMin );
  }

  PowerLawEnergyGenerator::~PowerLawEnergyGenerator()
  {}

  double PowerLawEnergyGenerator::Generate()
  {
    // Generate a random value from a power-law spectrum.
    double energy = SampleFromPowerLaw(m_photonIndex, m_energyMin, m_energyMax);
    return energy;
  }

} // namespace gramssky
