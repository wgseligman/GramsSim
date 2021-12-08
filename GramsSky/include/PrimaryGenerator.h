// 08-Dec-2021 WGS 

// An interface for all the GramsSky generators.

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

    // Generate an event.
    virtual std::shared_ptr<ParticleInfo> Generate() = 0;
  };

} // namespace gramssky

#endif // Grams_PrimaryGenerator_h
