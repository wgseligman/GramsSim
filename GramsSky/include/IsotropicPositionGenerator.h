// 06-Jan-2022 WGS 

// Generate positions isotropically on the inside of the celestial sphere.

#ifndef Grams_IsotropicPositionGenerator_h
#define Grams_IsotropicPositionGenerator_h

#include "ParticleInfo.h"
#include "PositionGenerator.h"

// ROOT includes
#include "TVector3.h"

// C++ includes
#include <vector>
#include <memory>

namespace gramssky {

  class IsotropicPositionGenerator : public PositionGenerator
  {
  public:

    // Constructor. 
    IsotropicPositionGenerator();

    // Destructor.
    virtual ~IsotropicPositionGenerator();

    // Generate an event.
    virtual std::shared_ptr<ParticleInfo> Generate();

  private:
    // The PDG code and mass of the primary particle.
    int m_PDG;
    double m_mass;

    // Radius of "celestial" sphere.
    double m_radius;

    // The theta and phi boundaries of the regions on the sphere. For
    // an isotropic distribution, we want to distribute evenly in cos
    // theta.
    double m_cosThetaMin;
    double m_cosThetaMax;
    std::vector<double> m_phiRange;
  };

} // namespace gramssky

#endif // Grams_IsotropicPositionGenerator_h
