/// \file GramsG4WriteHitsAction.cc
/// \brief User-action class to write out hits.

/// This is a user-action class that writes out the
/// event hits. At present, the output takes the form
/// of basic (and inefficient) ROOT n-tuple.

#include "GramsG4WriteHitsAction.hh"
#include "GramsG4LArHit.hh"

#include "Options.h" // in util/
#include "UserAction.h" // in g4util/
#include "Analysis.h" // in g4util/

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"

namespace gramsg4 {

  WriteHitsAction::WriteHitsAction()
    : UserAction()
    , m_LArHitCollectionID(-1)
  {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  WriteHitsAction::~WriteHitsAction() {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // The user-action classes we'll need to define
  // the output file and write hits.

  void WriteHitsAction::BeginOfRunAction(const G4Run*) {
    // Access (maybe create) the G4AnalysisManager.
    auto analysisManager = G4AnalysisManager::Instance();

    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);

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

    if (debug)
      G4cout << "WriteHitsAction::BeginOfRunAction() - "
	     << "about to open file '" << filename
	     << "' for output" << G4endl;

    auto openFile = analysisManager->OpenFile(filename);
    if ( ! openFile )
      G4cerr << "WriteHitsAction::BeginOfRunAction() - "
	     << " could not open file '" << filename
	     << "' for output" << G4endl;

    // Create ntuple. This one will automatically be assigned the id
    // number 0.
    auto result = analysisManager->CreateNtuple("LArHits", "LAr TPC energy deposits");
    if (debug) {
      G4cout << "WriteHitsAction::() - "
	     << "result from creating ntuple 'LArHits' = "
	     << result << G4endl;
      G4cout << "WriteHitsAction::BeginOfRunAction() - "
	     << "current ntuple ID = " << analysisManager->GetFirstNtupleId()
	     << G4endl;
    }

    m_LArNTID = analysisManager->GetFirstNtupleId();
    // Column IDs are automatically assigned.
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

    if (debug) 
	G4cout << "WriteHitsAction::BeginOfRunAction() - "
	       << "finish n-tuple"
	       << G4endl;
    analysisManager->FinishNtuple();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteHitsAction::EndOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();

    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);

    // Save the n-tuple.

    if (debug)
      G4cout << "WriteHitsAction::EndOfRunAction - "
	     << "about to write n-tuple file" << G4endl;
    analysisManager->Write();

    if (debug)
      G4cout << "WriteHitsAction::EndOfRunAction - "
	     << "about to close n-tuple file" << G4endl;
    analysisManager->CloseFile();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteHitsAction::BeginOfEventAction(const G4Event*) {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void WriteHitsAction::EndOfEventAction(const G4Event* a_event) {
    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);

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
      if (debug) 
	G4cout << "WriteHitsAction::EndOfEventAction - "
	       << "Filling n-tuple ID=" << m_LArNTID << G4endl;

      auto analysisManager = G4AnalysisManager::Instance();
      if (debug)
	G4cout << "WriteHitsAction::EndOfEventAction() - "
	       << "current ntuple ID = " << analysisManager->GetFirstNtupleId()
	       << G4endl;

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

      if (debug) 
	G4cout << "WriteHitsAction::EndOfEventAction - Adding row" << G4endl;
      analysisManager->AddNtupleRow(m_LArNTID);  
    }
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  LArHitsCollection* 
  WriteHitsAction::GetHitsCollection(G4int hcID,
				     const G4Event* event) const
  {
    // Fetch the appropriate collection of LArHits from the current
    // event.
    
    auto hitsCollection 
      = static_cast<LArHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
    
    if ( ! hitsCollection ) {
      G4ExceptionDescription msg;
      msg << "Cannot access hitsCollection ID " << hcID; 
      G4Exception("gramsg4::WriteHitsAction::GetHitsCollection()",
		  "invalid hit collection", FatalException, msg);
    }         
    
    return hitsCollection;
  }    

} // namespace gramsg4
