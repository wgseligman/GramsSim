// 08-Dec-2021 WGS 

// An interface for all the GramsSky generators. This is part of a
// "strategy design pattern"; here we define the methods that must be
// implemented by specific primary-particle generators.

#ifndef Grams_PrimaryGenerator_h
#define Grams_PrimaryGenerator_h

#include "ParticleInfo.h"

// C++ includes
#include <memory>

namespace gramssky {

  class PrimaryGenerator
  {
  public:

    // Constructor. 
    PrimaryGenerator() {}

    // Destructor.
    virtual ~PrimaryGenerator() {}

    // Generate an event. This method MUST be overridden by a method
    // that inherits it.
    virtual std::shared_ptr<ParticleInfo> Generate() = 0;
  };

} // namespace gramssky

#endif // Grams_PrimaryGenerator_h
