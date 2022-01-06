// 20-Dec-2021 WGS 

// This is the "master routine" for handling particle generation in
// GramsSky. It's part of the "strategy design pattern": It determines
// which particle generation method is appropriate given the options
// in the user's options XML file.

#ifndef Grams_ParticleGeneration_h
#define Grams_ParticleGeneration_h

#include "PrimaryGenerator.h"

// C++ includes
#include <memory>

namespace gramssky {

  class ParticleGeneration
  {
  public:

    // Constructor. 
    ParticleGeneration();

    // Destructor.
    virtual ~ParticleGeneration();

    // Select an primary-generation method.
    std::shared_ptr<PrimaryGenerator> GetGenerator();

  private:
    // The generator selected by the user.
    std::shared_ptr<PrimaryGenerator> m_generator;
  };

} // namespace gramssky

#endif // Grams_ParticleGeneration_h
