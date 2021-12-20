// ParticleGeneration.cc
// 20-Dec-2021 WGS

// This is the "master routine" for handling particle generation in
// GramsSky. It's part of the "strategy design pattern": It determines
// which particle generation method is appropriate given the options
// in the user's options XML file.

#include "ParticleGeneration.h"
#include "PrimaryGenerator.h"
#include "ParticleInfo.h"
#include "Options.h"

// The different generators we can return.
#include "MonoPrimaryGenerator.h"

// ROOT includes
#include "TRandom.h"

// C++ includes
#include <memory>

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

  }

  // Destructor
  ParticleGeneration::~ParticleGeneration() {}

  std::shared_ptr<PrimaryGenerator> ParticleGeneration::GetGenerator() {

    std::shared_ptr<PrimaryGenerator> generator;

    // Stub.
    generator = std::make_shared<MonoPrimaryGenerator>();

    return generator;
  }

} // namespace gramssky
