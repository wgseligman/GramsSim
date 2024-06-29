// 21-Sep-2022 Satoshi Takashima, Luke Zerrer, William Seligman
// Implement a model of recombination in the LAr for the GRAMS
// detector.

#ifndef RecombinationModel_h
#define RecombinationModel_h

// From GramsDataObj
#include "MCLArHits.h"

#include <vector>

namespace gramsdetsim {

  class RecombinationModel
  {
  public:

    // Constructor.
    RecombinationModel();

    // The meat of this routine: Given the variables in the current
    // entry of the ntuple, calculate a revised energy.
    double Calculate(double energy, const grams::MCLArHit& hit);

  private:

    // Note that it's a convention to prefix variables defined in a
    // class header with "m_" as a visual clue in the ".cc" file that
    // the variable is defined in this ".h" file.

    // The constants required for the calculation. We'll get those as
    // program options.
    double m_field;
    double m_alpha;
    double m_beta;
    double m_rho;

    int m_recom_model;
    double m_A_B;
    double m_kB;

    // Save the verbose and debug options.
    bool m_verbose;
    bool m_debug;
  };

} // namespace gramsdetsim

#endif // RecombinationModel_h
