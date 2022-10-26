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

// The different generators we can manage.
#include "PointPositionGenerator.h"
#include "IsotropicPositionGenerator.h"

#include "FixedEnergyGenerator.h"
#include "GaussianEnergyGenerator.h"
#include "UniformEnergyGenerator.h"
#include "BlackBodyEnergyGenerator.h"
#include "PowerLawEnergyGenerator.h"
#include "HistogramEnergyGenerator.h"

#ifdef HEALPIX_INSTALLED
#include "MapPowerLawGenerator.h"
#include "MapEnergyBands.h"
#endif

// ROOT includes
#include "TRandom.h"

// C++ includes
#include <string>
#include <memory>
#include <iostream>
#include <algorithm>

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

    // Not all the generators require a separate energy generator.
    bool energyGeneratorNeeded = true;

    // Determine which position generator we're going to use.
    std::string positionName;
    options->GetOption("PositionGeneration",positionName);
    // Save the original text for the error message.
    std::string positionInput(positionName);

    // Since users rarely follow directions, convert the name into
    // lower case.
    auto lowC = [](unsigned char c){ return std::tolower(c); };
    std::transform(positionName.begin(), positionName.end(), positionName.begin(), lowC);

    if ( positionName.compare("point") == 0 )
      m_generator = std::make_shared<PointPositionGenerator>();
    else if ( positionName.compare("iso") == 0 )
      m_generator = std::make_shared<IsotropicPositionGenerator>();
    else if ( positionName.compare("mappowerlaw") == 0 )
      {
#ifdef HEALPIX_INSTALLED
	m_generator = std::make_shared<MapPowerLawGenerator>();
	energyGeneratorNeeded = false;
#else
	std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		  << "The generator option '" << positionInput
		  << "' not recognized, since this program was compiled "
		  << "without the HEALPix libraries."
		  << std::endl;
	exit(EXIT_FAILURE);
#endif
      }
    else if ( positionName.compare("mapenergybands") == 0 )
      {
#ifdef HEALPIX_INSTALLED
	m_generator = std::make_shared<MapEnergyBands>();
	energyGeneratorNeeded = false;
#else
	std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		  << "The generator option '" << positionInput
		  << "' not recognized, since this program was compiled "
		  << "without the HEALPix libraries."
		  << std::endl;
	exit(EXIT_FAILURE);
#endif
      }
    else {
      std::cerr << "File " << __FILE__ << " Line " << __LINE__ << ":" << std::endl
		<< " Did not recognize PositionGenerator option '" << positionInput
		<< "'; Aborting job"
		<< std::endl;
      exit(EXIT_FAILURE);
    }

    if ( energyGeneratorNeeded ) {
      // Determine which energy generator we're going to use.
      std::string energyName;
      options->GetOption("EnergyGeneration",energyName);
      // Save the original text for the error message.
      std::string energyInput(energyName);
      
      // Again, convert to lower case.
      std::transform(energyName.begin(), energyName.end(), energyName.begin(), lowC);
      
      if ( energyName.compare("fixed") == 0 ) {
	auto energyGenerator = new FixedEnergyGenerator();
	m_generator->AdoptEnergyGenerator( energyGenerator );
      } 
      else if ( energyName.compare("gaus") == 0 ) {
	auto energyGenerator = new GaussianEnergyGenerator();
	m_generator->AdoptEnergyGenerator( energyGenerator );
      } 
      else if ( energyName.compare("flat") == 0 ) {
	auto energyGenerator = new UniformEnergyGenerator();
	m_generator->AdoptEnergyGenerator( energyGenerator );
      } 
      else if ( energyName.compare("blackbody") == 0 ) {
	auto energyGenerator = new BlackBodyEnergyGenerator();
	m_generator->AdoptEnergyGenerator( energyGenerator );
      } 
      else if ( energyName.compare("powerlaw") == 0 ) {
	auto energyGenerator = new PowerLawEnergyGenerator();
	m_generator->AdoptEnergyGenerator( energyGenerator );
      } 
      else if ( energyName.compare("hist") == 0 ) {
	auto energyGenerator = new HistogramEnergyGenerator();
	m_generator->AdoptEnergyGenerator( energyGenerator );
      } 
      else {
	std::cerr << "File " << __FILE__ << " Line " << __LINE__ << ":" << std::endl
		  << " Did not recognize EnergyGenerator option '" << energyInput
		  << "'; Aborting job"
		  << std::endl;
	exit(EXIT_FAILURE);
      }
    } // if energy generator needed
  } // end constructor

  // Destructor
  ParticleGeneration::~ParticleGeneration() {}

  std::shared_ptr<PositionGenerator> ParticleGeneration::GetGenerator() {
    return m_generator;
  }

} // namespace gramssky
