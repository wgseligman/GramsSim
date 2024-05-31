/// \file GramsG4WriteNtuplesAction.cc
/// \brief User-action class to write out ntuples.

/// This is a user-action class that writes out the event hits and
/// truth information. At present, the output takes the form of basic
/// ROOT n-tuples.

#include "GramsG4WriteNtuplesAction.hh"
#include "GramsG4LArHit.hh"
#include "GramsG4ScintillatorHit.hh"
#include "G4SystemOfUnits.hh"

#include "Options.h" // in util/
#include "UserAction.h" // in g4util/
#include "Analysis.h" // in g4util/

// In GramsDataObj
#include "EventID.h"
#include "MCTrackList.h"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"

#include <string>

namespace gramsg4 {

  // In a multi-threaded application, there's an issue: std::vector
  // operations are not thread-safe. Vectors can reallocate their
  // memory, and we get into trouble when two or more vectors try to
  // reallocate memory at once.

  // To get around this, use G4's locking mechanism (which is
  // basically a wrapper around the standard C++ mutex). See
  // $G4INSTALL/include/G4AutoLock.hh for details.
  static G4Mutex myMutex;

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Default constructor. 
  WriteNtuplesAction::WriteNtuplesAction()
    : UserAction()
    , m_LArHitCollectionID(-1)
    , m_ScintillatorHitCollectionID(-1)
  {
    // Fetch the units from the Options XML file.
    m_options = util::Options::GetInstance();

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

  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Destructor.
  WriteNtuplesAction::~WriteNtuplesAction() {
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // The user-action classes we'll need to define
  // the output file and write hits.

  void WriteNtuplesAction::BeginOfRunAction(const G4Run*) {
    // Access (maybe create) the G4AnalysisManager.
    auto analysisManager = G4AnalysisManager::Instance();

    m_options->GetOption("debug",m_debug);

    G4bool verbose;
    m_options->GetOption("verbose",verbose);
    if (verbose)
      analysisManager->SetVerboseLevel(1);

    // Without this, each execution thread writes its own file, or
    // perhaps each event will have its own set of ntuples. It may
    // turn out that's what we want after all, but let's see. Note
    // that a consequence of merging files is that the events may not
    // be in order in the output file if you use multiple threads.
    analysisManager->SetNtupleMerging(true);

    // Get the output file name.
    m_options->GetOption("outputfile",m_filename);

    if (m_debug)
      G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	     << "about to open file '" << m_filename
	     << "' for output" << G4endl;

    auto openFile = analysisManager->OpenFile(m_filename);
    if ( ! openFile )
      G4cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << G4endl 
	     << "WriteNtuplesAction::BeginOfRunAction() - "
	     << " could not open file '" << m_filename
	     << "' for output" << G4endl;

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

    // Reminder: Units are defined via the Options XML file.
    analysisManager->CreateNtupleIColumn("Run");                      // id 0         
    analysisManager->CreateNtupleIColumn("Event");                    // id 1
    analysisManager->CreateNtupleIColumn("TrackID");                  // id 2
    analysisManager->CreateNtupleIColumn("ParentID");                 // id 3
    analysisManager->CreateNtupleIColumn("PDGCode");                  // id 4
    analysisManager->CreateNtupleSColumn("ProcessName");              // id 5
    analysisManager->CreateNtupleDColumn("t", m_time);                // id 6
    analysisManager->CreateNtupleFColumn("x", m_xpos);                // id 7
    analysisManager->CreateNtupleFColumn("y", m_ypos);                // id 8
    analysisManager->CreateNtupleFColumn("z", m_zpos);                // id 9
    analysisManager->CreateNtupleDColumn("Etot", m_energy);           // id 10
    analysisManager->CreateNtupleFColumn("px", m_xmom);               // id 11
    analysisManager->CreateNtupleFColumn("py", m_ymom);               // id 12
    analysisManager->CreateNtupleFColumn("pz", m_zmom);               // id 13
    analysisManager->CreateNtupleIColumn("identifier", m_identifier); // id 14

    if (m_debug) 
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "finish TrackInfo n-tuple"
	       << G4endl;
    analysisManager->FinishNtuple();

    // Create another ntuple.
    m_LArNTID = analysisManager->CreateNtuple("LArHits", "LAr TPC energy deposits");
    if (m_debug) 
      G4cout << "WriteNtuplesAction::() - "
	     << "ntuple id of 'LArHits' = " << m_LArNTID << G4endl;

    // The quantities written are based on
    // https://github.com/LArSoft/lardataobj/blob/develop/lardataobj/Simulation/SimEnergyDeposit.h

    // Column IDs are automatically assigned.
    // Reminder: G4's units are MeV, mm, ns
    analysisManager->CreateNtupleIColumn("Run");        // id 0         
    analysisManager->CreateNtupleIColumn("Event");      // id 1
    analysisManager->CreateNtupleIColumn("TrackID");    // id 2
    analysisManager->CreateNtupleIColumn("PDGCode");    // id 3
    analysisManager->CreateNtupleIColumn("numPhotons"); // id 4
    analysisManager->CreateNtupleIColumn("cerPhotons"); // id 5
    analysisManager->CreateNtupleDColumn("energy");     // id 6
    analysisManager->CreateNtupleDColumn("tStart");     // id 7
    analysisManager->CreateNtupleFColumn("xStart");     // id 8
    analysisManager->CreateNtupleFColumn("yStart");     // id 9
    analysisManager->CreateNtupleFColumn("zStart");     // id 10
    analysisManager->CreateNtupleDColumn("tEnd");       // id 11
    analysisManager->CreateNtupleFColumn("xEnd");       // id 12
    analysisManager->CreateNtupleFColumn("yEnd");       // id 13
    analysisManager->CreateNtupleFColumn("zEnd");       // id 14
    analysisManager->CreateNtupleIColumn("identifier"); // id 15

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
    analysisManager->CreateNtupleDColumn("tStart");     // id 5
    analysisManager->CreateNtupleFColumn("xStart");     // id 6
    analysisManager->CreateNtupleFColumn("yStart");     // id 7
    analysisManager->CreateNtupleFColumn("zStart");     // id 8
    analysisManager->CreateNtupleDColumn("tEnd");       // id 9
    analysisManager->CreateNtupleFColumn("xEnd");       // id 10
    analysisManager->CreateNtupleFColumn("yEnd");       // id 11
    analysisManager->CreateNtupleFColumn("zEnd");       // id 12
    analysisManager->CreateNtupleIColumn("identifier"); // id 13
    analysisManager->FinishNtuple();

    // Yet another ntuple: This contains the options used to run this
    // program. 

    // Why do this? Isn't there an options.xml file that tells us
    // which options were used? Not necessarily. For one thing, the
    // user may have overridden options using the command line; this
    // ntuple will record any such changes.

    // Also, I find that often in an analysis I keep many files
    // created by many jobs with many versions. If we store the exact
    // options used to generate a file, we have a better chance of
    // recreating results.

    m_optionsNTID = analysisManager->CreateNtuple("Options", "Options used for this program");
    if (m_debug) 
      G4cout << "WriteNtuplesAction::() - "
             << "ntuple id of 'Options' = " << m_optionsNTID << G4endl;

    analysisManager->CreateNtupleSColumn("OptionName");     // id 0
    analysisManager->CreateNtupleSColumn("OptionValue");    // id 1
    analysisManager->CreateNtupleSColumn("OptionType");     // id 2
    analysisManager->CreateNtupleSColumn("OptionBrief");    // id 3
    analysisManager->CreateNtupleSColumn("OptionDesc");     // id 4
    analysisManager->CreateNtupleSColumn("OptionSource");   // id 5
    analysisManager->CreateNtupleSColumn("OptionLow");      // id 6
    analysisManager->CreateNtupleSColumn("OptionHigh");     // id 7

    if (m_debug) 
      G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	     << "finish Options n-tuple"
	     << G4endl;
    analysisManager->FinishNtuple();

    // In multi-threaded running, ntuples exist in worker threads, but
    // not in the main thread. If we try to fill the options ntuple in
    // the master thread, we get lots of annoying (but harmless) error
    // messages. The following test makes sure we only fill the ntuple
    // if there are no threads (SEQUENTIAL_ID), or for a single worker
    // thread (ID == 0) in a multi-threaded application.

    auto threadID = G4Threading::G4GetThreadId();
    if ( threadID == G4Threading::SEQUENTIAL_ID  ||  
         threadID == 0 ) {

      // Write the options to the ntuple.
      auto numOptions = m_options->NumberOfOptions();
      if (m_debug) 
        G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
               << "number of options = " << numOptions
               << G4endl;
      for ( size_t i = 0; i != numOptions; ++i ) {
        if (m_debug) 
          G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
                 << "filling for option number = " << i
                 << G4endl;
        analysisManager->FillNtupleSColumn(m_optionsNTID, 0, m_options->GetOptionName(i));
        analysisManager->FillNtupleSColumn(m_optionsNTID, 1, m_options->GetOptionValue(i));
        analysisManager->FillNtupleSColumn(m_optionsNTID, 2, m_options->GetOptionType(i));
        analysisManager->FillNtupleSColumn(m_optionsNTID, 3, m_options->GetOptionBrief(i));
        analysisManager->FillNtupleSColumn(m_optionsNTID, 4, m_options->GetOptionDescription(i));
        analysisManager->FillNtupleSColumn(m_optionsNTID, 5, m_options->GetOptionSource(i));
        analysisManager->FillNtupleSColumn(m_optionsNTID, 6, m_options->GetOptionLow(i));
        analysisManager->FillNtupleSColumn(m_optionsNTID, 7, m_options->GetOptionHigh(i));
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
	     << "about to close n-tuple file '" 
	     << m_filename << "'"
	     << G4endl;
    analysisManager->CloseFile();

    if (m_debug)
      G4cout << "WriteNtuplesAction::EndOfRunAction - "
	     << "about to clear analysisManager" 
	     << G4endl;
    analysisManager->Clear();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::BeginOfEventAction(const G4Event*) {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::EndOfEventAction(const G4Event* a_event) {

    auto analysisManager = G4AnalysisManager::Instance();

    // Get hit collection ID (only once)
    if ( m_LArHitCollectionID == -1 ) {
      // Make sure the following collection ID name agrees with that
      // in GramsG4DetectorConstruction.cc
      m_LArHitCollectionID
	= G4SDManager::GetSDMpointer()->GetCollectionID("LArHits");
    }

    // Get collection of hits
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
      analysisManager->FillNtupleIColumn(m_LArNTID, 5, hit->GetCerPhotons() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 6, hit->GetEnergy() / m_energyScale );
      analysisManager->FillNtupleDColumn(m_LArNTID, 7, hit->GetStartTime() / m_timeScale );
      analysisManager->FillNtupleFColumn(m_LArNTID, 8, (hit->GetStartPosition()).x() / m_lengthScale );
      analysisManager->FillNtupleFColumn(m_LArNTID, 9, (hit->GetStartPosition()).y() / m_lengthScale );
      analysisManager->FillNtupleFColumn(m_LArNTID,10, (hit->GetStartPosition()).z() / m_lengthScale );
      analysisManager->FillNtupleDColumn(m_LArNTID,11, hit->GetEndTime() / m_timeScale);
      analysisManager->FillNtupleFColumn(m_LArNTID,12, (hit->GetEndPosition()).x() / m_lengthScale );
      analysisManager->FillNtupleFColumn(m_LArNTID,13, (hit->GetEndPosition()).y() / m_lengthScale );
      analysisManager->FillNtupleFColumn(m_LArNTID,14, (hit->GetEndPosition()).z() / m_lengthScale );
      analysisManager->FillNtupleIColumn(m_LArNTID,15, hit->GetIdentifier() );

      if (m_debug) 
	G4cout << "WriteNtuplesAction::EndOfEventAction - Adding LAr row" << G4endl;
      analysisManager->AddNtupleRow(m_LArNTID);  
    } // For each LArHit

    // Do the same thing for the Scintillator hits.

    // Get hit collection ID (only once)
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
      analysisManager->FillNtupleDColumn(m_ScintNTID, 4, hit->GetEnergy() / m_energyScale );
      analysisManager->FillNtupleDColumn(m_ScintNTID, 5, hit->GetStartTime() / m_timeScale );
      analysisManager->FillNtupleFColumn(m_ScintNTID, 6, (hit->GetStartPosition()).x() / m_lengthScale );
      analysisManager->FillNtupleFColumn(m_ScintNTID, 7, (hit->GetStartPosition()).y() / m_lengthScale );
      analysisManager->FillNtupleFColumn(m_ScintNTID, 8, (hit->GetStartPosition()).z() / m_lengthScale );
      analysisManager->FillNtupleDColumn(m_ScintNTID, 9, hit->GetEndTime() / m_timeScale );
      analysisManager->FillNtupleFColumn(m_ScintNTID,10, (hit->GetEndPosition()).x() / m_lengthScale );
      analysisManager->FillNtupleFColumn(m_ScintNTID,11, (hit->GetEndPosition()).y() / m_lengthScale );
      analysisManager->FillNtupleFColumn(m_ScintNTID,12, (hit->GetEndPosition()).z() / m_lengthScale );
      analysisManager->FillNtupleIColumn(m_ScintNTID,13, hit->GetIdentifier() );

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

    // It appears that the analysis manager has a problem writing
    // vectors in a multi-threaded environment. Within the scope of
    // the braces, G4AutoLock will make sure only a single thread can
    // execute this block of code.
    {
      G4AutoLock lock(&myMutex);
      analysisManager->AddNtupleRow(m_TrackNTID);  
    }
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

  // At each step, check if we should add a trajectory point.
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

      static const G4double small = 1.e-4;
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
    // There's a small chance that std::vector::clear might cause a
    // memory reallocation, so lock the following code so only one
    // thread can execute it.
    G4AutoLock lock(myMutex);

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

  void WriteNtuplesAction::AddTrajectoryPoint( const G4Track* a_track )
  {
    // Lock the following code so only one thread can execute it.
    G4AutoLock lock(myMutex);

    // Add the components of our two 4-vectors (t,x,y,z) (E,px,py,pz)
    // to the individual std::vectors.
    m_time.push_back( a_track->GetGlobalTime() / m_timeScale );
    m_xpos.push_back( a_track->GetPosition().x() / m_lengthScale );
    m_ypos.push_back( a_track->GetPosition().y() / m_lengthScale );
    m_zpos.push_back( a_track->GetPosition().z() / m_lengthScale );
    m_energy.push_back( a_track->GetTotalEnergy() / m_energyScale );
    m_xmom.push_back( a_track->GetMomentum().x() / m_energyScale );
    m_ymom.push_back( a_track->GetMomentum().y() / m_energyScale );
    m_zmom.push_back( a_track->GetMomentum().z() / m_energyScale );
    m_identifier.push_back( a_track->GetVolume()->GetCopyNo() );
  }

} // namespace gramsg4
