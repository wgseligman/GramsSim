// gramsdetsim.cc
// Take the hit output from GramsG4 and apply detector-response effects.
// 25-Nov-2011 William Seligman

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TFile.h"
#include "TString.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// C++ includes
#include <exception>
#include <iostream>
#include <string>

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
  std::string inputFile;
  options->GetOption("inputfile",inputFile);
  
  std::string inputNtuple;
  options->GetOption("inputntuple",inputNtuple);

  // Open the input file. For historical reasons, ROOT methods can't
  // handle the type std::string, so we use the c_str() method to
  // convert the std::string into an old-style C string.
  auto input = TFile::Open(inputFile.c_str());
  if (!input || input->IsZombie()) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "GramsDetSim: Could not open file '" << inputFile << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // The standard way of reading a TTree (without using RDataFrame) in
  // C++ is using the TTreeReader.
  TTreeReader reader(inputNtuple.c_str(), input);

  // Create a TTreeReaderValue for each column in the ntuple whose
  // value we'll use. Note that the variable we create must be
  // accessed as if it were a pointer; e.g., if you want the value of
  // "energy", you must use *energy in the code.
  TTreeReaderValue<Double_t> energy = {reader, "energy"};

  // Now read in the options associated with the output file and ntuple. 
  std::string outputFile;
  options->GetOption("outputfile",outputFile);
  
  std::string outputNtuple;
  options->GetOption("outputntuple",outputNtuple);

  // Open the output file.
  auto output = TFile::Open(outputFile.c_str(),"RECREATE");

  // Define the output ntuple.
  auto ntuple = new TTree(outputNtuple.c_str(),"Detector Response");

  // Define the columns of the output ntuple. Since this ntuple will
  // be "friends" with the input ntuple, we only have to include
  // columns that are unique to the detector response.

  // In this case, we're giving the column in the friend ntuple the
  // same name as the column in the main ntuple. This means that we'd
  // have to qualify the column name with the name of the ntuple in
  // subsequent code; e.g., if the output ntuple name is "DetSim",
  // then this new column would be called "DetSim.energy".

  Double_t energyAtAnode;
  ntuple->Branch("energy",&energyAtAnode);

  // For each row in the input ntuple:
  while ( reader.Next() ) {
    energyAtAnode = *energy;
    ntuple->Fill();
  }

  // Wrap-up.
  ntuple->Write();
  output->Close();
  input->Close();
}
