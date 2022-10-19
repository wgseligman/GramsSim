/// \file GramsG4RandomSeedAction.cc
/// \brief User-action class to handle random-number engine and seeds.

/// This is a user-action class that handles the initialization and
/// saving of the random-number engine state for this
/// simulation. Contrary to what the name suggests, it does not
/// generate a seed at random!

#include "GramsG4RandomSeedAction.hh"

#include "Options.h" // in util/
#include "UserAction.h" // in g4util/

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Threading.hh"
#include "Randomize.hh"

namespace gramsg4 {

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Default constructor. 
  RandomSeedAction::RandomSeedAction()
    : UserAction()
    , m_verbose(false)
    , m_debug(false)
  {
    // Set up access to the options from the XML file.
    m_options = util::Options::GetInstance();
    m_options->GetOption("verbose",m_verbose);
    m_options->GetOption("debug",m_debug);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Destructor.
  RandomSeedAction::~RandomSeedAction() {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Initialize a random-number engine with the seed supplied by the
  // user.
  void RandomSeedAction::BeginOfRunAction(const G4Run*) {

    // The manager we'll need.
    auto runManager = G4RunManager::GetRunManager();

    // Getting random-number generation to work in a multi-threaded
    // application is a bit tricky. Save the thread ID for diagnostic
    // messages.
    auto threadID = G4Threading::G4GetThreadId();

    // Working with random-number generator (RNG) states.
    G4String rngDirectory;
    m_options->GetOption("rngdir",rngDirectory);
    if ( ! rngDirectory.empty() ) {
      // The user wants to save or restore the RNG state.
      runManager->SetRandomNumberStoreDir(rngDirectory);
      runManager->SetRandomNumberStore(true);
      if (m_verbose || m_debug) 
	G4cout << "GramsG4RandomSeedAction::BeginOfRunAction: "
	       << "thread ID = " << threadID << ": "
	       << "Saving/restoring RNG states from "
	       << rngDirectory << G4endl;

      // For the meaning of 'rngPerEvent', see either
      // options.xml or $G4INSTALL/include/Geant4/G4RunManager.hh.
      // As of Oct-2022, the recognized values are:
      //  value="0" - Do not save per-event RNG state in 'rngdir'.
      //  value="1" - Save RNG state before primary-particle generation.
      //  value="2" - Save RNG state before event processing (after primary generation)
      //  value="3" - Both are stored.

      G4int rngPerEvent;
      m_options->GetOption("rngperevent",rngPerEvent);
      if ( rngPerEvent != 0 ) {
	runManager->SetRandomNumberStorePerEvent(true);
	runManager->StoreRandomNumberStatusToG4Event(rngPerEvent);
	if (m_verbose || m_debug) 
	  G4cout << "GramsG4RandomSeedAction::BeginOfRunAction: "
		 << "thread ID = " << threadID << ": "
		 << "setting rngperevent to " << rngPerEvent << G4endl;
      }

    } // if rngDirectory not empty

    // We have to be careful for multi-threaded running; we don't want
    // the worker threads to each read in the identical random-number
    // seed. So if this is a multi-threaded application, skip this if
    // we're in a worker thread.

    if ( G4Threading::IsMultithreadedApplication()  &&
	 G4Threading::IsWorkerThread() ) {
      if (m_verbose || m_debug) 
	G4cout << "GramsG4RandomSeedAction::BeginOfRunAction: "
	       << "thread ID = " << threadID << ": "
	       << "No need for separate seed initialization " 
	       << G4endl;
      return;
    }

    // Working with random numbers. First, define a RNG (Random Number
    // Generator). (For a list of engines, see
    // http://geant4.web.cern.ch/ooaandd/analysis/class_spec/global/randommisstat)
    // Note that I have no reason to expect that RanecuEngine is better
    // or worse than any of the other distributions.
    auto rngEngine = new CLHEP::RanecuEngine();

    // Did the user supply a random-number seed?
    // (Note that if they requested that the RNG
    // be restored from a file, this will be ignored.)
    G4int rngSeed;
    m_options->GetOption("rngseed",rngSeed);
    if ( rngSeed != 0 ) {
      if (m_verbose || m_debug) 
	G4cout << "GramsG4RandomSeedAction::BeginOfRunAction: "
	       << "thread ID = " << threadID << ": "
	       << "Setting RNG seed to " 
	       << rngSeed << G4endl;
      rngEngine->setSeed(rngSeed);
    }

    // Save the engine.
    G4Random::setTheEngine(rngEngine);

    // Does the user want to restore the RNG
    // state from a file?
    G4String rngRestoreFile;
    m_options->GetOption("rngrestorefile",rngRestoreFile);
    if ( ! rngRestoreFile.empty() ) {
      if (m_verbose || m_debug) 
	G4cout << "GramsG4RandomSeedAction::BeginOfRunAction: "
	       << "thread ID = " << threadID << ": "
	       << "Restoring RNG state from "
	       << rngRestoreFile << G4endl;
      runManager->RestoreRandomNumberStatus(rngRestoreFile);
    }

  } // BeginOfRunAction

} // namespace gramsg4
