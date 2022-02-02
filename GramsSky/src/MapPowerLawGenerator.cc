// Have the HEALPix libraries been found on this system?
#ifdef FITS_FOUND

#include "MapPowerLawGenerator.h"
#include "PositionGenerator.h" // For GetTransform
#include "TransformCoordinates.h"
#include "Options.h" // in util

// ROOT includes
#include "TRandom.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

// HEALPix and FITSIO includes
#include <healpix_cxx/healpix_base.h>
#include <healpix_cxx/healpix_map.h>
#include <healpix_cxx/healpix_map_fitsio.h>
#include <healpix_cxx/fitshandle.h>

// C++ includes
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

namespace gramssky {

  MapPowerLawGenerator::MapPowerLawGenerator()
    : PositionGenerator()
  {
    // Get the values from the Options XML file.
    auto options = util::Options::GetInstance();

    // Debug flag from Options.
    bool debug;
    options->GetOption("debug",debug);

    options->GetOption("EnergyMin",m_energyMin);
    options->GetOption("EnergyMax",m_energyMax);

    // Prevent the users from creating zero-energy particles, which
    // would cause problems later in the simulation chain.
    m_energyMin = std::max( 1.0e-9, m_energyMin );

    // Get the PDG code; from that get the mass of the particle.
    options->GetOption("PrimaryPDG",m_PDG);
    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(m_PDG);
    m_mass = 0.;
    if ( particle != NULL ) {
      m_mass = particle->Mass();
      if (debug) {
	std::cout << "MapPowerLawGenerator: PDG code "
		  << m_PDG << " found in ROOT particle database, mass = "
		  << m_mass << std::endl;
      }
    }
    else {
      if (debug) {
	std::cout << "MapPowerLawGenerator: PDG code "
		  << m_PDG << " not found in ROOT particle database! "
		  << " mass set to 0" << std::endl;
      }
    }
  }

  MapPowerLawGenerator::~MapPowerLawGenerator()
  {}

  std::shared_ptr<ParticleInfo> MapPowerLawGenerator::Generate()
  {
    // Create a new particle.
    auto particle = std::make_shared<ParticleInfo>();

    // Use the values from the Options XML file.
    particle->SetPDG(m_PDG);

    // Stub.
    double energy = 1.0;
    particle->SetE(energy);

    // We don't have to set correct particle direction here, since the
    // Transform routine will rotate the momentum vector to point in
    // the direction of the detector's center. But we do want to get
    // the magnitude right.
    double momentum = std::sqrt( energy*energy - m_mass*m_mass );
    particle->SetPz(-momentum);

    // Transform the particle position and direction from the
    // celestial sphere to the detector coordinates, applying a random
    // shift to simulate a flux from infinity.
    auto transformed = GetTransform()->Transform( particle );

    return transformed;
  }

  void MapPowerLawGenerator::setCoordinate(){}
  void MapPowerLawGenerator::buildPixelIntegral(){}
  int MapPowerLawGenerator::samplePixel(){}

} // namespace gramssky

#endif // if FITS_FOUND
