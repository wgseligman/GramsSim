// 21-Sep-2022 Satoshi Takashima, Luke Zerrer, William Seligman
// Implement a recombination model calculation.

#include "RecombinationModel.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// C++ includes
#include <iostream>
#include <cmath>
#include <numeric>

namespace gramsdetsim {

  // Constructor: Initializes the class.
  RecombinationModel::RecombinationModel(TTreeReader* reader)
    : m_reader(reader)
  {
    // Get the options class. This contains all the program options
    // from options.xml and the command line.
    auto options = util::Options::GetInstance();

    // Fetch the options we want via the options class.. 
    options->GetOption("verbose",m_verbose);
    options->GetOption("debug",m_debug);

    options->GetOption("ElectricField",m_field);
    options->GetOption("box_alpha",m_alpha);
    options->GetOption("box_beta",m_beta);
    options->GetOption("LArDensity",m_rho);

    options->GetOption("RecombinationModel", m_recom_model);
    options->GetOption("birks_AB", m_A_B);
    options->GetOption("birks_kB", m_kB);

    if (m_verbose) {
      std::cout << "gramsdetsim::RecombinationModel - "
		<< "field= " << m_field
		<< " a= " << m_alpha
		<< " b= " << m_beta
		<< " rho= " << m_rho << std::endl;
    }

    // These are the columns in the ntuple that we'll require for our
    // calculation. 

    // The names of the columns, and which columns are available, come
    // from GramsG4/src/GramsG4WriteNtuplesAction.cc.
    m_xStart = new TTreeReaderValue<Float_t>(*m_reader, "xStart");
    m_yStart = new TTreeReaderValue<Float_t>(*m_reader, "yStart");
    m_zStart = new TTreeReaderValue<Float_t>(*m_reader, "zStart");
    m_xEnd   = new TTreeReaderValue<Float_t>(*m_reader, "xEnd");
    m_yEnd   = new TTreeReaderValue<Float_t>(*m_reader, "yEnd");
    m_zEnd   = new TTreeReaderValue<Float_t>(*m_reader, "zEnd");
  }

  // Destructor: clean up the class
  RecombinationModel::~RecombinationModel() {
    // Delete any pointers we created.
    delete m_xStart;
    delete m_yStart;
    delete m_zStart;
    delete m_xEnd;
    delete m_yEnd;
    delete m_zEnd;
  }

  // Note that the "a_" prefix is a convention to remind us that the
  // variable was an argument in this method.

  Double_t RecombinationModel::Calculate( double a_energy ) {

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

    // Physics: The following models come from "A study of electron
    // recombination using highly ionizing particles in the ArgoNeuT
    // Liquid Argon TPC", arXiv:1306.1712

    // The models take as input of the change in energy across the
    // distance traveled in a single step by the particle.
    double dx = std::sqrt( std::pow(**m_xStart - **m_xEnd, 2) + 
			   std::pow(**m_yStart - **m_yEnd, 2) + 
			   std::pow(**m_zStart - **m_zEnd, 2) );
    double dEdx = a_energy / dx;

    // It's possible for dx to be so small that the value of dEdx
    // becomes NaN (for "not a number"). If this happens, skip the
    // calculation.
    if ( std::isnan(dEdx) ) 
      return (0.0);

    // The following calculations are based off of the modified box
    // model used in the ICARUS experiment, with constant values taken
    // from the Brookhaven page on liquid argon TPCs.

    double effect = 1.0;

    // Calculate the effect based on our choice of recombination
    // model.
    switch (m_recom_model) {

    case 0 :
      // This modified box model works for all ranges of dE/dx as well
      // as not having the technical difficulties that arise when
      // applying the Birk's model to highly ionizing particles.
      {
	double effective_efield = (m_beta * dEdx) / (m_field * m_rho);
	effect = std::max(
			  std::log(m_alpha + effective_efield) / effective_efield, 
			  1.0e-6);
	
	if (m_debug)
	  std::cout << "gramsdetsim::RecombinationModel - "
		    << "dx= " << dx
		    << " dEdx= " << dEdx
		    << " effective_efield= " << effective_efield
		    << " effect= " << effect << std::endl;
      }
      break;

    case 1 :
      {
	effect = m_A_B / (1 + m_kB * dEdx / (m_field * m_rho));

	if (m_debug)
	  std::cout << "gramsdetsim::RecombinationModel - "
		    << "dx= " << dx
		    << " dEdx= " << dEdx
		    << " effect= " << effect << std::endl;
      }
      break;

    default :
      {
	std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		  << "gramsdetsim::RecombinationModel: Invalid value " << m_recom_model 
		  << " for recombination_model"
		  << std::endl;
	exit(EXIT_FAILURE);
      }
    } // switch on recombination model

  return a_energy * effect;
}

} // namespace gramsdetsim
