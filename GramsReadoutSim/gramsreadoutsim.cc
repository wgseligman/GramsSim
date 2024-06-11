// gramsreadoutsim.cc
// Take the charge-cluster results from GramsDetSim 
// and map their positions to the GRAMS readout.
// 25-Oct-2022 Satoshi Takashima
// 11-Jun-2024 William Seligman - revised for tree-based I/O

// The model of the readout geometry:
#include "AssignPixelID.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// For copying and accessing the detectory geometry.
#include "Geometry.h" // in util/

// From GramsDataObj:
#include "EventID.h"
#include "ReadoutID.h"
#include "ReadoutMap.h"
#include "ElectronClusters.h"

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// C++ includes
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <memory>

///////////////////////////////////////
int main(int argc,char **argv)
{
  // Access the options in the XML file and the command line. 
  auto options = util::Options::GetInstance();
  auto result = options->ParseOptions(argc, argv, "gramsreadoutsim");

  // Abort if we couldn't parse the job options.
  if (! result) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramsreadoutsim: Aborting job due to failure to parse options"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  bool debug;
  options->GetOption("debug",debug);

  bool help;
  options->GetOption("help",help);
  if (help) {
    options->PrintHelp();
    exit(EXIT_SUCCESS);
  }

  bool verbose;
  options->GetOption("verbose",verbose);
  if (verbose || debug) {
    // Display all program options.
    options->PrintOptions();
  }

  // Get the options associated with the input file and tree.
  std::string inputFileName;
  options->GetOption("inputReadoutFile",inputFileName);
    
  std::string inputTreeName;
  options->GetOption("inputReadoutTree",inputTreeName);

  if (verbose)
    std::cout << "gramsreadoutsim: input file = '" << inputFileName
	      << "', input tree = '" << inputTreeName
	      << "'" << std::endl;

  // Set up the readout ID algorithm. 
  auto assignPixelID = std::make_shared<gramsreadoutsim::AssignPixelID>(gramsreadoutsim::AssignPixelID());

  auto input = TFile::Open(inputFileName.c_str());
  if (!input || input->IsZombie()) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramsreadoutsim: Could not open file '" << inputFileName << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // Copy any options saved in the input file. This allows us to
  // track the history of how the files are created.
  options->CopyInputNtuple(input);

  // The standard way of reading a TTree (without using RDataFrame) in
  // C++ is using the TTreeReader.
  auto reader = new TTreeReader(inputTreeName.c_str(), input);

  // Create a TTreeReaderValue for each column in the tree whose
  // value we'll use.
  TTreeReaderValue<grams::EventID> inputEventID      = {*reader, "EventID"};
  TTreeReaderValue<grams::ElectronClusters> clusters = {*reader, "ElectronClusters"};

  // Now read in the options associated with the output file and tree. 
  std::string outputFileName;
  options->GetOption("outputReadoutFile", outputFileName);

  std::string outputTreeName;
  options->GetOption("outputReadoutTree", outputTreeName);

  if (verbose)
    std::cout << "gramsreadoutsim: output file = '" << outputFileName
	      << "', output tree = '" << outputTreeName
	      << "'" << std::endl;

  // Open the output file.
  auto output = TFile::Open(outputFileName.c_str(),"RECREATE");
  // Write the options to the output file, so we have a record.
  options->WriteNtuple(output);

  // Copy the detector geometry from the input file to the output
  // file.
  auto geometry = util::Geometry::GetInstance();
  if (debug)
    std::cout << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramsreadoutsim: About to copy geometry"
	      << std::endl;
  geometry->CopyGeometry(input,output);
  if (debug)
    std::cout << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramsreadoutsim: geometry copied"
	      << std::endl;

  // Define our output tree.
  TTree* outputTree = new TTree(outputTreeName.c_str(), "ReadoutSim");

  // Define the columns of the output tree. Since this tree will
  // be "friends" with the input tree, we only have to include
  // columns that are unique to the detector response.

  // The exception is "EventID', which we duplicate between trees, to
  // make it easier for them to be friends.

  auto eventID = new grams::EventID();
  auto readoutMap = new grams::ReadoutMap();
  outputTree->Branch("EventID",    &eventID);
  outputTree->Branch("ReadoutMap", &readoutMap);

  if (verbose)
    std::cout << "gramsreadoutsim: output tree defined" << std::endl;

  // For each row in the input tree:
  while ( (*reader).Next() ) {

    // Copy the event ID from one tree to another.
    (*eventID) = (*inputEventID);
    
    // Clean out any readout data from the previous event.
    readoutMap->clear();
    
    for ( const auto& [ ckey, cluster ] : (*clusters) ) {

      // Assign a pixel readout ID to each cluster.
      const grams::ReadoutID readoutID
	= assignPixelID->Assign(cluster);
      
      // We're using C++ STL structures in two different ways
      // here. For the key (ReadoutID) in ReadoutMap, using a map's
      // operator[] notation; this is because we will add many cluster
      // keys for every readoutID. For the list that the map points
      // to, use the insert function to expand that list.
      (*readoutMap)[ readoutID ].insert( ckey );
    }

    // Add a row to the output tree.
    outputTree->Fill();

  } // For each event

  // Build an index for this tree. This will allow downstream
  // programs to quickly access a given EventID within the tree.
  outputTree->BuildIndex("EventID.Index()");

  outputTree->Write();
  output->Close();
}
