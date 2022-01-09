#include "BlackBodyEnergyGenerator.h"
#include "Options.h" // in util

// ROOT includes
#include "TRandom.h"

// C++ includes
#include <cmath>

namespace gramssky {

  BlackBodyEnergyGenerator::BlackBodyEnergyGenerator()
    : EnergyGenerator()
  {
    // Get the parameters from the Options XML file.
    auto options = util::Options::GetInstance();
    options->GetOption("RadTemp",m_radTemp);
    options->GetOption("EnergyMax",m_energyMax);

    // Prevent the users from creating zero-energy particles, which
    // would cause problems later in the simulation chain.
    m_energyMin = std::max( 1.0e-9, m_energyMin );
    
    // The formula for black-body radiation.
    m_bbformula = new TF1("bb","(x*x)/(TMath::Exp(x) - 1.0)",
			  m_energyMin/m_radTemp,
			  m_energyMax/m_radTemp);
  }

  BlackBodyEnergyGenerator::~BlackBodyEnergyGenerator()
  {}

  double BlackBodyEnergyGenerator::Generate()
  {
    // Copied from
    // https://github.com/odakahirokazu/ComptonSoft/blob/master/anlgeant4/src/BasicPrimaryGen.cc

    // Generate a random value from the black-body distribution.
    // Scale it to the radiation temperature. 
    double energy = m_radTemp * m_bbformula->GetRandom();
    return energy;
  }

} // namespace gramssky
