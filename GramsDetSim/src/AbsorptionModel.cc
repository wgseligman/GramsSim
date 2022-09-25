// 21-Sep-2022 Satoshi Takashima, William Seligman
// Implement a absorption model calculation.

#include "AbsorptionModel.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// C++ includes
#include <iostream>
#include <cmath>

namespace gramsdetsim {

  // Constructor: Initializes the class.
  AbsorptionModel::AbsorptionModel(TTreeReader* reader)
    : m_reader(reader)
  {
    // Get the options class. This contains all the program options
    // from options.xml and the command line.
    auto options = util::Options::GetInstance();

    // Fetch the options we want from the class. 

    options->GetOption("verbose",m_verbose);
    options->GetOption("debug",m_debug);

    options->GetOption("ElectronLifeTimeCorr",m_LifeTimeCorr_const);
    options->GetOption("ElectronDriftVelocity", m_DriftVel);
    options->GetOption("ReadoutPlaneCoord", m_readout_plane_coord);

    if (m_verbose) {
      std::cout << "gramsdetsim::AbsorptionModel - "
		<< " LifeTimeCorr_const= " << m_LifeTimeCorr_const << std::endl;
    }

    // These are the columns in the ntuple that we'll require for our
    // calculation. 

    // The names of the columns, and which columns are available, come
    // from GramsG4/src/GramsG4WriteNtuplesAction.cc.
    m_zStart = new TTreeReaderValue<Float_t>(*m_reader, "zStart");
    m_zEnd   = new TTreeReaderValue<Float_t>(*m_reader, "zEnd");

    m_RecipDriftVel = 1.0 / m_DriftVel;
  }

  // Destructor: clean up the class
  AbsorptionModel::~AbsorptionModel() {
    // Delete any pointers we created.
    delete m_zStart;
    delete m_zEnd;
  }

  // Note that the "a_" prefix is a convention to remind us that the
  // variable was an argument in this method.

  double AbsorptionModel::Calculate( double a_energy ) {

    // The TTreeReaderValue variables are "in sync" with the
    // TTreeReader. So as the main routine uses TTreeReader to go
    // through the ntuple, the TTreeReaderValue variables in this
    // program will automatically point to the current row within
    // TTreeReader.

    // This is a tricky part: Normally a TTreeReaderValue acts like a
    // pointer, e.g., if you have "TTreeReaderValue<double> energy"
    // then you refer to its value as "*energy". But in this routine,
    // m_zStart (for example) is a pointer itself, so to get its value
    // you need "**m_zStart" (a pointer to a pointer). 

    double z_mean = 0.5 * ( **m_zStart + **m_zEnd );

    double DriftDistance = m_readout_plane_coord - z_mean;
    double TDrift = std::abs(DriftDistance * m_RecipDriftVel);

    double effect = std::exp( -1.0 * TDrift / m_LifeTimeCorr_const);

    return a_energy *  effect;
  }

} // namespace gramsdetsim
