// gramsdetsim.cc
// Take the hit output from GramsG4 and apply detector-response effects.
// 25-Nov-2011 William Seligman

// Our function(s) for the detector response.
#include "RecombinationModel.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TFile.h"
#include "TTreeReader.h"

// C++ includes
#include <iostream>
#include <string>
#include <cmath>

///////////////////////////////////////
int main(int argc,char **argv)
{
  // Initialize the options from the XML file and the
  // command line. Make sure this happens first!
  auto options = util::Options::GetInstance();

  // Here, we omit the third argument to ParseOptions as an example:
  // the ParseOptions method will look for the program name in argv[0]
  // and search for the tag-block in the XML file with that name for
  // this program's options. See options.xml and/or util/README.md to
  // see how this works.
  auto result = options->ParseOptions(argc, argv);

  // Abort if we couldn't parse the job options.
  if (! result) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramsdetsim: Aborting job due to failure to parse options"
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

  bool verbose;
  options->GetOption("verbose",verbose);
  if (verbose) {
    // Display all program options.
    options->PrintOptions();
  }

  // Get the options associated with the input file and ntuple..
  std::string inputFileName;
  options->GetOption("inputfile",inputFileName);
  
  std::string inputNtupleName;
  options->GetOption("inputntuple",inputNtupleName);

  if (verbose)
    std::cout << "gramsdetsim: input file = '" << inputFileName
	      << "', input ntuple = '" << inputNtupleName
	      << "'" << std::endl;

  // Open the input file. For historical reasons, ROOT methods can't
  // handle the type std::string, so we use the c_str() method to
  // convert the std::string into an old-style C string.
  auto input = TFile::Open(inputFileName.c_str());
  if (!input || input->IsZombie()) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramsdetsim: Could not open file '" << inputFileName << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // The standard way of reading a TTree (without using RDataFrame) in
  // C++ is using the TTreeReader.
  auto reader = new TTreeReader(inputNtupleName.c_str(), input);

  // Create a TTreeReaderValue for each column in the ntuple whose
  // value we'll use. Note that the variable we create must be
  // accessed as if it were a pointer; e.g., if you want the value of
  // "energy", you must use *energy in the code.
  TTreeReaderValue<Double_t> energy = {*reader, "energy"};

  // Now read in the options associated with the output file and ntuple. 
  std::string outputFileName;
  options->GetOption("outputfile",outputFileName);
  
  std::string outputNtupleName;
  options->GetOption("outputntuple",outputNtupleName);

  if (verbose)
    std::cout << "gramsdetsim: output file = '" << outputFileName
	      << "', output ntuple = '" << outputNtupleName
	      << "'" << std::endl;

  // Open the output file.
  auto output = TFile::Open(outputFileName.c_str(),"RECREATE");

  // Write the options to the output file, so we have a record.
  options->WriteNtuple(output);

  // Define our output ntuple.
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

  // Are we using this particular model?
  bool doRecombination;
  options->GetOption("recombination",doRecombination);

  // If we're using this model, initialize it. 
  gramsdetsim::RecombinationModel* recombinationModel = NULL;
  if ( doRecombination ) {
    recombinationModel = new gramsdetsim::RecombinationModel(reader);
    if (verbose)
      std::cout << "gramsdetsim: RecombinationModel turned on" << std::endl;
  }
  else {
    if (verbose)
      std::cout << "gramsdetsim: RecombinationModel turned off" << std::endl;
  }

  // For each row in the input ntuple:
  while ( (*reader).Next() ) {

    if (debug)
      std::cout << "gramsdetsim: at entry " << reader->GetCurrentEntry() << std::endl;

    // Start with the approximation that the energy at the anode will
    // be the same as the hit energy, then apply corrections to it.

    // Remember that given the TTreeReaderValue definitions above, a
    // variable read from the input ntuple must be accessed like a
    // pointer.
    energyAtAnode = *energy;

    if (debug)
      std::cout << "gramsdetsim: before model corrections, energyAtAnode=" 
		<< energyAtAnode << std::endl;

    // Apply the model(s).

    // Note to Luke: Check that this how the result of this
    // calculation should be applied.
    if ( doRecombination ) {
      energyAtAnode *= recombinationModel->Calculate(energyAtAnode);
    }

    if (debug)
      std::cout << "gramsdetsim: after model corrections, energyAtAnode=" 
		<< energyAtAnode << std::endl;

    // After all the model effects have been applied, write the
    // detector-response value(s).
    outputNtuple->Fill();
  }

  // Wrap-up. Close all files. Delete any pointers we created.
  outputNtuple->Write();
  output->Close();
  delete recombinationModel;
  delete reader;
  input->Close();
}
