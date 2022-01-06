// ParticleGeneration.cc
// 20-Dec-2021 WGS

// This is the "master routine" for handling particle generation in
// GramsSky. It's part of the "strategy design pattern": It determines
// which particle generation methods are appropriate given the options
// in the user's options XML file.

#include "ParticleGeneration.h"
#include "PositionGenerator.h"
#include "EnergyGenerator.h"
#include "ParticleInfo.h"
#include "Options.h"

// The different generators we can return.
#include "PointPositionGenerator.h"
#include "IsotropicPositionGenerator.h"
#include "FixedEnergyGenerator.h"
#include "GaussianEnergyGenerator.h"

// ROOT includes
#include "TRandom.h"

// C++ includes
#include <string>
#include <memory>
#include <iostream>

namespace gramssky {

  // Constructor
  ParticleGeneration::ParticleGeneration() {

    // Set up accessing program options.
    auto options = util::Options::GetInstance();
    
    // Most generators will require random numbers.  Get and set the
    // random number seed.
    int seed;
    options->GetOption("rngseed",seed);
    // Note that the default random-number generator in ROOT is
    // TRandom3.
    gRandom->SetSeed(seed);

    // Determine which position generator we're going to use.
    std::string positionName;
    options->GetOption("PositionGeneration",positionName);
    if ( positionName.compare("Point") == 0 )
      m_generator = std::make_shared<PointPositionGenerator>();
    else if ( positionName.compare("Iso") == 0 )
      m_generator = std::make_shared<IsotropicPositionGenerator>();
    else {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << ":" << std::endl
		<< " Did not recognize position generator '" << positionName
		<< "'; Aborting job"
		<< std::endl;
      exit(EXIT_FAILURE);
    }

    // Determine which energy generator we're going to use.
    std::string energyName;
    options->GetOption("EnergyGeneration",energyName);
    if ( energyName.compare("Fixed") == 0 ) {
      auto energyGenerator = new FixedEnergyGenerator();
      m_generator->AdoptEnergyGenerator( energyGenerator );
    } 
    else if ( energyName.compare("Gaus") == 0 ) {
      auto energyGenerator = new GaussianEnergyGenerator();
      m_generator->AdoptEnergyGenerator( energyGenerator );
    } 
    else {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << ":" << std::endl
		<< " Did not recognize energy generator '" << energyName
		<< "'; Aborting job"
		<< std::endl;
      exit(EXIT_FAILURE);
    }
  }

  // Destructor
  ParticleGeneration::~ParticleGeneration() {}

  std::shared_ptr<PositionGenerator> ParticleGeneration::GetGenerator() {
    return m_generator;
  }

} // namespace gramssky
