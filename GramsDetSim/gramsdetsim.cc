// gramsdetsim.cc
// Take the hit output from GramsG4 and apply detector-response effects.
// 21-Sep-2022 Satoshi Takashima and William Seligman

// Our function(s) for the detector response.
#include "RecombinationModel.h"
#include "AbsorptionModel.h"
#include "DiffusionModel.h"

// For processing command-line and XML file options.
#include "Options.h" // in util/

// ROOT includes
#include "TFile.h"
#include "TTreeReader.h"
#include "TRandom.h"

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

  // Get the options associated with the input file and ntuple..
  std::string inputFileName;
  options->GetOption("inputfile",inputFileName);
  
  std::string inputNtupleName;
  options->GetOption("inputntuple",inputNtupleName);

  if (verbose)
    std::cout << "gramsdetsim: input file = '" << inputFileName
	      << "', input ntuple = '" << inputNtupleName
	      << "'" << std::endl;

  // For any model that requires random-number generation (as of
  // Sep-2022, only DiffusionModel), Get and set the random number
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
  auto reader = new TTreeReader(inputNtupleName.c_str(), input);

  // Create a TTreeReaderValue for each column in the ntuple whose
  // value we'll use. Note that the variable we create must be
  // accessed as if it were a pointer; e.g., if you want the value of
  // "energy", you must use *energy in the code.
  TTreeReaderValue<Int_t> Run        = {*reader, "Run"};
  TTreeReaderValue<Int_t> Event      = {*reader, "Event"};
  TTreeReaderValue<Int_t> TrackID    = {*reader, "TrackID"};
  TTreeReaderValue<Int_t> PDGCode    = {*reader, "PDGCode"};
  TTreeReaderValue<Int_t> NumPhotons = {*reader, "numPhotons"};
  TTreeReaderValue<Double_t> energy  = {*reader, "energy"};
  TTreeReaderValue<Float_t> xStart   = {*reader, "xStart"};
  TTreeReaderValue<Float_t> yStart   = {*reader, "yStart"};
  TTreeReaderValue<Float_t> zStart   = {*reader, "zStart"};
  TTreeReaderValue<Float_t> xEnd     = {*reader, "xEnd"};
  TTreeReaderValue<Float_t> yEnd     = {*reader, "yEnd"};
  TTreeReaderValue<Float_t> zEnd     = {*reader, "zEnd"};
  TTreeReaderValue<Int_t> Identifier = {*reader, "identifier"};

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

  Int_t run;
  Int_t event;
  Int_t trackID;
  Int_t numPhotons;
  Int_t pdgCode;
  Int_t identifier;

  // DiffusionModel will break up the ionization into electron
  // clusters. Define the vectors for the cluster energies and
  // (x,y,z,t) of each cluster.
  std::vector<Double_t> energyAtAnode;
  std::vector<Double_t> electronAtAnode;
  std::vector<Double_t> xPosAtAnode;
  std::vector<Double_t> yPosAtAnode;
  std::vector<Double_t> zPosAtAnode;
  std::vector<Double_t> timeAtAnode;

  outputNtuple->Branch("Run",           &run);
  outputNtuple->Branch("Event",         &event);
  outputNtuple->Branch("TrackID",       &trackID);
  outputNtuple->Branch("PDGCode",       &pdgCode);
  outputNtuple->Branch("numPhotons",    &numPhotons);
  outputNtuple->Branch("energy",        &energyAtAnode);
  outputNtuple->Branch("numElectrons",  &electronAtAnode);
  outputNtuple->Branch("x",             &xPosAtAnode);
  outputNtuple->Branch("y",             &yPosAtAnode);
  outputNtuple->Branch("z",             &zPosAtAnode);
  outputNtuple->Branch("timeAtAnode",   &timeAtAnode);
  outputNtuple->Branch("identifier",    &identifier);

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

  //absorption
  bool doAbsorption;
  options->GetOption("absorption", doAbsorption);

  // If we're using this model, initialize it. 
  gramsdetsim::AbsorptionModel* absorptionModel = NULL;
  if ( doAbsorption ) {
    absorptionModel = new gramsdetsim::AbsorptionModel(reader);
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
    diffusionModel = new gramsdetsim::DiffusionModel(reader);
    if (verbose)
      std::cout << "gramsdetsim: DiffusionModel turned on" << std::endl;
  }
  else {
    if (verbose)
      std::cout << "gramsdetsim: DiffusionModel turned off" << std::endl;
  }

  double energy_sca;

  // For each row in the input ntuple:
  while ( (*reader).Next() ) {

    // Remember that given the TTreeReaderValue definitions above, a
    // variable read from the input ntuple must be accessed like a
    // pointer.

    run = *Run;
    event = *Event;
    trackID = *TrackID;
    numPhotons = *NumPhotons;
    pdgCode = *PDGCode;
    identifier = *Identifier;
    
    energyAtAnode.clear();
    electronAtAnode.clear();
    xPosAtAnode.clear();
    yPosAtAnode.clear();
    zPosAtAnode.clear();
    timeAtAnode.clear();

    if (debug)
      std::cout << "gramsdetsim: at entry " << reader->GetCurrentEntry() << std::endl;
    
    // Set up some preliminary default values, which will be recalculated
    // below. 
    energy_sca = *energy;
    energyAtAnode.push_back(energy_sca);
    electronAtAnode.push_back(energy_sca * m_MeVToElectrons);
    xPosAtAnode.push_back(0.5 * (*xStart + *xEnd));
    yPosAtAnode.push_back(0.5 * (*yStart + *yEnd));
    zPosAtAnode.push_back(0.5 * (*zStart + *zEnd));
    timeAtAnode.push_back((m_readout_plane_coord - zPosAtAnode[0]) / m_DriftVel);

    if (debug)
        std::cout << "gramsdetsim: before model corrections, energyAtAnode=" 
		  << energy_sca 
		  << " timeAtAnode=" << (m_readout_plane_coord - zPosAtAnode[0]) / m_DriftVel << std::endl;

    // Apply the model(s). Handle potential computation errors (i.e.,
    // if dx is zero) within the different models.

    if ( doRecombination ) {
      energy_sca = recombinationModel->Calculate(energy_sca);
      if ( isnan(energy_sca) ) 
	energyAtAnode[0] = 0.0;
      else
	energyAtAnode[0] = energy_sca;
    }

    if (debug)
      std::cout << "gramsdetsim: after recombination model corrections, energyAtAnode=" 
	  	<< energy_sca << std::endl;
    
    //absorption
    if ( doAbsorption  &&  ! isnan(energy_sca) ) {
      energy_sca = absorptionModel->Calculate(energy_sca);
      energyAtAnode[0] = energy_sca;
    }
    else
      energyAtAnode[0] = 0.0;
      
    if (debug)
      std::cout << "gramsdetsim: after absorption model corrections, energyAtAnode=" 
	  	<< energy_sca << std::endl;
    
    //diffusion
    if ( doDiffusion  &&  ! isnan(energy_sca) ) {
      // This an "STL trick" to return a number of different vectors
      // at once from a single method.
      std::tie(energyAtAnode, electronAtAnode, xPosAtAnode, yPosAtAnode, zPosAtAnode, timeAtAnode)
	= diffusionModel->Calculate(energy_sca);
    }

    if (debug)
      std::cout << "gramsdetsim: after diffusion model corrections, energyAtAnode.size()=" 
	  	<< energyAtAnode.size() << std::endl;

    // After all the model effects have been applied, write the
    // detector-response value(s).
    outputNtuple->Fill();
  }

  // Wrap-up. Close all files. Delete any pointers we created.
  outputNtuple->Write();
  output->Close();
  delete recombinationModel;
  delete absorptionModel;
  delete diffusionModel;
  delete reader;
  input->Close();
}
