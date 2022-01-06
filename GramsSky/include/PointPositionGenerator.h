// 08-Dec-2021 WGS 

// A simple generator: A beam from a fixed point in the sky.

#ifndef Grams_PointPositionGenerator_h
#define Grams_PointPositionGenerator_h

#include "ParticleInfo.h"
#include "PositionGenerator.h"

// ROOT includes
#include "TVector3.h"

// C++ includes
#include <memory>

namespace gramssky {

  class PointPositionGenerator : public PositionGenerator
  {
  public:

    // Constructor. 
    PointPositionGenerator();

    // Destructor.
    virtual ~PointPositionGenerator();

    // Generate an event.
    virtual std::shared_ptr<ParticleInfo> Generate();

  private:
    // The PDG code of the primary particle.
    int m_PDG;

    // The (x,y,z) location of the primary particle.
    TVector3 m_point;
  };

} // namespace gramssky

#endif // Grams_PointPositionGenerator_h
