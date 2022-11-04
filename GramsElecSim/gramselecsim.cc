// gramselecsim.cc 

// Apply electronics-simulation models to the electron clusters that
// come from GramsReadoutSim.

// 25-Oct-2022 Satoshi Takashima

// Our function(s) for the electronics response.
#include "UtilFunctions.h"
#include "AddNoise.h"
#include "ADConvert.h"
#include "ElecStructure.h"
#include "PreampProcessor.h"
#include "LoadOptionFile.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"

// C++ includes
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <numeric>
#include <memory>
#include <ctime>
#include <unordered_map>

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

  std::string inputFileName;
  std::string inputNtupleName;
  options->GetOption("inputfile",     inputFileName);
  options->GetOption("inputntuple",   inputNtupleName);
  
  if (verbose || debug)
    std::cout << "gramselecsim: input file = '" << inputFileName
              << "', input ntuple = '" << inputNtupleName
              << "'" << std::endl;
  
  auto input = TFile::Open(inputFileName.c_str());
  if (!input || input->IsZombie()) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "gramselecsim: Could not open file '" << inputFileName << "'"
	      << std::endl;
    exit(EXIT_FAILURE);
  }
  
  // Save the options from the input file, to provide a historical
  // record of the analysis chain.
  options->CopyInputNtuple(input);
  
  if (debug) {
    std::cout << "gramselecsim main: about to access ElecStructure options via LoadOptionFile" << std::endl;
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

  // Open the input TTree.
  TTree* intree = (TTree*)input->Get(inputNtupleName.c_str());

  // Define the variables to be read from the input ntuple. 
  int run;
  int event;
  int num_step;
    
  std::vector<double>   timeAtAnode;
  std::vector<double>   numElectrons;
  std::vector<double>*  ptimeAtAnode = 0;
  std::vector<double>*  pnumElectrons = 0;
  std::vector<int>*     p_pixel_idx = 0;
  std::vector<int>*     p_pixel_idy = 0;

  int nEntry = intree->GetEntries();
  intree->SetBranchAddress("Run",             &run);
  intree->SetBranchAddress("Event",           &event);
  intree->SetBranchAddress("numElectrons",    &pnumElectrons);
  intree->SetBranchAddress("timeAtAnode",     &ptimeAtAnode);
  intree->SetBranchAddress("num_step",        &num_step);
  intree->SetBranchAddress("pixel_idx",       &p_pixel_idx);
  intree->SetBranchAddress("pixel_idy",       &p_pixel_idy);

  // These functions, defined with Elecstructure.h, return the ROOT
  // ntuple specification for how to store the options values in the
  // output ntuple. (As of Oct-2022, these are not used.)
  std::string general_header_type     = gramselecsim::GeneralHeaderType();
  std::string preamp_header_type      = gramselecsim::PreampHeaderType();
  std::string noise_header_type       = gramselecsim::NoiseHeaderType();
  std::string adc_header_type         = gramselecsim::ADCHeaderType();

  if (debug) {
    std::cout << "gramselecsim main: ntuple write options accessed" << std::endl;
  }

  // Now read in the options associated with the output file and ntuple.
  std::string outputFileName;
  options->GetOption("outputfile",    outputFileName);

  std::string outputNtupleName;
  options->GetOption("outputntuple",  outputNtupleName);

  // Open the output file.
  auto output = TFile::Open(outputFileName.c_str(),"RECREATE");

  // Write the options to the output file in order to preserve them.
  options->WriteNtuple(output);

  // Output ntuple: information for each event
  TTree* outtree = new TTree(outputNtupleName.c_str(),"Electronics Response");

  std::vector<double>*  p_charge_a = 0; // output analog waveform from a given pixel
  std::vector<int>*     p_charge_d = 0; // output digital waveform from a given pixel
  int pixel_idx = 0;
  int pixel_idy = 0;

  outtree->Branch("Run",              &run,          "Run/I");
  outtree->Branch("Event",            &event,        "Event/I");
  outtree->Branch("pixel_idx",        &pixel_idx,    "pixel_idx/I");
  outtree->Branch("pixel_idy",        &pixel_idy,    "pixel_idy/I");
  outtree->Branch("charge_A",         &p_charge_a);
  outtree->Branch("charge_D",         &p_charge_d);

  if (debug) {
    std::cout << "gramselecsim main: output ntuple defined" << std::endl;
  }

  double sample_freq = header_adc.sample_freq;

  //int response_current_bin = int(header_current.current_func_duration / header_gen.timebin_width);
  int num_tbin = int(header_gen.time_window / header_gen.timebin_width);

  if (verbose) {
    std::cout << "gramselecsim main: number of output time bins=" << num_tbin << std::endl;
  }

  // Use make_shared for these model routines so that we don't have
  // to worry about deleting them later.
  auto adconverter = std::make_shared<gramselecsim::ADConvert>();
  auto addNoise = std::make_shared<gramselecsim::AddNoise>();
  auto preampProcessor = std::make_shared<gramselecsim::PreampProcessor>(num_tbin);

  if (debug) {
    std::cout << "gramselecsim main: model routines defined" << std::endl;
  }

  // The map of (x,y) pixel IDs to the electrons accumulated within each time bin.

  // Unlike a regular std::map, which stores its (key,value) pairs in sorted order
  // by key, an std::unordered_map stores its (key,value) pairs in buckets which
  // are computed by the "hash" of its key. This makes accessing individual keys
  // faster, at the expense of making searches slower. 
  // typedef std::unordered_map<std::vector<int>, std::vector<int>, gramselecsim::HashVI> p2w_type;

  // Let's hold off on the unordered_map, and use a std::pair instead
  // as the the key to a map. Recall that for a std::pair, .first and
  // .second refer to the first and second elements of the pair
  // respectively.
  typedef std::pair<int,int> key_type;
  typedef std::map<key_type, std::vector<int>> p2w_type;
  p2w_type mp_pixel2waveform;

  std::vector<int> arrival_counter(num_tbin, 0);
  std::vector<double> waveform_with_noise( num_tbin, 0.0 );
  std::vector<double> waveform_csa_shaper( num_tbin, 0.0 );
  std::vector<int>    digi_waveform( num_tbin, 0 );
  std::vector<int> zeros_array(num_tbin, 0);

  std::vector<int> hittime;

  int num_cl;
  int ti;
  time_t t1 = time(NULL);

  if (debug) {
    std::cout << "gramselecsim main: about to process input" << std::endl;
  }

  // For each row in the input ntuple...
  for ( int istep=0; istep<nEntry; istep++ ) {

    mp_pixel2waveform.clear();

    if (debug) {
      std::cout << "gramselecsim main: about to get entry istep=" << istep << std::endl;
    }

    intree->GetEntry(istep);

    //Aggregate pixels electrons arrive at

    // For each step within an event...
    for ( int j=0; j<num_step; j++ ) {

      if (debug) {
	std::cout << "gramselecsim main: about to get entry istep+j=" << istep+j << std::endl;
      }

      intree->GetEntry(istep + j);
      // The number of electron clusters that arrived at the readout pixels.
      num_cl = (*p_pixel_idx).size();
      
      if (debug) {
	std::cout << "gramselecsim main: at row istep=" << istep
		  << " j=" << j << " num_cl=" << num_cl << std::endl;
      }

      // For each cluster...
      for ( int jcl=0; jcl<num_cl; jcl++ ) {

	if (debug) {
	  std::cout << "gramselecsim main: about to process cluster " << jcl << std::endl;
	}

	// The integer time bin in which the cluster arrived.
	ti = std::max(0, 
	     std::min(num_tbin-1, int(std::floor((*ptimeAtAnode)[jcl] / header_gen.timebin_width)))
                     );

	// The number of electrons in this cluster.
	auto clusterElectrons = (*pnumElectrons)[jcl];

	// Create the pixel ID key.
	key_type key( (*p_pixel_idx)[jcl], (*p_pixel_idy)[jcl] );

	// Search for the (x,y) pixel ID in our map of pixels->waveforms..
	auto itr = mp_pixel2waveform.find(key);

	// Accumulate the number of electrons to arrive at the pixel within each time bin.

	// Is this the first cluster to arrive at the pixel?
	if( itr == mp_pixel2waveform.end() ) {
	  // Yes, so create a new waveform for that pixel.
	  // Fetch the address of the new vector, so we don't
	  // have to search the map more than once. 
	  auto& newWaveform = mp_pixel2waveform[key];
	  newWaveform = zeros_array;
	  newWaveform[ti] = clusterElectrons;
	} else {
	  // At least one cluster has already arrived at the pixel.
	  // Save ourselves a bit of time: itr is already pointing an
	  // entry in the map mp_pixel2waveform, so use that pointer
	  // to fetch the vector we want to add the electrons to.
	  itr->second[ti] += clusterElectrons;
	}
      } // for each cluster that arrives at a pixel
    } // for each step within an event 

    if (debug) {
      std::cout << "gramselecsim main: pixel2waveform accumulated" 
		<< " size=" << mp_pixel2waveform.size() << std::endl;
    }

    // For each pixel which accumulated any electrons: 
    for (auto mp_iter = mp_pixel2waveform.cbegin(); mp_iter != mp_pixel2waveform.cend(); ++mp_iter) {

      // The iterator mp_itr points to a given (key,value) pair in mp_pixel2waveform. 
      // These (key,value) pairs are stored as an std::pair<key-type, value-type>, and
      // are accessed by the methods "first" (key) and "second" (value). 

      // So "mp_itr->first.first" means to look at the (key,value) pointed to by iter_mp,
      // to look at the key part ("first"), and since the key is a std::pair, get the
      // value of the first element of the key; similarly mp_itr->first.second is the
      // second element of the pair. .

      pixel_idx = mp_iter->first.first;
      pixel_idy = mp_iter->first.second;

      // A trick: iter_mp-> is a vector, but instead of copying
      // the the entire vector, just copy its address to save
      // execution time and memory.
      const std::vector<int>& num_arrival_electron = mp_iter->second;
      std::vector<int> num_arrival_electron_with_noise;

      if (debug) {
	std::cout << "gramselecsim main: about to compute waveform for "
		  << "pixel=" << pixel_idx << "," << pixel_idy << std::endl;
	std::cout << "gramselecsim main: AddNoise..." << std::endl;
      }

      // Add noise to mp_pixel2waveform:[[ipix, ipiy]: e_arrival_flag]
      num_arrival_electron_with_noise = addNoise->ProcessElectronNoise(num_arrival_electron);

      if (debug) {
	std::cout << "gramselecsim main: PreAmp..." << std::endl;
      }
            
      //Add a response function
      waveform_csa_shaper = preampProcessor->ConvoluteResponse( num_arrival_electron_with_noise );
      p_charge_a = &waveform_csa_shaper;

      if (debug) {
	std::cout << "gramselecsim main: ADConvert..." << std::endl;
      }

      //Convert analog into digital
      digi_waveform = adconverter->Process(waveform_csa_shaper);
      p_charge_d = &digi_waveform;

      if (debug) {
	std::cout << "gramselecsim main: Fill output ntuple" << std::endl;
      }

      outtree->Fill();

      if (debug) {
	std::cout << "gramselecsim main: Output ntuple filled" << std::endl;
      }

    } // for each pixel with arriving electrons
    
    if (debug) {
      std::cout << "gramselecsim main: Finished pixel loop" << std::endl;
    }

    // Let our istep counter skip over any input rows we've already read.
    istep += (num_step-1);

  } // for each input ntuple row

  if (verbose) {
    time_t t2 = time(NULL);
    std::cout << "Time: " << t2 - t1 << "s" << std::endl;
  }

  outtree->Write();
  output->Close();
}
