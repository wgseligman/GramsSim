// 26-Nov-2021 WGS 

// Implement a model of diffusion in the LAr for the GRAMS
// detector.

#ifndef DiffusionModel_h
#define DiffusionModel_h

// From GramsDataObj
#include "MCLArHits.h"
#include "Clusters.h"

#include <vector>

namespace gramsdetsim {

  class DiffusionModel
  {
  public:

    // Constructor.
    DiffusionModel();

    // The meat of this routine: Given the variables in the current
    // entry of the ntuple, calculate a list of clusters.
    std::vector< grams::Cluster > Calculate(double energy, const grams::MCLArHit& hit);

  private:

    // Note that it's a convention to prefix variables defined in a
    // class header with "m_" as a visual clue in the ".cc" file that
    // the variable is defined in this ".h" file.

    // The constants required for the calculation. We'll get those as
    // program options.
    double m_MeVToElectrons;
    double m_LongitudinalDiffusion;
    double m_TransverseDiffusion;
    double m_readout_plane_coord;
    double m_DriftVel;
    double m_RecipDriftVel;
    int m_ElectronClusterSize;
    int m_MinNumberOfElCluster;

    // Save the verbose and debug options.
    bool m_verbose;
    bool m_debug;
  };

} // namespace gramsdetsim

#endif // DiffusionModel_h
