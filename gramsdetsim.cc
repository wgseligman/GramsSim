// gramsdetsim.cc
// Take the hit output from GramsG4 and apply detector-response effects.
// 25-Nov-2011 William Seligman

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// C++ includes
#include <exception>
#include <iostream>
#include <string>
#include <cmath>

// These are "global" variables that will be shared by both our model
// functions and the main routines.

// Recombination constants.
static double field;
static double a;
static double b;
static double rho;

// Recombination model function definition.
double RecombinationModel( double energy,
			   double xStart, 
			   double yStart, 
			   double zStart, 
			   double xEnd, 
			   double yEnd, 
			   double zEnd )
{
  // Takes input of the change in energy across the distance travelled
  // in a single step by the particle source for this equation is eqn
  // 2.4 in the 2013 paper "A study of electron recombination using
  // highly ionizing particles in the ArgoNeuT Liquid ArgonTPC". This
  // modified box model is used because it works for all ranges of
  // dE/dx as well as not having the technical difficulties that arise
  // when applying the birks model to highly ionizing particles.

  double dx = std::sqrt( std::pow(xStart-xEnd,2) + 
			 std::pow(yStart-yEnd,2) + 
			 std::pow(zStart-zEnd,2) );
  double dEdx = energy / dx;

  // The above variables are in Geant4 units (MeV, mm, ns). But the
  // recombination constants use cm.
  dEdx *= 10.;

  // Note that a, b, field, and rho are static constants defined
  // above. Their values are read in from options.xml in the main
  // routine below.

  // The following calculations are based off of the modified box
  // model used in the ICARUS experiment, with constant values taken
  // from the Brookhaven page on liquid argon TPCs. Be very specific in
  // where this equation is from (what paper), what it is finding,
  // what it is talking about.
  double sigma = (b * dEdx) / (field * rho);
  double effect = std::log(a + sigma) / sigma;

  // Note to Luke: Check the units! Check that "effect" is a
  // multiplication factor on dEdx! If it's not, fix my equation!
  return energy * effect * dx;
}



///////////////////////////////////////
int main(int argc,char **argv)
{
  // Initialize the options from the XML file and the
  // command line. Make sure this happens first!
  auto options = util::Options::GetInstance();

  // The third argument of ParseOptions is the name of the tag to use
  // for this program's options. See options.xml and/or util/README.md
  // to see how this works.
  auto result = options->ParseOptions(argc, argv, "gramsdetsim");

  // Abort if we couldn't parse the job options.
  if (result) std::cout << "ParseOptions succeeded" << std::endl;
  else {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "GramsDetSim: Aborting job due to failure to parse options"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // We may not be debugging this at this moment, but if we
  // want to use the debug flag later in this routine,
  // let's have it ready.
  bool debug;
  options->GetOption("debug",debug);

  // Check for help message.
  bool help;
  options->GetOption("help",help);
  if (help) {
    options->PrintHelp();
    exit(EXIT_SUCCESS);
  }

  // Get the options associated with the input file and ntuple..
  std::string inputFileName;
  options->GetOption("inputfile",inputFileName);
  
  std::string inputNtupleName;
  options->GetOption("inputntuple",inputNtupleName);

  // Open the input file. For historical reasons, ROOT methods can't
  // handle the type std::string, so we use the c_str() method to
  // convert the std::string into an old-style C string.
  auto input = TFile::Open(inputFileName.c_str());
  if (!input || input->IsZombie()) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "GramsDetSim: Could not open file '" << inputFileName << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // The standard way of reading a TTree (without using RDataFrame) in
  // C++ is using the TTreeReader.
  TTreeReader reader(inputNtupleName.c_str(), input);

  // Create a TTreeReaderValue for each column in the ntuple whose
  // value we'll use. Note that the variable we create must be
  // accessed as if it were a pointer; e.g., if you want the value of
  // "energy", you must use *energy in the code.
  TTreeReaderValue<Double_t> energy = {reader, "energy"};
  TTreeReaderValue<Float_t>  xStart = {reader, "xStart"};
  TTreeReaderValue<Float_t>  yStart = {reader, "yStart"};
  TTreeReaderValue<Float_t>  zStart = {reader, "zStart"};
  TTreeReaderValue<Float_t>  xEnd   = {reader, "xEnd"};
  TTreeReaderValue<Float_t>  yEnd   = {reader, "yEnd"};
  TTreeReaderValue<Float_t>  zEnd   = {reader, "zEnd"};

  // Now read in the options associated with the output file and ntuple. 
  std::string outputFileName;
  options->GetOption("outputfile",outputFileName);
  
  std::string outputNtupleName;
  options->GetOption("outputntuple",outputNtupleName);

  // Open the output file.
  auto output = TFile::Open(outputFileName.c_str(),"RECREATE");

  // Define the output ntuple.
  auto outputNtuple = new TTree(outputNtupleName.c_str(),"Detector Response");

  // Define the columns of the output ntuple. Since this ntuple will
  // be "friends" with the input ntuple, we only have to include
  // columns that are unique to the detector response.

  // In this case, we're giving the column in the friend ntuple the
  // same name as the column in the main ntuple. This means that we'd
  // have to qualify the column name with the name of the ntuple in
  // subsequent code; e.g., if the output ntuple name is "DetSim",
  // then this new column would be called "DetSim.energy".

  Double_t energyAtAnode;
  outputNtuple->Branch("energy",&energyAtAnode);

  // Get the options for the various models.
  bool doRecombination;
  options->GetOption("recombination",doRecombination);
  options->GetOption("field",field);
  options->GetOption("a",a);
  options->GetOption("b",b);
  options->GetOption("rho",rho);

  // For each row in the input ntuple:
  while ( reader.Next() ) {

    // Remember that given the TTreeReaderValue definitions above, a
    // variable read from the input ntuple must be accessed like a
    // pointer.
    energyAtAnode = *energy;

    // Apply the model(s).

    if ( doRecombination ) {
      energyAtAnode *= RecombinationModel( energyAtAnode,
	   *xStart, *yStart, *zStart, *xEnd, *yEnd, *zEnd );
    }

    // After all the model effects have been applied, write the
    // detector-response value(s).
    outputNtuple->Fill();

  }

  // Wrap-up.
  outputNtuple->Write();
  output->Close();
  input->Close();
}
