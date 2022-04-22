// MapEnergyBands.h
// 01-Feb-2022 WGS 

// See comments in GramsSky/src/MapEnergyBands.cc

#ifndef Grams_MapEnergyBands_h
#define Grams_MapEnergyBands_h

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

  class MapEnergyBands : public PositionGenerator
  {
  public:

    // Constructor. 
    MapEnergyBands();

    // Destructor.
    virtual ~MapEnergyBands();

    // Generate an event.
    virtual std::shared_ptr<ParticleInfo> Generate();

  private:
    // Potential flags from the command line.
    bool m_verbose;
    bool m_debug;

    // The PDG code and mass of the primary particle.
    int m_PDG;
    double m_mass;

    // Minimum and maximum energy limits.
    double m_energyMin;
    double m_energyMax;

    // Private routines required for random position generation and
    // map selection.
    void m_setCoordinate();
    void m_buildEnergyPixelIntegral();
    int m_sampleEnergyBandIndex();
    int m_samplePixel(int energyBandIndex);
    void m_calcIntegratedPhotonFluxInEnergyBand( const Healpix_Map<double>& photonfluxmap1, 
						 const Healpix_Map<double>& photonfluxmap2, 
						 const double e1, 
						 const double e2, 
						 const int imap);

    // Variables required for random map selection. 
    // To save on memory, make this a vector of pointers.
    std::vector< std::shared_ptr<Healpix_Map<double> > > m_differentialPhotonFluxMap;
    std::vector< double > m_energyBand; // normalization map

    std::vector< std::vector<double> > m_indexMap;
    std::vector< std::vector<double> > m_integratedPhotonFluxMap;

    std::vector<double> m_energyIntegral;
    std::vector< std::vector<double> > m_pixelIntegral;

    int m_numberMaps; // Number of maps in the input file that pass the energy cuts
    int m_numPixels; // Number of pixels in the HEALPix maps

    // Pre-compute a conversion between pixel number and coordinates
    // on the celestial sphere.
    std::vector<double> m_imagePhi; // l in Galactic coordinate.
    std::vector<double> m_imageTheta; // b in Galactic coordinate.
  };

} // namespace gramssky

#endif // Grams_MapEnergyBands_h
