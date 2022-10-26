#include "GaussianEnergyGenerator.h"
#include "Options.h" // in util

// ROOT includes
#include "TRandom.h"

// C++ includes
#include <cmath>

namespace gramssky {

  GaussianEnergyGenerator::GaussianEnergyGenerator()
    : EnergyGenerator()
  {
    // Get the parameters from the Options XML file.
    auto options = util::Options::GetInstance();
    options->GetOption("GausMean",m_mean);
    options->GetOption("GausWidth",m_width);
    options->GetOption("EnergyMin",m_energyMin);
    options->GetOption("EnergyMax",m_energyMax);

    // Prevent the users from creating zero-energy particles, which
    // would cause problems later in the simulation chain.
    m_energyMin = std::max( 1.0e-9, m_energyMin );
  }

  GaussianEnergyGenerator::~GaussianEnergyGenerator()
  {}

  double GaussianEnergyGenerator::Generate()
  {
    // Generate a random value from a gaussian distribution.
    double energy = gRandom->Gaus(m_mean,m_width);

    while ( energy < m_energyMin || energy > m_energyMax ) {
      energy = gRandom->Gaus(m_mean,m_width);
    }

    return energy;
  }

} // namespace gramssky
