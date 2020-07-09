/// \file GramsG4WriteNtuplesAction.cc
/// \brief User-action class to write out ntuples.

/// This is a user-action class that writes out the event hits and
/// truth information. At present, the output takes the form of basic
/// (and inefficient) ROOT n-tuples.

#include "GramsG4WriteNtuplesAction.hh"
#include "GramsG4LArHit.hh"

#include "Options.h" // in util/
#include "UserAction.h" // in g4util/
#include "Analysis.h" // in g4util/

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"

namespace gramsg4 {

  WriteNtuplesAction::WriteNtuplesAction()
    : UserAction()
    , m_LArHitCollectionID(-1)
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
      G4cerr << "WriteNtuplesAction::BeginOfRunAction() - "
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

    if (m_debug) 
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "finish LArHits n-tuple"
	       << G4endl;
    analysisManager->FinishNtuple();

    // Create another ntuple.
    m_TrackNTID = analysisManager->CreateNtuple("TrackInfo", "MC truth G4Track information");
    if (m_debug) 
      G4cout << "WriteNtuplesAction::() - "
	     << "ntuple id of 'LArHits' = " << m_TrackNTID << G4endl;

    // Reminder: G4's units are MeV, mm, ns
    analysisManager->CreateNtupleIColumn("Run");            // id 0         
    analysisManager->CreateNtupleIColumn("Event");          // id 1
    analysisManager->CreateNtupleIColumn("TrackID");        // id 2
    analysisManager->CreateNtupleIColumn("ParentID");       // id 3
    analysisManager->CreateNtupleIColumn("PDGCode");        // id 4
    analysisManager->CreateNtupleSColumn("ProcessName");    // id 5
    analysisManager->CreateNtupleDColumn("tStart");         // id 6
    analysisManager->CreateNtupleDColumn("xStart");         // id 7
    analysisManager->CreateNtupleDColumn("yStart");         // id 8
    analysisManager->CreateNtupleDColumn("zStart");         // id 9
    analysisManager->CreateNtupleDColumn("EStart");         // id 10
    analysisManager->CreateNtupleSColumn("VolNameStart");   // id 11
    analysisManager->CreateNtupleDColumn("tEnd");           // id 12
    analysisManager->CreateNtupleDColumn("xEnd");           // id 13
    analysisManager->CreateNtupleDColumn("yEnd");           // id 14
    analysisManager->CreateNtupleDColumn("zEnd");           // id 15
    analysisManager->CreateNtupleDColumn("EEnd");           // id 16
    analysisManager->CreateNtupleSColumn("VolNameEnd");     // id 17

    if (m_debug) 
	G4cout << "WriteNtuplesAction::BeginOfRunAction() - "
	       << "finish TrackInfo n-tuple"
	       << G4endl;
    analysisManager->FinishNtuple();

  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::EndOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();

    // Save the n-tuple.

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
    // Get hits collections IDs (only once)
    if ( m_LArHitCollectionID == -1 ) {
      // Make sure the following name agrees with that in
      // GramsG4DetectorConstruction.cc
      m_LArHitCollectionID
	= G4SDManager::GetSDMpointer()->GetCollectionID("LArHits");
    }

    // Get hits collection
    auto LArHC = GetHitsCollection(m_LArHitCollectionID, a_event);

    G4int entries = LArHC->entries();
    // For each hit in the collection...
    for ( G4int i = 0; i != entries; ++i ) {
      auto larHit = (*LArHC)[i];

      // Fill a row in the n-tuple.
      if (m_debug) 
	G4cout << "WriteNtuplesAction::EndOfEventAction - "
	       << "Filling n-tuple ID=" << m_LArNTID << G4endl;

      auto analysisManager = G4AnalysisManager::Instance();
      analysisManager->FillNtupleIColumn(m_LArNTID, 0, G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID() );
      analysisManager->FillNtupleIColumn(m_LArNTID, 1, a_event->GetEventID() );
      analysisManager->FillNtupleIColumn(m_LArNTID, 2, larHit->GetTrackID() );
      analysisManager->FillNtupleIColumn(m_LArNTID, 3, larHit->GetPDGCode() );
      analysisManager->FillNtupleIColumn(m_LArNTID, 4, larHit->GetNumPhotons() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 5, larHit->GetEnergy() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 6, larHit->GetTime() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 7, (larHit->GetPosition()).x() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 8, (larHit->GetPosition()).y() );
      analysisManager->FillNtupleDColumn(m_LArNTID, 9, (larHit->GetPosition()).z() );

      if (m_debug) 
	G4cout << "WriteNtuplesAction::EndOfEventAction - Adding row" << G4endl;
      analysisManager->AddNtupleRow(m_LArNTID);  
    } // For each LArHit
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteNtuplesAction::PreTrackingAction (const G4Track* a_track) {
    if (m_debug) 
      G4cout << "WriteNtuplesAction::PreTrackingAction - Filling n-tuple ID = " 
	     << m_TrackNTID << G4endl;

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
    analysisManager->FillNtupleDColumn(m_TrackNTID, 6, a_track->GetGlobalTime() );
    analysisManager->FillNtupleDColumn(m_TrackNTID, 7, a_track->GetPosition().x() );
    analysisManager->FillNtupleDColumn(m_TrackNTID, 8, a_track->GetPosition().y() );
    analysisManager->FillNtupleDColumn(m_TrackNTID, 9, a_track->GetPosition().z() );
    analysisManager->FillNtupleDColumn(m_TrackNTID,10, a_track->GetTotalEnergy() );
    analysisManager->FillNtupleSColumn(m_TrackNTID,11, a_track->GetVolume()->GetName() );
}

  void WriteNtuplesAction::PostTrackingAction(const G4Track* a_track) {

    // Fill in the rest of the n-tuple values for the end of the track.
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleDColumn(m_TrackNTID,12, a_track->GetGlobalTime() );
    analysisManager->FillNtupleDColumn(m_TrackNTID,13, a_track->GetPosition().x() );
    analysisManager->FillNtupleDColumn(m_TrackNTID,14, a_track->GetPosition().y() );
    analysisManager->FillNtupleDColumn(m_TrackNTID,15, a_track->GetPosition().z() );
    analysisManager->FillNtupleDColumn(m_TrackNTID,16, a_track->GetTotalEnergy() );
    analysisManager->FillNtupleSColumn(m_TrackNTID,17, a_track->GetVolume()->GetName() );

    if (m_debug) 
      G4cout << "WriteNtuplesAction::PostTrackingAction - Adding row" << G4endl;
    analysisManager->AddNtupleRow(m_TrackNTID);  
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  LArHitsCollection* 
  WriteNtuplesAction::GetHitsCollection(G4int hcID,
				     const G4Event* event) const
  {
    // Fetch the appropriate collection of LArHits from the current
    // event.
    
    auto hitsCollection 
      = static_cast<LArHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
    
    if ( ! hitsCollection ) {
      G4ExceptionDescription msg;
      msg << "Cannot access hitsCollection ID " << hcID; 
      G4Exception("gramsg4::WriteNtuplesAction::GetHitsCollection()",
		  "invalid hit collection", FatalException, msg);
    }         
    
    return hitsCollection;
  }    

} // namespace gramsg4
