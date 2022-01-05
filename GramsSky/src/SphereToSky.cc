#include "SphereToSky.h"
#include "ParticleInfo.h"
#include "Options.h" // in util/

// ROOT includes
#include <TRandom.h>
#include <TVector3.h> // defines ROOT::Math::XYZVector

// C++ includes
#include <memory> // for shared_ptr
#include <vector>
#include <cmath>

namespace gramssky {

  SphereToSky::SphereToSky()
  {
    // Fetch options from XML file.
    auto options = util::Options::GetInstance();

    options->GetOption("RadiusSphere",m_radiusSphere);

    // If the disc radius is invalid, use the sphere radius.
    options->GetOption("RadiusDisc",m_radiusDisc);
    if ( m_radiusDisc <= 0. ) m_radiusDisc = m_radiusSphere;

    std::vector<double> coord;
    options->GetOption("OriginSphere",coord);
    m_originSphere.SetXYZ(coord[0],coord[1],coord[2]);

    // The map direction has to be a unit vector.
    options->GetOption("MapDirection",coord);
    m_mapDirection.SetXYZ(coord[0],coord[1],coord[2]);
    m_mapDirection = m_mapDirection.Unit();
  }

  SphereToSky::~SphereToSky()
  {}

  std::shared_ptr<ParticleInfo> SphereToSky::Transform( const std::shared_ptr<ParticleInfo> a_info )
  {
    // Make a copy of the ParticleInfo object in the argument. That
    // way we can fiddle with it our heart's content without affected
    // the object in the calling routine.
    auto info = std::make_shared<ParticleInfo>(*a_info);

    // Get the source of the particle on the inner surface of the
    // simulation sphere.
    TVector3 source(info->GetX(), info->GetY(), info->GetZ());

    // Rescale this to the radius of the simulation sphere. The caller
    // may have already done this, or they may have simply generated
    // the particle on a unit sphere or something like that.
    source.SetMag(m_radiusSphere);

    // Rotate the position (in the sphere's rotation) to match the
    // detector's rotation.
    source.RotateUz(m_mapDirection);

    // Determine the vector that lies along the plane of the disc. 
    TVector3 disc = source.Orthogonal();

    // A random position from the center of the disc.
    disc.SetMag( m_radiusDisc * std::sqrt( gRandom->Rndm() ) );

    // A random angle around the disc.
    const double angle = M_PI * gRandom->Rndm();
    disc.Rotate( angle, source );

    // The adjusted position on the tangent disc.
    TVector3 position = m_originSphere + source + disc;

    // Determine the revised particle direction, and scale it by the
    // particle's original momentum.
    TVector3 direction = -source;
    TVector3 momentum( info->GetPx(), info->GetPy(), info->GetPz() );
    direction.SetMag( momentum.Mag() );

    // Put the revised values into the ParticleInfo output.
    info->SetX( position.x() );
    info->SetY( position.y() );
    info->SetZ( position.z() );
    info->SetPx( direction.x() );
    info->SetPy( direction.y() );
    info->SetPz( direction.z() );

    return info;
  }

} // namespace gramssky
