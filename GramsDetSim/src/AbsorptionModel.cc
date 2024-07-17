// 21-Sep-2022 Satoshi Takashima, William Seligman
// Implement a absorption model calculation.

#include "AbsorptionModel.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// From GramsDataObj
#include "MCLArHits.h"

// C++ includes
#include <iostream>
#include <cmath>

namespace gramsdetsim {

  // Constructor: Initializes the class.
  AbsorptionModel::AbsorptionModel()
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

    m_RecipDriftVel = 1.0 / m_DriftVel;
  }

  // Note that the "a_" prefix is a convention to remind us that the
  // variable was an argument in this method.

  double AbsorptionModel::Calculate( double a_energy, const grams::MCLArHit& hit ) {

    double z_mean = 0.5 * ( hit.StartZ() + hit.EndZ() );

    double DriftDistance = m_readout_plane_coord - z_mean;
    double TDrift = std::abs(DriftDistance * m_RecipDriftVel);

    double effect = std::exp( -1.0 * TDrift / m_LifeTimeCorr_const);

    if (m_debug) {
      std::cout << "GramsDetSim::AbsorptionModel  z_mean=" << z_mean
		<< " DriftDistance=" << DriftDistance
		<< " TDrift=" << TDrift
		<< " effect=" << effect
		<< std::endl;
    }
    
    return a_energy *  effect;
  }

} // namespace gramsdetsim
