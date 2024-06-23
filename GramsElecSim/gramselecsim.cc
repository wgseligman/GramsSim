// gramselecsim.cc 

// Apply electronics-simulation models to the electron clusters that
// come from GramsReadoutSim.

// 25-Oct-2022 Satoshi Takashima
// Note that all comments were added by William Seligman

// 12-Jun-2024 William Seligman
// Substantial structural revisions for the new tree format of the files.

// Our function(s) for the electronics response.
#include "UtilFunctions.h"
#include "AddNoise.h"
#include "ADConvert.h"
#include "ElecStructure.h"
#include "PreampProcessor.h"
#include "LoadOptionFile.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// For copying and accessing the detectory geometry.
#include "Geometry.h" // in util/

// From GramsDataObj
#include "EventID.h"
#include "ElectronClusters.h"
#include "ReadoutID.h"
#include "ReadoutMap.h"
#include "ReadoutWaveforms.h"

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TRandom.h"

// C++ includes
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <ctime>

///////////////////////////////////////


int main(int argc,char **argv)
{
  auto options = util::Options::GetInstance();
  auto result = options->ParseOptions(argc, argv, "gramselecsim");
  
  if (! result) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramselecsim: Aborting job due to failure to parse options"
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
    options->PrintOptions();
  }
  
  // For any model that requires random-number generation, get and set
  // the random number seed.
  int seed;
  options->GetOption("rngseed",seed);
  // Note that the default random-number generator in ROOT is
  // TRandom3.
  gRandom->SetSeed(seed);

  // This program reads two trees; or, if you wish, a single tree
  // that's divided into two files, with different columns in each
  // file. 

  // One tree is the output from GramsDetSim.
  std::string inputClustersFileName;
  std::string inputClustersTreeName;
  options->GetOption("inputElecClustersFile",   inputClustersFileName);
  options->GetOption("inputElecClustersTree",   inputClustersTreeName);
  
  auto inputClusters = TFile::Open(inputClustersFileName.c_str());
  if (!inputClusters || inputClusters->IsZombie()) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramselecsim: Could not open file '" << inputClustersFileName << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  auto clustersTree = inputClusters->Get<TTree>(inputClustersTreeName.c_str());
  if (!clustersTree) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramselecsim: Could not open tree '" << inputClustersTreeName << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // The other is the output from GramsReadoutSim.
  std::string inputMapFileName;
  std::string inputMapTreeName;
  options->GetOption("inputElecMapFile",   inputMapFileName);
  options->GetOption("inputElecMapTree",   inputMapTreeName);
  
  auto inputMap = TFile::Open(inputMapFileName.c_str());
  if (!inputMap || inputMap->IsZombie()) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramselecsim: Could not open file '" << inputMapFileName << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  auto mapTree = inputMap->Get<TTree>(inputMapTreeName.c_str());
  if (!mapTree) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramselecsim: Could not open tree '" << inputMapTreeName << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  if (verbose || debug)
    std::cout << "gramselecsim: input clusters file = '" << inputClustersFileName
              << "', input tree = '" << inputClustersTreeName
              << "'" << std::endl
	      << "gramselecsim: input map file = '" << inputMapFileName
              << "', input tree = '" << inputMapTreeName
              << "'" << std::endl;
  
  // Save the options from the input Map file, to provide a historical
  // record of the analysis chain.
  options->CopyInputNtuple(inputMap);

  // Define the trees from the above two files as friends with each
  // other. Note that when these trees were created in GramsDetSim and
  // GramsElecSim, we took care to make sure that each had the same
  // number of rows, indexed by EventID.
  mapTree->AddFriend(clustersTree);

  // Define the TTreeReader for this combined tree.
  auto reader = new TTreeReader(mapTree);

  // Create a TTreeReaderValue for each column in the combined tree
  // whose value we'll use. Note that we have two columns named
  // "EventID" in the combined tree, so specify which one to use.
  std::string eventColumn = inputMapTreeName + ".EventID";
  TTreeReaderValue<grams::EventID> inputEventID      = {*reader, eventColumn.c_str()};
  TTreeReaderValue<grams::ElectronClusters> clusters = {*reader, "ElectronClusters"};
  TTreeReaderValue<grams::ReadoutMap> readoutMap     = {*reader, "ReadoutMap"};

  // These functions, defined in GramsElecSim/include/Elecstructure.h,
  // return the ROOT ntuple specification for how to store the options
  // values in the output ntuple. 

  // (As of Oct-2022, these were not used; as of 12-Jun-2024, they are
  // now obsolete. See Elecstructure.h for more information.)
  std::string general_header_type     = gramselecsim::GeneralHeaderType();
  std::string preamp_header_type      = gramselecsim::PreampHeaderType();
  std::string noise_header_type       = gramselecsim::NoiseHeaderType();
  std::string adc_header_type         = gramselecsim::ADCHeaderType();

  // Now read in the options associated with the output file and ntuple.
  std::string outputFileName;
  options->GetOption("outputElecFile",    outputFileName);

  std::string outputTreeName;
  options->GetOption("outputElecTree",  outputTreeName);

  // Open the output file.
  auto output = TFile::Open(outputFileName.c_str(),"RECREATE");

  // Write the options to the output file in order to preserve them.
  options->WriteNtuple(output);

  // Copy the detector geometry from the input file to the output
  // file.
  auto geometry = util::Geometry::GetInstance();
  if (debug)
    std::cout << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramselecsim: About to copy geometry"
	      << std::endl;
  geometry->CopyGeometry(inputMap,output);
  if (debug)
    std::cout << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramselecsim: geometry copied"
	      << std::endl;

  // Define our output tree.
  TTree* outputTree = new TTree(outputTreeName.c_str(), "Electronics Response");

  // Define the columns of the output tree. Note that we include
  // EventID in the output, so that this new tree can be friends with
  // the input trees in other analysis programs.

  auto eventID = new grams::EventID();
  auto readoutWaveforms = new grams::ReadoutWaveforms();
  outputTree->Branch("EventID",          &eventID);
  outputTree->Branch("ReadoutWaveforms", &readoutWaveforms);

  if (debug) {
    std::cout << "gramselecsim main: output tree defined" << std::endl;
  }

  // The original author of this program set up a routine to load
  // values from the options XML file, then share those options among
  // the various subroutines.
  
  if (debug) {
    std::cout << "gramselecsim main: about to access options via LoadOptionFile" << std::endl;
  }
  
  // Read in the options for the various electronics-simulation models.
  auto optionloader = gramselecsim::LoadOptionFile::GetInstance();
  optionloader->Load();
  gramselecsim::general_header    header_gen      = optionloader->GeneralHeader();
  gramselecsim::preamp_header     header_pre      = optionloader->PreampHeader();
  gramselecsim::noise_header      header_noise    = optionloader->NoiseHeader();
  gramselecsim::adc_header        header_adc      = optionloader->ADCHeader();

  if (debug) {
    std::cout << "gramselecsim main: ElecStructure options accessed" << std::endl;
  }

  const double sample_freq = header_adc.sample_freq;

  // The number of time bins for the analog waveform. 
  const int num_tbin = int(header_gen.time_window / header_gen.timebin_width);

  if (verbose) {
    std::cout << "gramselecsim main: number of analog time bins=" << num_tbin << std::endl;
  }

  // Algorithms for computing waveforms.  Use make_shared for these
  // model routines so that we don't have to worry about deleting them
  // later.
  auto adconverter = std::make_shared<gramselecsim::ADConvert>();
  auto addNoise = std::make_shared<gramselecsim::AddNoise>();
  auto preampProcessor = std::make_shared<gramselecsim::PreampProcessor>(num_tbin);

  if (debug) {
    std::cout << "gramselecsim main: model routines defined" << std::endl;
  }

  // For timing how long this routine takes.
  time_t t1 = time(NULL);

  if (debug) {
    std::cout << "gramselecsim main: about to process input" << std::endl;
  }

  // For each row in the input tree:
  while ( (*reader).Next() ) {

    // Copy the event ID from input to output.
    (*eventID) = (*inputEventID);

    // Clear out any waveform information from the previous event.
    readoutWaveforms->clear();

    // For each readout cell that received any electron clusters:
    for ( const auto& [ readoutID, clusterKeys ] : (*readoutMap) ) {

      // We'll create new waveforms for each readout cell.
      grams::ReadoutWaveform readoutWaveform;
      readoutWaveform.readoutID = readoutID;

      // For accumulating the electrons arriving within each time bin. 
      std::vector<int> num_arrival_electrons( num_tbin, 0 );

      // for each electron cluster assigned to this readout cell:
      for ( const auto& clusterKey: clusterKeys ) {

	// Find the key for this cluster in our list of electron
	// clusters. (By the way, this is the point at which we're
	// making use of columns in two different files.)
	const auto search = clusters->find( clusterKey );

	if ( search == clusters->cend() ) {
	  // This should not happen. It means that GramsReadoutSim
	  // inserted a cluster key that was never defined in
	  // GramsDetSim.
	  std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		    << "gramselecsim: Aborting due to mis-match between:" << std::endl
		    << "file " << inputClustersFileName << " tree " << inputClustersTreeName
		    << " and "
		    << "file " << inputMapFileName << " tree " << inputMapTreeName
		    << std::endl;
	  exit(EXIT_FAILURE);
	}

	// We found the cluster's key in the list of electron
	// clusters. Fetch that cluster; remember that a map consists
	// of pairs (first,second).
	const auto& cluster = (*search).second;

	if (debug) {
	  std::cout << "gramselecsim main: about to process cluster: " << std::endl
		    << cluster << std::endl;
	}

	// The integer time bin in which the cluster arrived.
	int ti = std::max(0, 
			  std::min(num_tbin-1, int(std::floor( cluster.TAtAnode() / header_gen.timebin_width)))
                     );

	// Accumulate the number of electrons to arrive at the cell
	// within each time bin.
	num_arrival_electrons[ ti ] += cluster.NumElectrons();

      } // for each cluster within a readout cell

      if (debug) {
	std::cout << "gramselecsim main: about to compute waveform for "
		  << "ReadoutID=" << readoutID << std::endl;
	std::cout << "gramselecsim main: AddNoise..." << std::endl;
      }

      // Add noise to the number of electrons.
      const auto num_arrival_electron_with_noise 
	= addNoise->ProcessElectronNoise( num_arrival_electrons );

      if (debug) {
	std::cout << "gramselecsim main: PreAmp..." << std::endl;
      }
            
      //Add a response function
      readoutWaveform.analog = preampProcessor->ConvoluteResponse( num_arrival_electron_with_noise );

      if (debug) {
	std::cout << "gramselecsim main: ADConvert..." << std::endl;
      }

      // Convert analog into digital
      readoutWaveform.digital = adconverter->Process( readoutWaveform.analog );

      // Add the waveforms to our list of readout cells with a signal.
      readoutWaveforms->insert( std::make_pair( readoutID, readoutWaveform ) );

    } // for each cell with arriving electrons

    if (debug) {
      std::cout << "gramselecsim main: Readout waveforms for event " << (*eventID) 
		<< ":" << std::endl
		<< (*readoutWaveforms)
		<< std::endl;
    }

    outputTree->Fill();

    if (debug) {
      std::cout << "gramselecsim main: Output ntuple filled" << std::endl;
    }

  } // for each event

  if (verbose) {
    time_t t2 = time(NULL);
    std::cout << "Time: " << t2 - t1 << "s" << std::endl;
  }

  // Build an index for this tree. This will allow downstream
  // programs to quickly access a given EventID within the tree.
  outputTree->BuildIndex("EventID.Index()");

  outputTree->Write();
  output->Close();
}
