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
    // https://heasarc.gsfc.nasa.gov/docs/software/fitsio/user_f/node17.html
    // I found this value experimentally, and it probably only applies to file
    // AliceSprings_Australia_2021_3_21_alt30000m_map_photon.fits

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
    read_Healpix_map_from_fits( *input, imageNorm_, columnNorm ) ;
    read_Healpix_map_from_fits( *input, imageIndex_, columnIndex ) ;
    read_Healpix_map_from_fits( *input, imageEnergyRef_, columnEref ) ;

    // Number of pixels in the HEALPix maps. All the maps in the file
    // are assumed to be the same size. .
    npix_ = imageNorm_.Npix();

    // Integrate over position and energy for random-number
    // generation.
    imageIntegratedPhotonFlux_.resize( npix_ );
    imageIntegratedEnergyFlux_.resize( npix_ );
    for(int ipix = 0; ipix < npix_; ++ipix){
      imageIntegratedPhotonFlux_[ipix] 
	= calcIntegratedPhotonFlux( imageNorm_[ipix], 
				    imageIndex_[ipix], 
				    imageEnergyRef_[ipix] , 
				    m_energyMin, 
				    m_energyMax ) ;
      imageIntegratedEnergyFlux_[ipix] 
	= calcIntegratedEnergyFlux( imageNorm_[ipix], 
				    imageIndex_[ipix], 
				    imageEnergyRef_[ipix] , 
				    m_energyMin, 
				    m_energyMax ) ;
    } // loop over pixels

    // Precompute pixel->(theta,phi).
    setCoordinate();

    // Create pixel integral for random-number generation.
    buildPixelIntegral();

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
    auto it = std::upper_bound(pixelIntegral_.cbegin(), pixelIntegral_.cend(), r);
    const int pixel = it - pixelIntegral_.cbegin() - 1;

    // Get (theta,phi) from pixel coordinates.
    const double theta = imageDec_[pixel];
    const double phi = imageRA_[pixel];

    // Construct a unit vector for particle position with direction
    // from (theta,phi). The Transform method below will tranlate that
    // vector to the inner surface of the celestial sphere.
    particle->SetX( std::sin(theta) * std::cos(phi) );
    particle->SetY( std::sin(theta) * std::sin(phi) );
    particle->SetZ( std::cos(theta) );

    // Generate energy randomly from power-law distribution.
    double energy = SampleFromPowerLaw( imageIndex_[pixel], m_energyMin, m_energyMax );
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
  void MapPowerLawGenerator::setCoordinate()
  {
    double z_ ;
    double phi_ ;

    imageRA_.resize(npix_);
    imageDec_.resize(npix_);

    for(int i = 0; i < npix_; ++i){
      // ang = ( theta in radian, phi = zenith angle)
      // z = cos(theta), phi = zenith
      imageNorm_.pix2zphi( i, z_, phi_ );

      imageRA_[i] = phi_;
      imageDec_[i] = std::acos( z_ );
    }
  }

  // Create integral over the power-law function for random pixel
  // selection.
  void MapPowerLawGenerator::buildPixelIntegral()
  {
    pixelIntegral_.resize(npix_+1);
    pixelIntegral_[0] = 0.0;
    for(int ipix = 0; ipix < npix_; ++ipix){
      pixelIntegral_[ipix+1] = pixelIntegral_[ipix] + imageIntegratedPhotonFlux_[ipix];
    }
    const double norm = pixelIntegral_.back();
    for (auto& v: pixelIntegral_) {
      v /= norm;
    }
  }

  // Calculate the integrals of the power-law function for each pixel. 

  double MapPowerLawGenerator::calcIntegratedEnergyFlux( double norm_, 
							 double photonIndex_, 
							 double energy, 
							 double E_min, 
							 double E_max ) {
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

  double MapPowerLawGenerator::calcIntegratedPhotonFlux( double norm_, double photonIndex_, double energy, double E_min, double E_max ) {
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
