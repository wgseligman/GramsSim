// 26-Nov-2021 WGS 

// Implement a model of recombination in the LAr for the GRAMS
// detector.

#ifndef RecombinationModel_h
#define RecombinationModel_h

#include <vector>

// ROOT includes
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

namespace gramsdetsim {

  class RecombinationModel
  {
  public:

    // Constructor. It requires a pointer to the TTreeReader that's
    // used to read the ntuple.
    RecombinationModel( TTreeReader* );

    // Destructor. It's standard practice to declare all destructors
    // as virtual in C++, though it's only relevant if we were ever to
    // create a class that inherits from this one.
    virtual ~RecombinationModel();

    // The meat of this routine: Given the variables in the current
    // entry of the ntuple, calculate a revised energy.
    double Calculate(double energy);

  private:

    // Note that it's a convention to prefix variables defined in a
    // class header with "m_" as a visual clue in the ".cc" file that
    // the variable is defined in this ".h" file.

    // Used to read the ntuple.
    TTreeReader* m_reader;

    // These will point to the variables in the ntuple that we need
    // for this calculation.
    TTreeReaderValue<float>* m_xStart;
    TTreeReaderValue<float>* m_yStart;
    TTreeReaderValue<float>* m_zStart;
    TTreeReaderValue<float>* m_xEnd;
    TTreeReaderValue<float>* m_yEnd;
    TTreeReaderValue<float>* m_zEnd;

    // The constants required for the calculation. We'll get those as
    // program options.
    double m_field;
    double m_a;
    double m_b;
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
