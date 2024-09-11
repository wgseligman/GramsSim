// gramsdetsim.cc
// Take the hit output from GramsG4 and apply detector-response effects.
// 21-Sep-2022 Satoshi Takashima and William Seligman

// Our function(s) for the detector response.
#include "RecombinationModel.h"
#include "AbsorptionModel.h"
#include "DiffusionModel.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// For copying and accessing the detectory geometry.
#include "Geometry.h" // in util/

// From GramsDataObj
#include "EventID.h"
#include "MCLArHits.h"
#include "ElectronClusters.h"

// ROOT includes
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TRandom.h"
#include "Math/Vector4D.h"

// C++ includes
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <numeric>

///////////////////////////////////////
int main(int argc,char **argv)
{
  // Initialize the options from the XML file and the
  // command line. Make sure this happens first!
  auto options = util::Options::GetInstance();
  auto result = options->ParseOptions(argc, argv, "gramsdetsim");

  // Abort if we couldn't parse the job options.
  if (! result) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramsdetsim: Aborting job due to failure to parse options"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // Fetch the debug flag if it's on the command line.
  bool debug;
  options->GetOption("debug",debug);

  // If the user included "-h" or "--help" on the command line, print
  // a help message and exit.
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

  // Get the options associated with the input file and tree..
  std::string inputFileName;
  options->GetOption("inputDetSimFile",inputFileName);
  
  std::string inputTreeName;
  options->GetOption("inputHitsTree",inputTreeName);

  if (verbose)
    std::cout << "gramsdetsim: input file = '" << inputFileName
	      << "', input tree = '" << inputTreeName
	      << "'" << std::endl;

  // For any model that requires random-number generation (as of
  // Sep-2022, only DiffusionModel), get and set the random number
  // seed.
  int seed;
  options->GetOption("rngseed",seed);
  // Note that the default random-number generator in ROOT is
  // TRandom3.
  gRandom->SetSeed(seed);

  // Parameters for computing hit projections onto the readout plane.
  double m_readout_plane_coord;
  double m_DriftVel;
  double m_MeVToElectrons;
  options->GetOption("ReadoutPlaneCoord",    m_readout_plane_coord);  
  options->GetOption("ElectronDriftVelocity", m_DriftVel);
  options->GetOption("MeVToElectrons",        m_MeVToElectrons);

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

  // If the input file has an Options ntuple created by an earlier
  // program in the analysis chain, copy those options to maintain a
  // historical record.
  options->CopyInputNtuple(input);

  // The standard way of reading a TTree (without using RDataFrame) in
  // C++ is using the TTreeReader.
  auto reader = new TTreeReader(inputTreeName.c_str(), input);

  // Create a TTreeReaderValue for each column in the tree whose
  // value we'll use.
  TTreeReaderValue<grams::EventID> inputEventID = {*reader, "EventID"};
  TTreeReaderValue<grams::MCLArHits> LArHits    = {*reader, "LArHits"};

  // Now read in the options associated with the output file and tree. 
  std::string outputFileName;
  options->GetOption("outputDetSimFile",outputFileName);

  std::string outputTreeName;
  options->GetOption("outputDetSimTree",outputTreeName);

  if (verbose)
    std::cout << "gramsdetsim: output file = '" << outputFileName
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
	      << "gramsdetsim: About to copy geometry"
	      << std::endl;
  geometry->CopyGeometry(input,output);
  if (debug)
    std::cout << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramsdetsim: geometry copied"
	      << std::endl;

  // Define our output tree.
  auto outputTree = new TTree(outputTreeName.c_str(),"Detector Response");

  // Define the columns of the output tree. Since this tree will
  // be "friends" with the input tree, we only have to include
  // columns that are unique to the detector response.

  // The exception is "EventID', which we duplicate between trees, to
  // make it easier for them to be friends.

  auto eventID = new grams::EventID();
  auto clusters = new grams::ElectronClusters();
  // By experimenting, it turns out that setting the splitlevel to 0
  // improves potential issues with ROOT's TBrowser.
  outputTree->Branch("EventID",          &eventID,  32000, 0);
  outputTree->Branch("ElectronClusters", &clusters, 32000, 0);

  // Are we using this particular model?
  bool doRecombination;
  options->GetOption("recombination",doRecombination);

  // If we're using this model, initialize it. 
  gramsdetsim::RecombinationModel* recombinationModel = NULL;
  if ( doRecombination ) {
    recombinationModel = new gramsdetsim::RecombinationModel();
    if (verbose)
      std::cout << "gramsdetsim: RecombinationModel turned on" << std::endl;
  }
  else {
    if (verbose)
      std::cout << "gramsdetsim: RecombinationModel turned off" << std::endl;
  }

  //absorption
  bool doAbsorption;
  options->GetOption("absorption", doAbsorption);

  // If we're using this model, initialize it. 
  gramsdetsim::AbsorptionModel* absorptionModel = NULL;
  if ( doAbsorption ) {
    absorptionModel = new gramsdetsim::AbsorptionModel();
    if (verbose)
      std::cout << "gramsdetsim: AbsorptionModel turned on" << std::endl;
  }
  else {
    if (verbose)
      std::cout << "gramsdetsim: AbsorptionModel turned off" << std::endl;
  }

  //diffusion
  bool doDiffusion;
  options->GetOption("diffusion", doDiffusion);

  gramsdetsim::DiffusionModel* diffusionModel = NULL;
  if ( doDiffusion ) {
    diffusionModel = new gramsdetsim::DiffusionModel();
    if (verbose)
      std::cout << "gramsdetsim: DiffusionModel turned on" << std::endl;
  }
  else {
    if (verbose)
      std::cout << "gramsdetsim: DiffusionModel turned off" << std::endl;
  }

  if (debug) 
    std::cout << "gramsdetsim.cc - debug 1000" 
	      << std::endl;

  // For each row in the input tree:
  while ( (*reader).Next() ) {

    // Increase the clusterID across all the hits in this event.
    int clusterID = 0;
    
    // Clean out any cluster data from the previous event.
    clusters->clear();

    // Copy the event ID from one tree to another.
    (*eventID) = (*inputEventID);

    // For each hit in the event:
    for ( const auto& [ key, hit ] : (*LArHits) ) {

      if (debug)
	std::cout << "gramsdetsim: at entry " << reader->GetCurrentEntry() << std::endl;

      // The total hit energy, which is gradually adjusted by the models
      // during this routine.
      double energy_sca = hit.energy;

      if (debug)
        std::cout << "gramsdetsim: before model corrections, energy=" 
		  << energy_sca
		  << std::endl;

      // Apply the model(s). Handle potential computation errors (i.e.,
      // if dx is zero) within the different models.

      if ( doRecombination ) {
	energy_sca = recombinationModel->Calculate(energy_sca, hit);
	if ( std::isnan(energy_sca) ) 
	  energy_sca = 0.0;
      }

      if (debug)
	std::cout << "gramsdetsim: after recombination model corrections, energyAtAnode=" 
		  << energy_sca << std::endl;
    
      //absorption
      if ( doAbsorption  &&  energy_sca > 0. )
	energy_sca = absorptionModel->Calculate(energy_sca, hit);
      
      if (debug)
	std::cout << "gramsdetsim: after absorption model corrections, energyAtAnode=" 
		  << energy_sca << std::endl;
    
      //diffusion
      std::vector< grams::ElectronCluster > calcClusters;
      if ( doDiffusion  &&  energy_sca > 0. ) {
	// The clusters in this vector returned by DiffusionModel will
	// replace the "default cluster", if all goes well. Maintain
	// and increment the cluster ID across all the clusters in
	// this event.
	calcClusters = diffusionModel->Calculate(energy_sca, hit, clusterID);
      }

      if (debug) {
	std::cout << "gramsdetsim: after diffusion model corrections, calcClusters.size()=" 
		  << calcClusters.size() << std::endl;
	std::cout << hit << std::endl;
	for ( const auto& c : calcClusters ) {
	  std::cout << c << std::endl;
	}
	std::cout << std::endl;
      }

      // For each cluster returned by the diffusion model::
      for ( auto& cluster : calcClusters ) {
	// Create the key for this cluster.
	auto ckey = std::make_tuple( cluster.trackID, cluster.hitID, cluster.clusterID );

	// Append this cluster to the overall list of clusters for this event.
	clusters->insert( std::make_pair( ckey, cluster ) );
      }

    } // for each hit

    // After all the model effects have been applied, write the
    // detector-response value(s) for all the hits/clusters in the
    // event.
    outputTree->Fill();

  } // for each event

  // Build an index for this tree. This will allow downstream
  // programs to quickly access a given EventID within the tree.
  outputTree->BuildIndex("EventID.Index()");

  // Wrap-up. Close all files. Delete any pointers we created.
  outputTree->Write();
  output->Close();
  delete recombinationModel;
  delete absorptionModel;
  delete diffusionModel;
  delete reader;
  input->Close();
}
