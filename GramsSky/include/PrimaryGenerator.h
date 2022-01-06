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

  // Forward declarations.
  class TransformCoordinates;
  class EnergyGenerator;

  class PrimaryGenerator
  {
  public:

    // Constructor. 
    PrimaryGenerator(); 

    // Destructor.
    virtual ~PrimaryGenerator();

    // Generate an event. This method MUST be overridden by a method
    // that inherits it.
    virtual std::shared_ptr<ParticleInfo> Generate() = 0;

    // One thing most generators will have in common is a need to
    // transform the particle they generate (on the inside of a
    // celestial sphere) into the coordinate system of the
    // detector. This gives the generators common access to the same
    // routine.
    TransformCoordinates* GetTransform() { return m_transform; }

    // Another things most generators will need is a separate process
    // to generate energy. This will be determined by
    // ParticleGeneration (a double strategy!) and stored (and
    // deleted) in this class.
    void AdoptEnergyGenerator( EnergyGenerator* gen ) { m_energyGenerator = gen; }
    EnergyGenerator* GetEnergyGenerator() { return m_energyGenerator; }

  private:
    
    TransformCoordinates* m_transform;
    EnergyGenerator* m_energyGenerator;

  };

} // namespace gramssky

#endif // Grams_PrimaryGenerator_h
