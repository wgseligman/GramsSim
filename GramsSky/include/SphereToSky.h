// SphereToSky.h
// 05-Jan-2022 WGS 

// In GramsSky, we generate a particle coming from the celestial
// sphere according to some energy/position distribution. However, the
// simulation's sphere has some finite radius within the world volume,
// and the actual celestial sphere is actually at "infinity".

// To simulate this effect, we construct an imaginary tangent disc
// tangent to the simulation sphere at the point of the particle's
// generation; see GramsSky/SkyDiagram.jpg and GramsSky/README.md for
// details. This routine randomly shifts the origin of the particle
// across the surface of that disc.

// It also adjusts for the direction of the sphere with respect to
// detector coordinates, and the center of the detector with respect
// to the center of the sphere.

#ifndef Grams_SphereToSky_h
#define Grams_SphereToSky_h

#include "ParticleInfo.h"

#include <TVector3.h>
#include <memory> // for shared_ptr

namespace gramssky {

  class SphereToSky
  {
  public:

    // Constructor. 
    SphereToSky();

    // Destructor.
    virtual ~SphereToSky();

    // Take a pointer to a ParticleInfo and generate a new
    // ParticleInfo with a transformed position.
    std::shared_ptr<ParticleInfo> Transform( const std::shared_ptr<ParticleInfo>  );

  private:

    // Parameters from the Options XML file. Units are arbitrary, but
    // make sure they agree with GramsSky/gramssky.cc.

    // The actual radius of the simulated sphere.
    double m_radiusSphere;

    // The radius of the tangent disc. If this is <= 0, then
    // m_radiusSphere is used.
    double m_radiusDisc;

    // The center of the simulated sphere in detector
    // coordinates. Bear in mind that the origin of the coordinate
    // system is determined by the GRAMS GDML file, and as Jan-2022 it
    // is _not_ the center of the detector.
    TVector3 m_originSphere;

    // This is the direction of the "north pole" of the celestial
    // sphere with in the detector coordinate system. Bear in mind
    // that imported celestial maps are usually aligned with the
    // galactic coordinate system, while the physical detector will
    // have typically have some other orientation for its z-axis.
    TVector3 m_mapDirection;
  };

} // namespace gramssky

#endif // Grams_SphereToSky_h
