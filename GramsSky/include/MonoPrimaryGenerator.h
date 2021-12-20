// 08-Dec-2021 WGS 

// A simple generator: A monoenergetic beam from a fixed point in the
// sky.

#ifndef Grams_MonoPrimaryGenerator_h
#define Grams_MonoPrimaryGenerator_h

#include "ParticleInfo.h"
#include "PrimaryGenerator.h"

// C++ includes
#include <memory>

namespace gramssky {

  class MonoPrimaryGenerator : public PrimaryGenerator
  {
  public:

    // Constructor. 
    MonoPrimaryGenerator();

    // Destructor.
    virtual ~MonoPrimaryGenerator();

    // Generate an event.
    virtual std::shared_ptr<ParticleInfo> Generate();

  private:
  };

} // namespace gramssky

#endif // Grams_MonoPrimaryGenerator_h
