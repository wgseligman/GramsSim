/// \file GramsG4WriteNtuplesAction.cc
/// \brief User-action class to write out ntuples.

/// This is a user-action class that writes out the event hits and
/// truth information. At present, the output takes the form of basic
/// ROOT n-tuples.

#include <TSystem.h>

#include "GramsG4WriteNtuplesAction.hh"
#include "GramsG4LArHit.hh"
#include "GramsG4ScintillatorHit.hh"
#include "G4SystemOfUnits.hh"

#include "Options.h" // in util/
#include "UserAction.h" // in g4util/

// In GramsDataObj
#include "EventID.h"
#include "MCTrackList.h"
#include "MCLArHits.h"
#include "MCScintHits.h"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"

// ROOT
#include <TFile.h>
#include <TTree.h>
#include <Math/Vector4D.h>

#include <string>

namespace gramsg4 {

  // In a multi-threaded application, we have to make sure that all
  // operations are thread-safe. Example of operations that are _not_
  // thread-safe are adding rows to a tree, and some vector
  // operations.

  // To get around this, use G4's locking mechanism (which is
  // basically a wrapper around the standard C++ mutex). See
  // $G4INSTALL/include/G4AutoLock.hh for details.
  static G4Mutex myMutex;

  // In multi-threaded running, we need to make sure any file-related
  // variables are kept unique between instances of this class. I know
  // that static variables are considered "wrong" but they're the best
  // solution in this case.
  static TFile* s_file = nullptr;
  static TTree* s_tree = nullptr;

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Default constructor. 
  WriteNtuplesAction::WriteNtuplesAction()
    : UserAction()
    , m_LArHitCollectionID(-1)
    , m_ScintillatorHitCollectionID(-1)
    , m_eventID(nullptr)
    , m_mcTrackList(nullptr)
    , m_mcLArHits(nullptr)
    , m_mcScintHits(nullptr)
  {
    // Fetch the units from the Options XML file.
    m_options = util::Options::GetInstance();

    m_options->GetOption("debug",m_debug);
    m_options->GetOption("verbose",m_verbose);

    // Unit definitions from G4SystemOfUnits.hh.
    std::string units;

    m_options->GetOption("TimeUnit",units);
    m_timeScale = ns;
    if ( units == "s" ) m_timeScale = second;
    if ( units == "ms" ) m_timeScale = millisecond;

    m_options->GetOption("LengthUnit",units);
    m_lengthScale = millimeter;
    if ( units == "cm" ) m_lengthScale = centimeter;

    m_options->GetOption("EnergyUnit",units);
    m_energyScale = MeV;
    if ( units == "GeV" ) m_energyScale = GeV;

    // Get the output file name, and the name of the tree we'll
    // create.
    m_options->GetOption("outputg4file",m_filename);
    m_options->GetOption("outputg4ntuple",m_treeName);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Destructor.
  WriteNtuplesAction::~WriteNtuplesAction() {
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // The user-action classes we'll need to define
  // the output file and write hits.

  void WriteNtuplesAction::BeginOfRunAction(const G4Run*) {

    // In multi-threaded running, there's some setup we have to do in
    // the master thread. If we don't have threads, we do that same
    // setup.

    auto threadID = G4Threading::G4GetThreadId();

    if (m_debug)
      G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	     << "starting threadID '" << threadID
	     << G4endl;

    if ( threadID == G4Threading::MASTER_ID   ||
	 threadID == G4Threading::SEQUENTIAL_ID ) {

      if (m_debug)
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "about to open file '" << m_filename
	       << "' for output" << G4endl;

      gSystem->Load("GramsDataObj/libGramsSimProjectDataObj.so");

      // Open the output file.
      s_file = new TFile(m_filename, "RECREATE");

      // Create the tree within the file.
      s_tree = new TTree(m_treeName, "GramsG4 MC Truth");

      // Define the branches within that tree.
      s_tree->Branch("EventID",  &m_eventID);
      s_tree->Branch("TrackList",&m_mcTrackList);
      s_tree->Branch("LArHits",  &m_mcLArHits);
      s_tree->Branch("ScintHits",&m_mcScintHits);

      // Write the options used to run this program. See
      // GramsSim/util/README.md for why we do this.
      m_options->WriteNtuple(s_file);

      if (m_debug)
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "in threadID '" << threadID
	       << "' resetting branch addresses"
	       << G4endl;

      // See the end of the EndOfRunAction code for why this is
      // needed.
      s_tree->ResetBranchAddresses();

    } // if master or sequential thread

    if (m_debug)
      G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	     << "at end of method for threadID '" << threadID
	     << G4endl;

  } // Begin of Run Action

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::EndOfRunAction(const G4Run*) {

    // Again, check that we're in the master thread.
    auto threadID = G4Threading::G4GetThreadId();

    if (m_debug)
      G4cout << "WriteNtuplesAction::EndOfRunAction() - "
	     << "at start of method for threadID '" << threadID
	     << G4endl;

    if ( threadID == G4Threading::MASTER_ID   ||
	 threadID == G4Threading::SEQUENTIAL_ID ) {

    if (m_debug)
      G4cout << "WriteNtuplesAction::EndOfRunAction() - "
	     << "about to close file in threadID '" << threadID
	     << G4endl;

      // Save the output tree and close the output file.
      s_tree->Write();
      s_file->Close();
    }

    if (m_debug)
      G4cout << "WriteNtuplesAction::EndOfRunAction() - "
	     << "at end of method for threadID '" << threadID
	     << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::BeginOfEventAction(const G4Event*) {
    // Clear out any previous values. Since this involves manipulating
    // STL containers, lock this method so that only one instance can
    // execute at a time.

    if (m_debug)
      G4cout << "WriteNtuplesAction::BeginOfEventAction() - "
	     << "at start of method for threadID '" << G4Threading::G4GetThreadId()
	     << G4endl;

    G4AutoLock lock(myMutex);
    delete m_mcTrackList;
    m_mcTrackList = new grams::MCTrackList();

    if (m_debug)
      G4cout << "WriteNtuplesAction::BeginOfEventAction() - "
	     << "at end of method for threadID '" << G4Threading::G4GetThreadId()
	     << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::EndOfEventAction(const G4Event* a_event) {

    // Convert the Geant4 information into the ROOT-based output
    // objects for the ntuple.

    if (m_debug)
      G4cout << "WriteNtuplesAction::EndOfEventAction() - "
	     << "at start of method for threadID '" << G4Threading::G4GetThreadId()
	     << G4endl;

    // Anything that involves STL (like all the ROOT-based classes,
    // which involve maps and sets) is not thread-safe, as is adding a
    // new row to an n-tuple. To avoid problems, lock this method so
    // that only one instance can execute at a time.
    G4AutoLock lock(myMutex);

    // Clear out any previous values.
    delete m_eventID;
    m_eventID = new grams::EventID( G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID(),
				    a_event->GetEventID() );
    delete m_mcLArHits;
    m_mcLArHits = new grams::MCLArHits;

    delete m_mcScintHits;
    m_mcScintHits = new grams::MCScintHits;

    // First, convert the energy deposits in the LAr. Get the Geant4
    // hit collection ID (only once)
    if ( m_LArHitCollectionID == -1 ) {
      // Make sure the following collection ID name agrees with that
      // in GramsG4DetectorConstruction.cc
      m_LArHitCollectionID
	= G4SDManager::GetSDMpointer()->GetCollectionID("LArHits");
    }

    // Get the collection of hits attached to this Geant4 event.
    auto LArHC 
      = GetHitsCollection<LArHitsCollection>(m_LArHitCollectionID, a_event);

    // For each hit in the collection...
    for ( size_t i = 0; i != LArHC->entries(); ++i ) {
      auto hit = (*LArHC)[i];

      // Create a new ROOT hit object.
      grams::MCLArHit mcLArHit;
      mcLArHit.trackID = hit->GetTrackID();
      mcLArHit.pdgCode = hit->GetPDGCode();
      mcLArHit.numPhotons = hit->GetNumPhotons();
      mcLArHit.cerPhotons = hit->GetCerPhotons();
      mcLArHit.volumeID = hit->GetIdentifier();
      mcLArHit.energy = hit->GetEnergy() / m_energyScale;
      mcLArHit.start = 
	ROOT::Math::XYZTVector( (hit->GetStartPosition()).x() / m_lengthScale,
			        (hit->GetStartPosition()).y() / m_lengthScale,
			        (hit->GetStartPosition()).z() / m_lengthScale,
			        hit->GetStartTime() / m_timeScale );
      mcLArHit.end = 
	ROOT::Math::XYZTVector( (hit->GetEndPosition()).x() / m_lengthScale,
			        (hit->GetEndPosition()).y() / m_lengthScale,
			        (hit->GetEndPosition()).z() / m_lengthScale,
			        hit->GetEndTime() / m_timeScale );
      // Add the new hit to the list.
      auto newHit = std::make_pair(mcLArHit.trackID, mcLArHit);
      m_mcLArHits->insert( newHit );

    } // For each LArHit

    // Do the same thing for the Scintillator hits.

    // Get the Geant4 hit collection ID (only once)
    if ( m_ScintillatorHitCollectionID == -1 ) {
      // Make sure the following collection ID name agrees with that
      // in GramsG4DetectorConstruction.cc
      m_ScintillatorHitCollectionID
	= G4SDManager::GetSDMpointer()->GetCollectionID("ScintillatorHits");
    }

    // Get the collection of hits attached to this Geant4 event.
    auto ScintillatorHC = 
      GetHitsCollection<ScintillatorHitsCollection>(m_ScintillatorHitCollectionID, a_event);

    // For each hit in the collection...
    for ( size_t i = 0; i != ScintillatorHC->entries(); ++i ) {
      auto hit = (*ScintillatorHC)[i];
      
      // Create a new ROOT hit object.
      grams::MCScintHit mcScintHit;
      mcScintHit.trackID = hit->GetTrackID();
      mcScintHit.pdgCode = hit->GetPDGCode();
      mcScintHit.volumeID = hit->GetIdentifier();
      mcScintHit.energy = hit->GetEnergy() / m_energyScale;
      mcScintHit.start = 
	ROOT::Math::XYZTVector( (hit->GetStartPosition()).x() / m_lengthScale,
			        (hit->GetStartPosition()).y() / m_lengthScale,
			        (hit->GetStartPosition()).z() / m_lengthScale,
			        hit->GetStartTime() / m_timeScale );
      mcScintHit.end = 
	ROOT::Math::XYZTVector( (hit->GetEndPosition()).x() / m_lengthScale,
			        (hit->GetEndPosition()).y() / m_lengthScale,
			        (hit->GetEndPosition()).z() / m_lengthScale,
			        hit->GetEndTime() / m_timeScale );
      // Add the new hit to the list.
      auto newHit = std::make_pair(mcScintHit.trackID, mcScintHit);
      m_mcScintHits->insert( newHit );

    } // For each ScintHit

    // For each track in m_mcTrackList, we've set the parent track
    // ID. Now we go through the list and fill in the daughter track
    // IDs.
    for ( auto& [ trackID, mcTrack ]: (*m_mcTrackList) ) {

      // Get the parent track ID for this track.
      auto parentID = mcTrack.ParentID();

      // The track "trackID" is the daughter of track
      // "parentID". Search for parentID in the track list. (Due to
      // energy cuts, it's possible for a daughter to exist without a
      // parent in the list, and vice versa.)
      auto search = m_mcTrackList->find(parentID);
      if ( search != m_mcTrackList->end() ) {

	// This is the MCTrack for the parentID.
	auto& parentTrack = (*search).second;

	// Add this track to the list of daughters for this parent.
	parentTrack.AddDaughter( trackID );

      } // search for parent
    } // for each track in list

    // Filling the tree. Within the Geant4 paradigm of master/worker
    // threads, it may not be clear that each thread has its own local
    // copies of the variables defined in the header. For ROOT, each
    // of those variables has a different address in each thread. So
    // we have to re-declare the branches every time we do a Fill(),
    // then reset the addresses.

    // If this is not clear, it may help to consult
    // `root-config`/tree/hvector.C

    s_tree->SetBranchAddress("EventID",  &m_eventID);
    s_tree->SetBranchAddress("TrackList",&m_mcTrackList);
    s_tree->SetBranchAddress("LArHits",  &m_mcLArHits);
    s_tree->SetBranchAddress("ScintHits",&m_mcScintHits);

    s_tree->Fill();

    s_tree->ResetBranchAddresses();

    if (m_debug)
      G4cout << "WriteNtuplesAction::EndOfEventAction() - "
	     << "at end of method for threadID '" << G4Threading::G4GetThreadId()
	     << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::PreTrackingAction (const G4Track* a_track) {

    if (m_debug)
      G4cout << "WriteNtuplesAction::PreTrackingAction() - "
	     << "at start of method for threadID '" << G4Threading::G4GetThreadId()
	     << G4endl;

    if (m_debug)
      G4cout << "WriteNtuplesAction::PreTrackingAction() - "
	     << "after mutex for threadID '" << G4Threading::G4GetThreadId()
	     << "', about to initialize a track " << G4endl;

    // Initialize a current track.
    m_mcTrack = grams::MCTrack();

    // Get the creator process. For primary particles the G4VProcess
    // object won't be created.
    auto process = a_track->GetCreatorProcess();
    G4String processName = "Primary";
    if ( process != NULL ) processName = process->GetProcessName();

    if (m_debug)
      G4cout << "WriteNtuplesAction::PreTrackingAction() - "
	     << "got process name for threadID '" << G4Threading::G4GetThreadId()
	     << "', process '" << processName << "'" << G4endl;

    // Fill in what values we can for the start of the track. 
    m_mcTrack.SetTrackID( a_track->GetTrackID() );
    m_mcTrack.SetParentID( a_track->GetParentID() );
    m_mcTrack.SetPDGCode( a_track->GetParticleDefinition()->GetPDGEncoding() );
    m_mcTrack.SetProcess ( processName );

    if (m_debug)
      G4cout << "WriteNtuplesAction::PreTrackingAction() - "
	     << "about to add trajectory point for threadID '" << G4Threading::G4GetThreadId()
	     << "'" << G4endl;

    // Record the starting four-vectors for this track's trajectory.
    AddTrajectoryPoint( a_track );

    if (m_debug)
      G4cout << "WriteNtuplesAction::PreTrackingAction() - "
	     << "at end of method for threadID '" << G4Threading::G4GetThreadId()
	     << "'" << G4endl;
  }

  void WriteNtuplesAction::PostTrackingAction(const G4Track*) {

    if (m_debug)
      G4cout << "WriteNtuplesAction::PostTrackingAction() - "
	     << "at start of method for threadID '" << G4Threading::G4GetThreadId()
	     << G4endl;

    // Lock this thread so that only one instance can execute at a
    // time, to avoid memory-management problems. 
    G4AutoLock lock(&myMutex);

    // std::map consists of (key,value) pairs. Construct such a pair
    // for this map, then insert it.
    auto newEntry = std::make_pair(m_mcTrack.TrackID(), m_mcTrack); 
    m_mcTrackList->insert( newEntry );

    if (m_debug)
      G4cout << "WriteNtuplesAction::PostTrackingAction() - "
	     << "at end of method for threadID '" << G4Threading::G4GetThreadId()
	     << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  template <class HC>
  HC* WriteNtuplesAction::GetHitsCollection(G4int hcID,
					    const G4Event* event) const
  {
    // Fetch the appropriate collection of hits from the current
    // event. 

    // Be careful with the structure returned by this routine. A
    // Geant4 hits collection (class G4THitsCollection) looks vaguely
    // like a vector (operator[] is defined), but it's not a vector
    // (no begin() or end() methods).
    
    auto hitsCollection 
      = static_cast<HC*>(event->GetHCofThisEvent()->GetHC(hcID));
    
    if ( ! hitsCollection ) {
      G4ExceptionDescription msg;
      msg << "Cannot access hitsCollection ID " << hcID; 
      G4Exception("gramsg4::WriteNtuplesAction::GetHitsCollection()",
		  "invalid hit collection", FatalException, msg);
    }         
    
    return hitsCollection;
  }    


  // Trajectory routines.

  // At each step, check if we should add a trajectory point.
  void WriteNtuplesAction::SteppingAction(const G4Step* a_step)
  {
    if (m_debug)
      G4cout << "WriteNtuplesAction::SteppingAction() - "
	     << "at start in thread '" << G4Threading::G4GetThreadId()
	     << "', about to get track" << G4endl;

    // Get the track this step is in.
    const G4Track* track = a_step->GetTrack();

    // If a particle is neutral, then each step marks a potential
    // change in trajectory and should have a trajectory point added.

    // However, if a particle is charged, then its step (at least in
    // the LAr) is limited; see GramsG4DetectorConstruction. We don't
    // want to save each tiny step as a trajectory point unless the
    // particle's direction changes.

    auto particle = track->GetParticleDefinition();
    auto charge = particle->GetPDGCharge();

    if (m_debug)
      G4cout << "WriteNtuplesAction::SteppingAction() - "
	     << "at start in thread '" << G4Threading::G4GetThreadId()
	     << "', about to test value of charge" << G4endl;

    if ( charge != 0.0 ) {
      // Is the momentum direction identical to that of the last
      // trajectory point?
      auto currentMomentumDirection = track->GetMomentumDirection();
      auto trajectory = m_mcTrack.Trajectory();

      // If the trajectory is empty, then we'll simply add the point.
      if ( ! trajectory.empty() ) {
	// We want to look at the last point of the trajectory; recall
	// that cend points to a location _after_ the last element in
	// our list.
	auto tpoint = trajectory.cend();
	tpoint--;
	
	// Compute a unit vector in the momentum direction of the last
	// trajectory point.
	G4ThreeVector lastMomentum( (*tpoint).X(), (*tpoint).Y(), (*tpoint).Z() );
	auto lastMomentumDirection = lastMomentum.unit();

	// Strictly speaking, what we want to test is if the current
	// momentum direction is equal to the last momentum
	// direction. However, testing for equality with floating-point
	// numbers is tricky on computer systems. Instead, let's check
	// if the different between their components is small.

	if (m_debug)
	  G4cout << "WriteNtuplesAction::SteppingAction() - "
		 << "at start in thread '" << G4Threading::G4GetThreadId()
		 << "', about to test if direction change is small" << G4endl;

	static const G4double small = 1.e-4;
	if ( std::abs( currentMomentumDirection.x() - lastMomentumDirection.x() ) < small
	     &&
	     std::abs( currentMomentumDirection.y() - lastMomentumDirection.y() ) < small
	     &&
	     std::abs( currentMomentumDirection.z() - lastMomentumDirection.z() ) < small )
	  return;

      } // is trajectory empty?
    } // is charge != 0?

    if (m_debug) 
      G4cout << "WriteNtuplesAction::SteppingAction - "
	     << "Current volume TrackID / Name / Identifier = " 
	     << track->GetTrackID() << " "
	     << track->GetVolume()->GetName() << " "
	     << track->GetVolume()->GetCopyNo() << G4endl;

    AddTrajectoryPoint(track);
  }

  void WriteNtuplesAction::AddTrajectoryPoint( const G4Track* a_track )
  {
    if (m_debug)
      G4cout << "WriteNtuplesAction::AddTrajectoryPoint() - "
	     << "adding trajectory point for threadID '" << G4Threading::G4GetThreadId()
	     << "'" << G4endl;

    ROOT::Math::XYZTVector position(
				    a_track->GetPosition().x() / m_lengthScale,
				    a_track->GetPosition().y() / m_lengthScale,
				    a_track->GetPosition().z() / m_lengthScale,
				    a_track->GetGlobalTime() / m_timeScale );
    ROOT::Math::XYZTVector momentum(
				    a_track->GetMomentum().x() / m_lengthScale,
				    a_track->GetMomentum().y() / m_lengthScale,
				    a_track->GetMomentum().z() / m_lengthScale,
				    a_track->GetTotalEnergy() / m_energyScale );
    

    // Anything that involves STL (like all the ROOT-based classes,
    // which involve maps and sets) is not thread-safe. To avoid
    // problems, lock this method so that only one instance can
    // execute at a time.
    G4AutoLock lock(myMutex);

    if (m_debug)
      G4cout << "WriteNtuplesAction::AddTrajectoryPoint() - "
	     << "inserting trajectory point for threadID '" << G4Threading::G4GetThreadId()
	     << "'" << G4endl;

    m_mcTrack.AddTrajectoryPoint ( position, 
				   momentum, 
				   a_track->GetVolume()->GetCopyNo() );
  }

} // namespace gramsg4
