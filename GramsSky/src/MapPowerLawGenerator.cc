// Have the HEALPix libraries been found on this system?
#ifdef HEALPIX_INSTALLED

// MapPowerLawGenerator.cc
// 01-Feb-2022 WGS

// Based on code written by Naomi Tsuji and Hiroki Yoneda, within the
// ComptonSoft framework written by Hirokazu Odaka.

// Generate energy and position of primary particles based on HEALPix
// maps of the power-law functional parameters.

// In other words, assume a power-law functional form of:

// 𝑓(𝐸) = 𝑁 * ( 𝐸 / 𝐸_𝑟𝑒𝑓 )^(−𝛼)

// The input file will have three HEALPix maps, one each for 𝑁, 𝐸_𝑟𝑒𝑓,
// and 𝛼. Each map will contain its corresponding parameter as a
// function of (z,phi) on the inside of the celestial sphere.

// This program will generate the position randomly on the celestrial
// sphere, then randomly generate the energy according to above
// functional form.

#include "MapPowerLawGenerator.h"
#include "PositionGenerator.h" // For GetTransform
#include "TransformCoordinates.h"
#include "SampleFromPowerLaw.h"
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

    // Flags from Options.
    options->GetOption("debug",m_debug);
    options->GetOption("verbose",m_verbose);

    // Power-law function energy limits.
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
      if (m_debug) {
	std::cout << "MapPowerLawGenerator: PDG code "
		  << m_PDG << " found in ROOT particle database, mass = "
		  << m_mass << std::endl;
      }
    }
    else {
      if (m_debug) {
	std::cout << "MapPowerLawGenerator: PDG code "
		  << m_PDG << " not found in ROOT particle database! "
		  << " mass set to 0" << std::endl;
      }
    }

    // The parameters needed to read HEALPix maps from the input FITS
    // file.
    std::string healpixFile;
    options->GetOption("MapPowerLawFile",healpixFile);
    
    // HDU = "Header Data Unit". For more on what an HDU is, see
    // https://heasarc.gsfc.nasa.gov/fitsio/c/c_user/node21.html
    // I found this value experimentally, and it probably only applies to file
    // AliceSprings_Australia_2021_3_21_alt30000m_powerlaw_photon.fits

    int hduNumber, columnNorm, columnIndex, columnEref;
    options->GetOption("MapPowerLawHDU",hduNumber);
    options->GetOption("MapPowerLawColumnNorm",columnNorm);
    options->GetOption("MapPowerLawColumnIndex",columnIndex);
    options->GetOption("MapPowerLawColumnEref",columnEref);

    // Open the FITS file. (FITS is a file format used in astrophysics
    // to store images and multi-dimensional data.)
    auto input = std::make_shared<fitshandle>();
    input->open(healpixFile);
    input->goto_hdu(hduNumber);
    
    // Read the maps. Each map is stored in its own column within a
    // data table identified by the HDU.
    read_Healpix_map_from_fits( *input, m_imageNorm, columnNorm ) ;
    read_Healpix_map_from_fits( *input, m_imageIndex, columnIndex ) ;
    read_Healpix_map_from_fits( *input, m_imageEnergyRef, columnEref ) ;

    // Number of pixels in the HEALPix maps. All the maps in the file
    // are assumed to be the same size. .
    m_npix = m_imageNorm.Npix();

    // Integrate over position and energy for random-number
    // generation.
    m_imageIntegratedPhotonFlux.resize( m_npix );
    m_imageIntegratedEnergyFlux.resize( m_npix );
    for(int ipix = 0; ipix < m_npix; ++ipix){
      m_imageIntegratedPhotonFlux[ipix] 
	= m_calcIntegratedPhotonFlux( m_imageNorm[ipix], 
				      m_imageIndex[ipix], 
				      m_imageEnergyRef[ipix] , 
				      m_energyMin, 
				      m_energyMax ) ;
      m_imageIntegratedEnergyFlux[ipix] 
	= m_calcIntegratedEnergyFlux( m_imageNorm[ipix], 
				      m_imageIndex[ipix], 
				      m_imageEnergyRef[ipix] , 
				      m_energyMin, 
				      m_energyMax ) ;
    } // loop over pixels

    // Precompute pixel->(theta,phi).
    m_setCoordinate();

    // Create pixel integral for random-number generation.
    m_buildPixelIntegral();

    // Clean up.
    input->close();

  } // end constructor

  // Destructor
  MapPowerLawGenerator::~MapPowerLawGenerator() {}

  std::shared_ptr<ParticleInfo> MapPowerLawGenerator::Generate()
  {
    // Create a new particle.
    auto particle = std::make_shared<ParticleInfo>();

    // Use the values from the Options XML file.
    particle->SetPDG(m_PDG);

    // Pick a random pixel from the HEALPix map.
    const double r = gRandom->Uniform();
    auto it = std::upper_bound(m_pixelIntegral.cbegin(), m_pixelIntegral.cend(), r);
    const int pixel = it - m_pixelIntegral.cbegin() - 1;

    // Get (theta,phi) from pixel coordinates.
    const double theta = m_imageTheta[pixel];
    const double phi = m_imagePhi[pixel];

    // Construct a unit vector for particle position with direction
    // from (theta,phi). The Transform method below will tranlate that
    // vector to the inner surface of the celestial sphere.
    particle->SetX( std::sin(theta) * std::cos(phi) );
    particle->SetY( std::sin(theta) * std::sin(phi) );
    particle->SetZ( std::cos(theta) );

    // Generate energy randomly from power-law distribution.
    double energy = SampleFromPowerLaw( m_imageIndex[pixel], m_energyMin, m_energyMax );
    particle->SetE(energy);

    // We don't have to set the correct particle direction here, since
    // the Transform routine will rotate the momentum vector to point
    // in the direction of the detector's center. But we do want to
    // get the magnitude right.
    double momentum = std::sqrt( energy*energy - m_mass*m_mass );
    particle->SetPz(-momentum);

    // Transform the particle position and direction from the
    // celestial sphere to the detector coordinates, applying a random
    // shift to simulate a flux from infinity.
    auto transformed = GetTransform()->Transform( particle );

    return transformed;
  }

  // Pre-compute the translation from pixel number to (theta,phi)
  void MapPowerLawGenerator::m_setCoordinate()
  {
    double z_ ;
    double phi_ ;

    m_imagePhi.resize(m_npix);
    m_imageTheta.resize(m_npix);

    for(int i = 0; i < m_npix; ++i){
      // ang = ( theta in radian, phi = zenith angle)
      // z = cos(theta), phi = zenith
      m_imageNorm.pix2zphi( i, z_, phi_ );

      m_imagePhi[i] = phi_;
      m_imageTheta[i] = std::acos( z_ );
    }
  }

  // Create integral over the power-law function for random pixel
  // selection.
  void MapPowerLawGenerator::m_buildPixelIntegral()
  {
    m_pixelIntegral.resize(m_npix+1);
    m_pixelIntegral[0] = 0.0;
    for(int ipix = 0; ipix < m_npix; ++ipix){
      m_pixelIntegral[ipix+1] = m_pixelIntegral[ipix] + m_imageIntegratedPhotonFlux[ipix];
    }
    const double norm = m_pixelIntegral.back();
    for (auto& v: m_pixelIntegral) {
      v /= norm;
    }
  }

  // Calculate the integrals of the power-law function for each pixel. 

  double MapPowerLawGenerator::m_calcIntegratedEnergyFlux
  ( double norm_, 
    double photonIndex_, 
    double energy, 
    double E_min, 
    double E_max ) 
  {
    double intg_1 ;
    double intg_2 ;
    double intg_ ;
    if ( (1.999 < photonIndex_) && (photonIndex_ < 2.001) ){
      intg_1 = norm_ * std::pow ( 1 / energy , (-1.0)*photonIndex_) * std::log( E_min );
      intg_2 = norm_ * std::pow ( 1 / energy , (-1.0)*photonIndex_) * std::log( E_max );
    }
    else{
      intg_1 = norm_ * std::pow ( 1 / energy , (-1.0)*photonIndex_) * std::pow (E_min, -photonIndex_+2)/(-photonIndex_+2) ;
      intg_2 = norm_ * std::pow ( 1 / energy , (-1.0)*photonIndex_) * std::pow (E_max, -photonIndex_+2)/(-photonIndex_+2) ;
    }
    intg_ = intg_2 - intg_1 ;
    return intg_ ; 
  }

  double MapPowerLawGenerator::m_calcIntegratedPhotonFlux
  ( double norm_, 
    double photonIndex_, 
    double energy, 
    double E_min, 
    double E_max ) 
  {
    double intg_1 ;
    double intg_2 ;
    double intg_ ;
    if ( (0.999 < photonIndex_) && (photonIndex_ < 1.001) ){
      intg_1 = norm_ * std::pow ( 1 / energy , (-1.0)*photonIndex_) * std::log( E_min );
      intg_2 = norm_ * std::pow ( 1 / energy , (-1.0)*photonIndex_) * std::log( E_max );
    }
    else{
      intg_1 = norm_ * std::pow ( 1 / energy , (-1.0)*photonIndex_) * std::pow (E_min, -photonIndex_+1)/(-photonIndex_+1) ;
      intg_2 = norm_ * std::pow ( 1 / energy , (-1.0)*photonIndex_) * std::pow (E_max, -photonIndex_+1)/(-photonIndex_+1) ;
    }
    intg_ = intg_2 - intg_1 ;
    return intg_ ; 
  }

} // namespace gramssky

#endif // if FITS_FOUND
