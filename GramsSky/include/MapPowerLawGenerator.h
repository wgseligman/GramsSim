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

#ifndef Grams_MapPowerLawGenerator_h
#define Grams_MapPowerLawGenerator_h

#include "ParticleInfo.h"
#include "PositionGenerator.h"

// HEALPix and FITSIO includes
#include <healpix_cxx/healpix_base.h>
#include <healpix_cxx/healpix_map.h>
#include <healpix_cxx/healpix_map_fitsio.h>
#include <healpix_cxx/fitshandle.h>

// C++ includes
#include <vector>
#include <memory>

namespace gramssky {

  class MapPowerLawGenerator : public PositionGenerator
  {
  public:

    // Constructor. 
    MapPowerLawGenerator();

    // Destructor.
    virtual ~MapPowerLawGenerator();

    // Generate an event.
    virtual std::shared_ptr<ParticleInfo> Generate();

  private:
    // The PDG code and mass of the primary particle.
    int m_PDG;
    double m_mass;

    // Minimum and maximum energy limits.
    double m_energyMin;
    double m_energyMax;

    // Private routines required for random position generation.
    void setCoordinate();
    void buildPixelIntegral();
    int samplePixel();

    // Variables required for random position/energy generation from
    // the HEALPix maps. First, the maps for the power-law function
    // parameters.
    Healpix_Map<double> imageNorm_; // normalization map, 𝑁
    Healpix_Map<double> imageIndex_; // index map, 𝛼
    Healpix_Map<double> imageEnergyRef_; // reference energy map, 𝐸_𝑟𝑒𝑓

    // For random-value generation using the rejection method, we'll
    // need the integrals of the power-law function in both energy,
    // value, and position.
    std::vector<double> imageIntegratedPhotonFlux_; 
    std::vector<double> imageIntegratedEnergyFlux_; 
    std::vector<double> pixelIntegral_;

    int npix_; // Number of pixels in the HEALPix map

    // Pre-compute a conversion between pixel number and coordinates
    // on the celestial sphere.
    std::vector<double> imageRA_; // l in Galactic coordinate.
    std::vector<double> imageDec_; // b in Galactic coordinate.
  };

} // namespace gramssky

#endif // Grams_MapPowerLawGenerator_h
