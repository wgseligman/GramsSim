/// \file GramsG4WriteNtuplesAction.cc
/// \brief User-action class to write out ntuples.

/// This is a user-action class that writes out the event hits and
/// truth information. At present, the output takes the form of basic
/// (and inefficient) ROOT n-tuples.

#include "GramsG4WriteNtuplesAction.hh"
#include "GramsG4LArHit.hh"
#include "GramsG4ScintillatorHit.hh"

#include "Options.h" // in util/
#include "UserAction.h" // in g4util/
#include "Analysis.h" // in g4util/

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4Threading.hh"

namespace gramsg4 {

  WriteNtuplesAction::WriteNtuplesAction()
    : UserAction()
    , m_LArHitCollectionID(-1)
    , m_ScintillatorHitCollectionID(-1)
  {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  WriteNtuplesAction::~WriteNtuplesAction() {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // The user-action classes we'll need to define
  // the output file and write hits.

  void WriteNtuplesAction::BeginOfRunAction(const G4Run*) {
    // Access (maybe create) the G4AnalysisManager.
    auto analysisManager = G4AnalysisManager::Instance();

    auto options = util::Options::GetInstance();
    options->GetOption("debug",m_debug);

    G4bool verbose;
    options->GetOption("verbose",verbose);
    if (verbose)
      analysisManager->SetVerboseLevel(1);

    // Without this, each execution thread writes its own file. It may
    // turn out that's what we want after all, but let's see. Note that
    // a consequence of merging files is that the events may not be in
    // order in the output file if you use multiple threads. 
    analysisManager->SetNtupleMerging(true);

    // Open the output file.
    G4String filename;
    options->GetOption("outputfile",filename);

    if (m_debug)
      G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	     << "about to open file '" << filename
	     << "' for output" << G4endl;

    auto openFile = analysisManager->OpenFile(filename);
    if ( ! openFile )
      G4cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << G4endl 
	     << "WriteNtuplesAction::BeginOfRunAction() - "
	     << " could not open file '" << filename
	     << "' for output" << G4endl;

    // Create ntuple.
    m_LArNTID = analysisManager->CreateNtuple("LArHits", "LAr TPC energy deposits");
    if (m_debug) 
      G4cout << "WriteNtuplesAction::() - "
	     << "ntuple id of 'LArHits' = " << m_LArNTID << G4endl;

    // Column IDs are automatically assigned.
    // Reminder: G4's units are MeV, mm, ns
    analysisManager->CreateNtupleIColumn("Run");        // id 0         
    analysisManager->CreateNtupleIColumn("Event");      // id 1
    analysisManager->CreateNtupleIColumn("TrackID");    // id 2
    analysisManager->CreateNtupleIColumn("PDGCode");    // id 3
    analysisManager->CreateNtupleIColumn("numPhotons"); // id 4
    analysisManager->CreateNtupleDColumn("energy");     // id 5
    analysisManager->CreateNtupleDColumn("time");       // id 6
    analysisManager->CreateNtupleDColumn("xPos");       // id 7
    analysisManager->CreateNtupleDColumn("yPos");       // id 8
    analysisManager->CreateNtupleDColumn("zPos");       // id 9
    analysisManager->CreateNtupleIColumn("identifier"); // id 10

    if (m_debug) 
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "finish LArHits n-tuple"
	       << G4endl;
    analysisManager->FinishNtuple();

    // Create another ntuple.
    m_ScintNTID = analysisManager->CreateNtuple("ScintillatorHits", "Scintillator energy deposits");
    // Column IDs are automatically assigned.
    // Reminder: G4's units are MeV, mm, ns
    analysisManager->CreateNtupleIColumn("Run");        // id 0         
    analysisManager->CreateNtupleIColumn("Event");      // id 1
    analysisManager->CreateNtupleIColumn("TrackID");    // id 2
    analysisManager->CreateNtupleIColumn("PDGCode");    // id 3
    analysisManager->CreateNtupleDColumn("energy");     // id 4
    analysisManager->CreateNtupleDColumn("time");       // id 5
    analysisManager->CreateNtupleDColumn("xPos");       // id 6
    analysisManager->CreateNtupleDColumn("yPos");       // id 7
    analysisManager->CreateNtupleDColumn("zPos");       // id 8
    analysisManager->CreateNtupleIColumn("identifier"); // id 9
    analysisManager->FinishNtuple();

    // Create yet another ntuple.
    m_TrackNTID = analysisManager->CreateNtuple("TrackInfo", "MC truth G4Track information");
    if (m_debug) 
      G4cout << "WriteNtuplesAction::() - "
	     << "ntuple id of 'TrackInfo' = " << m_TrackNTID << G4endl;


    // For tracking information, we're going to try to record the
    // trajectory of the track as it passes through the detector. A
    // trajectory is a set of two 4-vectors: (x,y,z,t) and
    // (px,py,pz,E). The Geant4 analysis manager won't let us store
    // 4-vectors directly, so store each components of these 4-vectors
    // in a separate std::vector.

    // Reminder: G4's units are MeV, mm, ns
    analysisManager->CreateNtupleIColumn("Run");                      // id 0         
    analysisManager->CreateNtupleIColumn("Event");                    // id 1
    analysisManager->CreateNtupleIColumn("TrackID");                  // id 2
    analysisManager->CreateNtupleIColumn("ParentID");                 // id 3
    analysisManager->CreateNtupleIColumn("PDGCode");                  // id 4
    analysisManager->CreateNtupleSColumn("ProcessName");              // id 5
    analysisManager->CreateNtupleDColumn("t", m_time);                // id 6
    analysisManager->CreateNtupleDColumn("x", m_xpos);                // id 7
    analysisManager->CreateNtupleDColumn("y", m_ypos);                // id 8
    analysisManager->CreateNtupleDColumn("z", m_zpos);                // id 9
    analysisManager->CreateNtupleDColumn("Etot", m_energy);           // id 10
    analysisManager->CreateNtupleDColumn("px", m_xmom);               // id 11
    analysisManager->CreateNtupleDColumn("py", m_ymom);               // id 12
    analysisManager->CreateNtupleDColumn("pz", m_zmom);               // id 13
    analysisManager->CreateNtupleIColumn("identifier", m_identifier); // id 14

    if (m_debug) 
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "finish TrackInfo n-tuple"
	       << G4endl;
    analysisManager->FinishNtuple();

    // Yet another ntuple: This contains the options used to run this
    // program. Why do this? Isn't there an options.xml file that
    // tells us which options were used? Not necessarily. For one
    // thing, the user may have overridden options using the command
    // line; this ntuple will record any such changes. Also, I find
    // that often in an analysis I keep many files created by many
    // jobs with many versions. If we store the exact options used to
    // generate a file, we have a better chance of recreating results.
    m_optionsNTID = analysisManager->CreateNtuple("Options", "Options used for this program");
    if (m_debug) 
      G4cout << "WriteNtuplesAction::() - "
	     << "ntuple id of 'Options' = " << m_optionsNTID << G4endl;

    analysisManager->CreateNtupleSColumn("OptionName");     // id 0
    analysisManager->CreateNtupleSColumn("OptionValue");    // id 1
    analysisManager->CreateNtupleSColumn("OptionType");     // id 2
    analysisManager->CreateNtupleSColumn("OptionBrief");    // id 3
    analysisManager->CreateNtupleSColumn("OptionDesc");     // id 4

    if (m_debug) 
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "finish Options n-tuple"
	       << G4endl;
    analysisManager->FinishNtuple();

    // In multi-threaded running, ntuples exist in worker threads, but
    // not in the main thread. If we try to fill the ntuple in the
    // master thread, we get lots of annoying (but harmless) error
    // messages. The following test makes sure we only fill the ntuple
    // if there are no threads (SEQUENTIAL_ID), or for a single worker
    // thread (WORKER_ID = 0) in a multi-threaded application.

    auto threadID = G4Threading::G4GetThreadId();
    if ( threadID == G4Threading::SEQUENTIAL_ID  ||  
	 threadID == G4Threading::WORKER_ID ) {

      // Write the options to the ntuple.
      auto numOptions = options->NumberOfOptions();
      if (m_debug) 
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "number of options = " << numOptions
	       << G4endl;
      for ( size_t i = 0; i != numOptions; ++i ) {
	if (m_debug) 
	  G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
		 << "filling for option number = " << i
		 << G4endl;
	analysisManager->FillNtupleSColumn(m_optionsNTID, 0, options->GetOptionName(i));
	analysisManager->FillNtupleSColumn(m_optionsNTID, 1, options->GetOptionValue(i));
	analysisManager->FillNtupleSColumn(m_optionsNTID, 2, options->GetOptionType(i));
	analysisManager->FillNtupleSColumn(m_optionsNTID, 3, options->GetOptionBrief(i));
	analysisManager->FillNtupleSColumn(m_optionsNTID, 4, options->GetOptionDescription(i));
	analysisManager->AddNtupleRow(m_optionsNTID);  
      }
    } // if sequential or worker thread
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::EndOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();

    // Save the n-tuples.

    if (m_debug)
      G4cout << "WriteNtuplesAction::EndOfRunAction - "
	     << "about to write n-tuple file" << G4endl;
    analysisManager->Write();

    if (m_debug)
      G4cout << "WriteNtuplesAction::EndOfRunAction - "
	     << "about to close n-tuple file" << G4endl;
    analysisManager->CloseFile();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::BeginOfEventAction(const G4Event*) {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::EndOfEventAction(const G4Event* a_event) {

    auto analysisManager = G4AnalysisManager::Instance();

    // Get hits collections IDs (only once)
    if ( m_LArHitCollectionID == -1 ) {
      // Make sure the following collection ID name agrees with that
      // in GramsG4DetectorConstruction.cc
      m_LArHitCollectionID
	= G4SDManager::GetSDMpointer()->GetCollectionID("LArHits");
    }

    // Get hits collection
    auto LArHC 
      = GetHitsCollection<LArHitsCollection>(m_LArHitCollectionID, a_event);

    G4int entries = LArHC->entries();
    // For each hit in the collection...
    for ( G4int i = 0; i != entries; ++i ) {
      auto hit = (*LArHC)[i];

      // Fill a row in the n-tuple.
      if (m_debug) 
	G4cout << "WriteNtuplesAction::EndOfEventAction - "
	       << "Filling n-tuple ID=" << m_LArNTID << G4endl;

      analysisManager->FillNtupleIColumn(m_LArNTID, 0, G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID() );
      analysisManager->FillNtupleIColumn(m_LArNTID, 1, a_event->GetEventID() );
      analysisManager->FillNtupleIColumn(m_LArNTID, 2, hit->GetTrackID() );
      analysisManager->FillNtupleIColumn(m_LArNTID, 3, hit->GetPDGCode() );
      analysisManager->FillNtupleIColumn(m_LArNTID, 4, hit->GetNumPhotons() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 5, hit->GetEnergy() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 6, hit->GetTime() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 7, (hit->GetPosition()).x() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 8, (hit->GetPosition()).y() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 9, (hit->GetPosition()).z() );
      analysisManager->FillNtupleIColumn(m_LArNTID,10, hit->GetIdentifier() );

      if (m_debug) 
	G4cout << "WriteNtuplesAction::EndOfEventAction - Adding LAr row" << G4endl;
      analysisManager->AddNtupleRow(m_LArNTID);  
    } // For each LArHit

    // *
    // Do the same thing for the Scintillator hits.

    // Get hits collections IDs (only once)
    if ( m_ScintillatorHitCollectionID == -1 ) {
      // Make sure the following collection ID name agrees with that
      // in GramsG4DetectorConstruction.cc
      m_ScintillatorHitCollectionID
	= G4SDManager::GetSDMpointer()->GetCollectionID("ScintillatorHits");
    }

    // Get hits collection
    auto ScintillatorHC = 
      GetHitsCollection<ScintillatorHitsCollection>(m_ScintillatorHitCollectionID, a_event);

    entries = ScintillatorHC->entries();
    // For each hit in the collection...
    for ( G4int i = 0; i != entries; ++i ) {
      auto hit = (*ScintillatorHC)[i];

      // Fill a row in the n-tuple.
      analysisManager->FillNtupleIColumn(m_ScintNTID, 0, G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID() );
      analysisManager->FillNtupleIColumn(m_ScintNTID, 1, a_event->GetEventID() );
      analysisManager->FillNtupleIColumn(m_ScintNTID, 2, hit->GetTrackID() );
      analysisManager->FillNtupleIColumn(m_ScintNTID, 3, hit->GetPDGCode() );
      analysisManager->FillNtupleDColumn(m_ScintNTID, 4, hit->GetEnergy() );
      analysisManager->FillNtupleDColumn(m_ScintNTID, 5, hit->GetTime() );
      analysisManager->FillNtupleDColumn(m_ScintNTID, 6, (hit->GetPosition()).x() );
      analysisManager->FillNtupleDColumn(m_ScintNTID, 7, (hit->GetPosition()).y() );
      analysisManager->FillNtupleDColumn(m_ScintNTID, 8, (hit->GetPosition()).z() );
      analysisManager->FillNtupleIColumn(m_ScintNTID, 9, hit->GetIdentifier() );

      if (m_debug) {
	G4cout << "WriteNtuplesAction::EndOfEventAction - ScintID=" 
	       << hit->GetIdentifier() << G4endl;
      }

      analysisManager->AddNtupleRow(m_ScintNTID);  
    } // For each ScintillatorHit
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::PreTrackingAction (const G4Track* a_track) {
    if (m_debug) 
      G4cout << "WriteNtuplesAction::PreTrackingAction - Filling n-tuple ID = " 
	     << m_TrackNTID << G4endl;

    ClearTrajectory();

    // Get the creator process. For primary particles the G4VProcess
    // object won't be created.
    auto process = a_track->GetCreatorProcess();
    G4String processName = "Primary";
    if ( process != NULL ) processName = process->GetProcessName();

    // Fill in what values we can for the start of the track. 

    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleIColumn(m_TrackNTID, 0, G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID() );
    analysisManager->FillNtupleIColumn(m_TrackNTID, 1, G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() );
    analysisManager->FillNtupleIColumn(m_TrackNTID, 2, a_track->GetTrackID() );
    analysisManager->FillNtupleIColumn(m_TrackNTID, 3, a_track->GetParentID() );
    analysisManager->FillNtupleIColumn(m_TrackNTID, 4, a_track->GetParticleDefinition()->GetPDGEncoding() );
    analysisManager->FillNtupleSColumn(m_TrackNTID, 5, processName );

    if (m_debug) 
      G4cout << "WriteNtuplesAction::PreTrackingAction - "
	     << "Starting volume TrackID / Name / Identifier = " 
	     << a_track->GetTrackID() << " "
	     << a_track->GetVolume()->GetName() << " "
	     << a_track->GetVolume()->GetCopyNo() << G4endl;

    // Record the starting four-vectors for this track's trajectory.
    AddTrajectoryPoint( a_track );
}

  void WriteNtuplesAction::PostTrackingAction(const G4Track*) {

    // Fill in the rest of the n-tuple values for the end of the track.
    auto analysisManager = G4AnalysisManager::Instance();

    // We don't have to explictly "fill" the columns that were defined
    // by std::vectors. The G4 Analysis Manager has stored their
    // addresses and will take care of that for us.

    if (m_debug) 
      G4cout << "WriteNtuplesAction::PostTrackingAction - Adding row" << G4endl;

    analysisManager->AddNtupleRow(m_TrackNTID);  
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  template <class HC>
  HC* WriteNtuplesAction::GetHitsCollection(G4int hcID,
					    const G4Event* event) const
  {
    // Fetch the appropriate collection of hits from the
    // current event.
    
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

  // At each step, check if we should add a trajector point.
  void WriteNtuplesAction::SteppingAction(const G4Step* a_step)
  {
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

    if ( charge != 0.0 ) {
      // Is the momentum direction identical to that of the last
      // trajectory point?
      auto currentMomentumDirection = track->GetMomentumDirection();

      G4ThreeVector lastMomentum( m_xmom.back(), m_ymom.back(), m_zmom.back() );
      auto lastMomentumDirection = lastMomentum.unit();

      // Strictly speaking, what we want to test is if the current
      // momentum direction is equal to the last momentum
      // direction. However, testing for equality with floating-point
      // numbers is tricky on computer systems. Instead, let's check
      // if the different between their components is small.

      const G4double small = 1.e-4;
      if ( std::abs( currentMomentumDirection.x() - lastMomentumDirection.x() ) < small
	   &&
	   std::abs( currentMomentumDirection.y() - lastMomentumDirection.y() ) < small
	   &&
	   std::abs( currentMomentumDirection.z() - lastMomentumDirection.z() ) < small )
	return;
    }

    if (m_debug) 
      G4cout << "WriteNtuplesAction::SteppingAction - "
	     << "Current volume TrackID / Name / Identifier = " 
	     << track->GetTrackID() << " "
	     << track->GetVolume()->GetName() << " "
	     << track->GetVolume()->GetCopyNo() << G4endl;

    AddTrajectoryPoint(track);
  }

  void WriteNtuplesAction::ClearTrajectory() {
    // Clear all the trajectory vectors. 
    m_time.clear();
    m_xpos.clear();
    m_ypos.clear();
    m_zpos.clear();
    m_energy.clear();
    m_xmom.clear();
    m_ymom.clear();
    m_zmom.clear();
    m_identifier.clear();
  }

  size_t WriteNtuplesAction::AddTrajectoryPoint( const G4Track* a_track )
  {
    // Add the components of our two 4-vectors (t,x,y,z) (E,px,py,pz)
    // to the individual std::vectors.
    m_time.push_back( a_track->GetGlobalTime() );
    m_xpos.push_back( a_track->GetPosition().x() );
    m_ypos.push_back( a_track->GetPosition().y() );
    m_zpos.push_back( a_track->GetPosition().z() );
    m_energy.push_back( a_track->GetTotalEnergy() );
    m_xmom.push_back( a_track->GetMomentum().x() );
    m_ymom.push_back( a_track->GetMomentum().y() );
    m_zmom.push_back( a_track->GetMomentum().z() );
    m_identifier.push_back( a_track->GetVolume()->GetCopyNo() );

    // All the std::vectors should be the same size, so pick one and
    // return its size.
    return m_time.size();
  }

} // namespace gramsg4
