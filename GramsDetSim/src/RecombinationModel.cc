// 26-Nov-2021 WGS

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

namespace gramsdetsim {

  // Constructor: Initializes the class.
  RecombinationModel::RecombinationModel(TTreeReader* reader)
    : m_reader(reader)
  {
    // Get the options class. This contains all the program options
    // from options.xml and the command line.
    auto options = util::Options::GetInstance();

    // Fetch the options we want from the class. 

    options->GetOption("verbose",m_verbose);
    options->GetOption("debug",m_debug);

    options->GetOption("field",m_field);
    options->GetOption("a",m_a);
    options->GetOption("b",m_b);
    options->GetOption("rho",m_rho);

    if (m_verbose) {
      std::cout << "gramsdetsim::RecombinationModel - "
		<< "field= " << m_field
		<< " a= " << m_a
		<< " b= " << m_b
		<< " rho= " << m_rho << std::endl;
    }

    // These are the columns in the ntuple that we'll require for our
    // calculation. Note: I may have misunderstood Luke's code; if we
    // don't have to calculate dEdx then these are not needed.

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

  double RecombinationModel::Calculate( double a_energy ) {

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

    double dx = std::sqrt( std::pow(**m_xStart - **m_xEnd, 2) + 
			   std::pow(**m_yStart - **m_yEnd, 2) + 
			   std::pow(**m_zStart - **m_zEnd, 2) );
    double dEdx = a_energy / dx;

    // The above variables are in Geant4 units (MeV, mm, ns). But the
    // recombination constants use cm.
    dEdx *= 10.;
    
    // The following calculations are based off of the modified box
    // model used in the ICARUS experiment, with constant values taken
    // from the Brookhaven page on liquid argon TPCs. Be very specific in
    // where this equation is from (what paper), what it is finding,
    // what it is talking about.
    double sigma = (m_b * dEdx) / (m_field * m_rho);
    double effect = std::log(m_a + sigma) / sigma;

    if (m_debug)
      std::cout << "gramsdetsim::RecombinationModel - "
		<< "dx= " << dx
		<< " dEdx= " << dEdx
		<< " sigma= " << sigma
		<< " effect= " << effect << std::endl;

    // Note to Luke: Check the units! Check that "effect" is a
    // multiplication factor on the energy! If it's not, fix my
    // equation!
    return a_energy * effect;
  }

} // namespace gramsdetsim
