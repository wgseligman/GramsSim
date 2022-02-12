// Have the HEALPix libraries been found on this system?
#ifdef HEALPIX_INSTALLED

// MapEnergyBands.cc
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

#include "MapEnergyBands.h"
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
#include <numeric> // for std::accumulate
#include <memory>
#include <iostream>

namespace gramssky {

  MapEnergyBands::MapEnergyBands()
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
	std::cout << "MapEnergyBands: PDG code "
		  << m_PDG << " found in ROOT particle database, mass = "
		  << m_mass << std::endl;
      }
    }
    else {
      if (m_debug) {
	std::cout << "MapEnergyBands: PDG code "
		  << m_PDG << " not found in ROOT particle database! "
		  << " mass set to 0" << std::endl;
      }
    }

    // The parameters needed to read HEALPix maps from the input FITS
    // file.
    std::string healpixFile;
    options->GetOption("MapEnergyBandsFile",healpixFile);
    
    // HDU = "Header Data Unit". For more on what an HDU is, see
    // https://heasarc.gsfc.nasa.gov/fitsio/c/c_user/node21.html
    // I found this value experimentally, and it probably only applies to file
    // AliceSprings_Australia_2021_3_21_alt30000m_map_photon.fits

    int hduNumber;
    std::string numberOfMapsKey, mapPrefix;
    options->GetOption("MapEnergyBandsHDU",hduNumber);
    options->GetOption("MapNumberEnergyBandsKey",numberOfMapsKey);
    options->GetOption("MapEnergyBandsPrefix",mapPrefix);

    // Open the FITS file. (FITS is a file format used in astrophysics
    // to store images and multi-dimensional data.)
    auto input = std::make_shared<fitshandle>();
    input->open(healpixFile);
    input->goto_hdu(hduNumber);

    // Get the total number of energy-band maps.
    int numberInputMaps;
    input->get_key("NMAP", numberInputMaps);

    // For each map:
    for (int imap = 0; imap < numberInputMaps; ++imap) {
      double energy;
      input->get_key(mapPrefix + std::to_string(imap+1), energy);
    
      // If the map's energy is within the limits set in the options
      // XML file:
      if(m_energyMin <= energy && energy < m_energyMax) {

	// Create a new (empty) HEALPix map. We're using a pointer, to
	// reduce the amount of copying and memory use of all these
	// energy-band maps.
	auto differentialPhotonFluxMap = std::make_shared< Healpix_Map<double> >();

	// Read the map.
	read_Healpix_map_from_fits( *input, *differentialPhotonFluxMap, imap+1 ) ;

	// Add the map and its corresponding energy to our vectors.
	m_differentialPhotonFluxMap.push_back(differentialPhotonFluxMap);
	m_energyBand.push_back(energy);
      }
    }
    m_numberMaps = m_energyBand.size();

    // Number of pixels in the HEALPix maps. All the maps in the file
    // are assumed to be the same size.
    m_numPixels = (*m_differentialPhotonFluxMap[0]).Npix();

    // Calculate the integrals of the maps and pixels within the maps
    // for the rejection method of random-value generation.
    m_indexMap.resize(m_numberMaps - 1);
    m_integratedPhotonFluxMap.resize(m_numberMaps - 1);
    for (int imap = 0; imap < m_numberMaps - 1; ++imap) {
      calcIntegratedPhotonFluxInEnergyBand(*(m_differentialPhotonFluxMap[imap]), 
					   *(m_differentialPhotonFluxMap[imap+1]),
					   m_energyBand[imap], 
					   m_energyBand[imap+1], 
					   imap);
    }

    // Precompute pixel->(theta,phi).
    setCoordinate();

    // Create pixel integral for random-number generation.
    buildEnergyPixelIntegral();

    // Clean up.
    input->close();

  } // end constructor

  // Destructor
  MapEnergyBands::~MapEnergyBands() {}

  std::shared_ptr<ParticleInfo> MapEnergyBands::Generate()
  {
    // Create a new particle.
    auto particle = std::make_shared<ParticleInfo>();

    // Use the values from the Options XML file.
    particle->SetPDG(m_PDG);

    // Randomly pick an energy map.
    const int energyIndex = sampleEnergyBandIndex();
    // Randomly pick a pixel within that energy map.
    const int pixel = samplePixel(energyIndex);

    // Get the photon index (power-law exponent) from the current pixel. 
    const double pindex = m_indexMap[energyIndex][pixel]; 
    // Randomly generate energy from power-law distribution within this energy map. 
    const double energy = SampleFromPowerLaw( pindex, 
					      m_energyBand[energyIndex], 
					      m_energyBand[energyIndex+1] );

    // Get (theta,phi) from pixel coordinates.
    const double theta = m_imageDec[pixel];
    const double phi = m_imageRA[pixel];

    // Construct a unit vector for particle position with direction
    // from (theta,phi). The Transform method below will tranlate that
    // vector to the inner surface of the celestial sphere.
    particle->SetX( std::sin(theta) * std::cos(phi) );
    particle->SetY( std::sin(theta) * std::sin(phi) );
    particle->SetZ( std::cos(theta) );

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
  void MapEnergyBands::setCoordinate()
  {
    double z_ ;
    double phi_ ;

    m_imageRA.resize(m_numPixels);
    m_imageDec.resize(m_numPixels);

    for (int i = 0; i < m_numPixels; ++i) {
      // ang = ( theta in radian, phi = zenith angle)
      // z = cos(theta), phi = zenith
      (*m_differentialPhotonFluxMap[0]).pix2zphi( i, z_, phi_ );

      m_imageRA[i] = phi_;
      m_imageDec[i] = std::acos( z_ );
    }
  }

  // Create integral over the power-law function for random pixel
  // selection.

  void MapEnergyBands::buildEnergyPixelIntegral()
  {
    m_energyIntegral.resize(m_numberMaps);
    m_energyIntegral[0] = 0.0;
    for (int imap = 0; imap < m_numberMaps - 1; ++imap) {
      m_energyIntegral[imap+1] 
	= m_energyIntegral[imap] 
	+ std::accumulate(m_integratedPhotonFluxMap[imap].begin(), 
			  m_integratedPhotonFluxMap[imap].end(), 0.0);
    }
    const double norm = m_energyIntegral.back();
    for (auto& v: m_energyIntegral) {
      v /= norm;
    }

    m_pixelIntegral.resize(m_numberMaps - 1);
    for (int imap = 0; imap < m_numberMaps - 1; ++imap) {
      std::vector<double> this_m_pixelIntegral;
      this_m_pixelIntegral.resize(m_numPixels+1);
      this_m_pixelIntegral[0] = 0.0;
      for (int pixel = 0; pixel < m_numPixels; ++pixel) {
	this_m_pixelIntegral[pixel+1] 
	  = this_m_pixelIntegral[pixel] 
	  + m_integratedPhotonFluxMap[imap][pixel];
      }
      const double this_norm = this_m_pixelIntegral.back();
      for (auto& v: this_m_pixelIntegral) {
	v /= this_norm;
      }
      m_pixelIntegral[imap] = this_m_pixelIntegral;
    }
  }

  // Select a random energy map using the rejection method against the
  // energy maps.
  int MapEnergyBands::sampleEnergyBandIndex()
  {
    const double r = gRandom->Uniform();
    auto it = std::upper_bound(m_energyIntegral.cbegin(), 
			       m_energyIntegral.cend(), r);
    const int r0 = it - m_energyIntegral.cbegin() - 1;
    return r0 ;
  }

  // Select a random pixel using the rejection method against the
  // pixels within an energy map.
  int MapEnergyBands::samplePixel(int energyBandIndex)
  {
    const double r = gRandom->Uniform();
    auto it = std::upper_bound(m_pixelIntegral[energyBandIndex].cbegin(), 
			       m_pixelIntegral[energyBandIndex].cend(), r);
    const int r0 = it - m_pixelIntegral[energyBandIndex].cbegin() - 1;
    return r0 ;
  }

  void MapEnergyBands::calcIntegratedPhotonFluxInEnergyBand
  ( const Healpix_Map<double>& photonfluxmap1,
    const Healpix_Map<double>& photonfluxmap2,
    const double e1, 
    const double e2, 
    const int imap)
  {
    std::vector<double> indexMap;
    std::vector<double> integratedPhotonFluxMap;
    indexMap.resize(m_numPixels);
    integratedPhotonFluxMap.resize(m_numPixels);
    for(int pixel = 0; pixel < m_numPixels; ++pixel){
      double n1 = photonfluxmap1[pixel];
      double n2 = photonfluxmap2[pixel];

      double photonIndex_ = -1 * std::log(n2/n1) / std::log(e2/e1);
      double intg_1 = n1 * e1 / (-photonIndex_+1);
      double intg_2 = n1 
	* std::pow ( 1 / e1, (-1.0)*photonIndex_) 
	* std::pow (e2, -photonIndex_+1)/(-photonIndex_+1);
      double intg = intg_2 - intg_1;
    
      indexMap[pixel] = photonIndex_;
      integratedPhotonFluxMap[pixel] = intg;
    }
    m_indexMap[imap] = indexMap;
    m_integratedPhotonFluxMap[imap] = integratedPhotonFluxMap;
  }

} // namespace gramssky

#endif // if FITS_FOUND
