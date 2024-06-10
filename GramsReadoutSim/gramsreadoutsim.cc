// gramsreadoutsim.cc
// Take the charge-cluster results from GramsDetSim 
// and map their positions to the GRAMS readout.
// 25-Oct-2022 Satoshi Takashima

// The model of the readout geometry:
#include "AssignPixelID.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// For copying and accessing the detectory geometry.
#include "Geometry.h" // in util/

// ROOT includes
#include "TFile.h"
#include "TTree.h"

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

    auto assignPixelID = std::make_shared<gramsreadoutsim::AssignPixelID>(gramsreadoutsim::AssignPixelID());

    // Define the variables in the input tree.
    int run;
    int event;
    int trackID;
    int pDGCode;
    int numPhotons; // scintillation photons
    int cerPhotons; // Cerenkov photons
    int identifier;

    std::vector<double>* penergyAtAnode = 0;
    std::vector<double>* pelectronAtAnode = 0;
    std::vector<double>* pxPosAtAnode = 0;
    std::vector<double>* pyPosAtAnode = 0;
    std::vector<double>* pzPosAtAnode = 0;
    std::vector<double>* ptimeAtAnode = 0;

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

    TTree* intree = (TTree*)input->Get(inputTreeName.c_str());

    intree->SetBranchAddress("Run",           &run);
    intree->SetBranchAddress("Event",         &event);
    intree->SetBranchAddress("TrackID",       &trackID);
    intree->SetBranchAddress("PDGCode",       &pDGCode);
    intree->SetBranchAddress("numPhotons",    &numPhotons); // scintillation photons
    intree->SetBranchAddress("cerPhotons",    &cerPhotons); // Cerenkov photons
    intree->SetBranchAddress("energy",        &penergyAtAnode);
    intree->SetBranchAddress("numElectrons",  &pelectronAtAnode);
    intree->SetBranchAddress("x",             &pxPosAtAnode);
    intree->SetBranchAddress("y",             &pyPosAtAnode);
    intree->SetBranchAddress("z",             &pzPosAtAnode);
    intree->SetBranchAddress("timeAtAnode",   &ptimeAtAnode);
    intree->SetBranchAddress("identifier",    &identifier);

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
    TTree* outtree = new TTree(outputTreeName.c_str(), "ReadoutSim");

    // The number of hits (steps) within an event. 
    int num_step = 0;
    std::vector<int>*   p_pixel_idx = 0;
    std::vector<int>*   p_pixel_idy = 0;
    std::vector<int>    pixel_idx;
    std::vector<int>    pixel_idy;

    outtree->Branch("Run",           &run,               "Run/I");
    outtree->Branch("Event",         &event,             "Event/I");
    outtree->Branch("TrackID",       &trackID,           "TrackID/I");
    outtree->Branch("PDGCode",       &pDGCode,           "PDGCode/I");
    outtree->Branch("numPhotons",    &numPhotons,        "numPhotons/I");
    outtree->Branch("cerPhotons",    &cerPhotons,        "cerPhotons/I");
    outtree->Branch("energy",        &penergyAtAnode);
    outtree->Branch("numElectrons",  &pelectronAtAnode);
    outtree->Branch("x",             &pxPosAtAnode);
    outtree->Branch("y",             &pyPosAtAnode);
    outtree->Branch("z",             &pzPosAtAnode);
    outtree->Branch("timeAtAnode",   &ptimeAtAnode);
    outtree->Branch("num_step",      &num_step);
    outtree->Branch("identifier",    &identifier);
    outtree->Branch("pixel_idx",     &p_pixel_idx);
    outtree->Branch("pixel_idy",     &p_pixel_idy);
 
    if (verbose)
        std::cout << "gramsreadoutsim: output tree defined" << std::endl;

    // The number of rows in the input ntuple.
    int num_row = intree->GetEntries();
    int current_runid = -1;
    int current_eventid = -1;

    // For each row in the input tree (i.e., for each event in
    // the original GramsG4 simulation):
    for (int i=0; i<num_row; i++) {

      // Read the row.
      intree->GetEntry(i);
      
      // Previous GramsSim programs like GramsG4 can use
      // multi-threading. While all the rows associated with a given
      // event will be adjacent to each other in the input ntuple, the
      // events are _not_ guaranteed to be in any kind of order.

      // When we cross the boundary between one event and the next..
      if (run != current_runid  ||  event != current_eventid) {

	// "Count ahead" in the ntuple to determine the number of rows
	// with the same run/event ID. This tells us the number of steps/hits
	// in the current event, num_step.

	// Note that the value of num_step in the output ntuple will
	// be the same for all the ntuple rows for this run/event ID.

	current_runid = run;
	current_eventid = event;

	num_step = 1;
	if ( i != (num_row-1) ) {
	  do {
	    // Move ahead in the ntuple rows while the run/event
	    // doesn't change, counting the rows as we do.
	    intree->GetEntry(i + num_step);
	    if (run == current_runid  &&  event == current_eventid) {
	      num_step += 1;
	    }
	  } while ( (current_runid == run ) && 
		    (current_eventid == event) && 
		    ((i + num_step) < num_row) );

	  // Now that we've moved ahead, go back to the first entry
	  // for this event ID.
	  intree->GetEntry(i);
 
	  if (verbose)
	    std::cout << "gramsreadoutsim: run=" << run
		      << " event=" << event << " has "
		      << num_step << " steps" << std::endl;

	} // If we haven't reached the last row. 
      } // New run/event ID in the input ntuple.
      
      // From the (x,y) position vectors of electron clusters at the
      // anode, assign a pixel readout ID to each cluster.
      std::tie(pixel_idx, pixel_idy) = assignPixelID->Assign(pxPosAtAnode, pyPosAtAnode);

      // A trick: Copy just the reference to the vectors,
      // instead of the entire vectors, to save time and memory.
      p_pixel_idx = &pixel_idx;
      p_pixel_idy = &pixel_idy;
      
      // Add a row to the output ntuple.
      outtree->Fill();

    } // For each hit

    outtree->Write();
    output->Close();
}
