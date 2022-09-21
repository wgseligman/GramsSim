// 26-Nov-2021 WGS

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

    options->GetOption("LifeTimeCorr_const",m_LifeTimeCorr_const);
    options->GetOption("DriftVel", m_DriftVel);
    options->GetOption("pixel_plane_offset", m_pixel_plane_offset);

    if (m_verbose) {
      std::cout << "gramsdetsim::AbsorptionModel - "
		<< " LifeTimeCorr_const= " << m_LifeTimeCorr_const << std::endl;
    }

    // These are the columns in the ntuple that we'll require for our
    // calculation. Note: I may have misunderstood Luke's code; if we
    // don't have to calculate dEdx then these are not needed.

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
    // m_xStart (for example) is a pointer itself, so to get its value
    // you need "**m_xStart" (a pointer to a pointer). 

    // Takes input of the change in energy across the distance travelled
    // in a single step by the particle source for this equation is eqn
    // 2.4 in the 2013 paper "A study of electron recombination using
    // highly ionizing particles in the ArgoNeuT Liquid ArgonTPC". This
    // modified box model is used because it works for all ranges of
    // dE/dx as well as not having the technical difficulties that arise
    // when applying the birks model to highly ionizing particles.


    double z_mean = 0.5 * (**m_zStart + ** m_zEnd);
    //A/I z_offset should be defined later;
    double DriftDistance = m_pixel_plane_offset - z_mean;
    double TDrift = std::abs(DriftDistance * m_RecipDriftVel);

    double effect = std::exp( -1.0 * TDrift / m_LifeTimeCorr_const);

    return a_energy *  effect;
  }

} // namespace gramsdetsim
