// 26-Nov-2021 WGS 

// Implement a model of absorption in the LAr for the GRAMS
// detector.

#ifndef AbsorptionModel_h
#define AbsorptionModel_h

// From GramsDataObj
#include "MCLArHits.h"

namespace gramsdetsim {

  class AbsorptionModel
  {
  public:

    // Constructor. It requires a pointer to the TTreeReader that's
    // used to read the ntuple.
    AbsorptionModel();

    // The meat of this routine: Given the variables in the current
    // entry of the ntuple, calculate a revised energy.
    double Calculate(double energy, const grams::MCLArHit& hit);

  private:

    // Note that it's a convention to prefix variables defined in a
    // class header with "m_" as a visual clue in the ".cc" file that
    // the variable is defined in this ".h" file.

    // The constants required for the calculation. We'll get those as
    // program options.
    double m_LifeTimeCorr_const;
    double m_DriftVel;
    double m_RecipDriftVel;
    double m_readout_plane_coord;

    // Save the verbose and debug options.
    bool m_verbose;
    bool m_debug;
  };

} // namespace gramsdetsim

#endif // AbsorptionModel_h
