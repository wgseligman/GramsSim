#include "PowerLawEnergyGenerator.h"
#include "Options.h" // in util

// ROOT includes
#include "TRandom.h"

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
    // Copied from
    // https://github.com/odakahirokazu/ComptonSoft/blob/master/anlgeant4/src/BasicPrimaryGen.cc

    // Generate a random value from a power-law spectrum.
    double energy = 0.0;
    
    // If the photon index is too close to 1, the exponent will blow
    // up. Treat that as a special case.
    if ( m_photonIndex > 0.999 && m_photonIndex < 1.001 ) {
      energy = m_energyMin * std::pow(m_energyMax/m_energyMin, gRandom->Uniform());
    }
    else {
      const double s = 1.0 - m_photonIndex;
      const double a0 = std::pow(m_energyMin, s);
      const double a1 = std::pow(m_energyMax, s);
      const double a = a0 + gRandom->Uniform()*(a1-a0);
      energy = std::pow(a, 1./s);
    }

    return energy;
  }

} // namespace gramssky
