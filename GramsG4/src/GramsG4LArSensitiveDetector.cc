/// \file GramsG4LArSensitiveDetector.cc
/// \brief Implementation of the LArSensitiveDetector class

/// Much of this code was copied from Geant4 example B2a.

#include "GramsG4LArSensitiveDetector.hh"
#include "GramsG4LArHit.hh"
#include "Options.h"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
#include "G4Scintillation.hh"
#include "G4Cerenkov.hh"
#include "G4Exception.hh"
#include "G4ios.hh"

namespace gramsg4 {

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  LArSensitiveDetector::LArSensitiveDetector(const G4String& name,
					     const G4String& hitsCollectionName) 
    : G4VSensitiveDetector(name),
      m_hitsCollection(NULL)
  {
    collectionName.insert(hitsCollectionName);
  }


  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void LArSensitiveDetector::Initialize(G4HCofThisEvent* a_hce)
  {
    // Create hits collection
    m_hitsCollection 
      = new LArHitsCollection(SensitiveDetectorName, collectionName[0]); 

    // Add this collection in "hits collection of this event" (HCE).
    G4int hcID 
      = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    a_hce->AddHitsCollection( hcID, m_hitsCollection ); 
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  G4bool LArSensitiveDetector::ProcessHits(G4Step* aStep, 
					   G4TouchableHistory*)
  {  
    // energy deposit
    G4double edep = aStep->GetTotalEnergyDeposit();

    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);
    if (debug) {
      G4cout << "LArSensitiveDetector::ProcessHits - energy="
	     << edep
	     << " track=" << aStep->GetTrack()->GetTrackID()
	     << G4endl;
    }

    if (edep <= 0.) return false;

    // The following code was copied from LArSoft:
    // https://github.com/LArSoft/larg4/blob/develop/larg4/Services/SimEnergyDepositSD.cc

    G4int sphotons = 0; // scintillation photons
    G4int cphotons = 0; // cerenkov photons

    G4SteppingManager* fpSteppingManager = G4EventManager::GetEventManager()
      ->GetTrackingManager()->GetSteppingManager();
    G4StepStatus stepStatus = fpSteppingManager->GetfStepStatus();

    // If the particle is not at rest...
    if (stepStatus != fAtRestDoItProc) {

      // Look at all the physics processes that were associated with this step.
      G4ProcessVector* procPost = fpSteppingManager->GetfPostStepDoItVector();
      size_t MAXofPostStepLoops = fpSteppingManager->GetMAXofPostStepLoops();

      // For each of the processes associated with this step...
      for (size_t i3 = 0; i3 < MAXofPostStepLoops; i3++) {

	// The following code was copied from
	// https://github.com/hanswenzel/CaTS_legacy/blob/master/src/lArTPCSD.cc

	// If the process is Cerenkov, accumulate the cerenkov photons. 
	if((*procPost)[i3]->GetProcessName() == "Cerenkov") {
	  G4Cerenkov* process = (G4Cerenkov*) (*procPost)[i3];
	  auto photons = process->GetNumPhotons();
	  cphotons += photons;
	} // if cerenkov photons

	// If the process is Scintillation, accumulate the scintillation photons. 
	if ((*procPost)[i3]->GetProcessName() == "Scintillation") {
	  G4Scintillation* process = (G4Scintillation*) (*procPost)[i3];
	  auto photons = process->GetNumPhotons(); 
	  sphotons += photons;
	} // if scintillation photons

      } // loop over MAXofPostStepLoops
    } // if particle not at rest
    
    auto start = aStep->GetPreStepPoint()->GetPosition();
    auto end   = aStep->GetPostStepPoint()->GetPosition();

    auto tstart = aStep->GetPreStepPoint()->GetGlobalTime();
    auto tend   = aStep->GetPostStepPoint()->GetGlobalTime();

    LArHit* newHit = new LArHit(
	aStep->GetTrack()->GetTrackID(),
	aStep->GetTrack()->
		GetDynamicParticle()->
		GetParticleDefinition()->
		GetPDGEncoding(),
	sphotons,
	cphotons,
	edep,
	tstart,
	tend,
	start,
	end,
	aStep->GetTrack()->GetVolume()->GetCopyNo()
				);

    m_hitsCollection->insert( newHit );

    return true;
  }


  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void LArSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
  {
    auto options = util::Options::GetInstance();
    G4bool verbose;
    options->GetOption("verbose",verbose);

    if ( verbose ) { 
      G4int nofHits = m_hitsCollection->entries();
      if ( nofHits == 0 )
	G4cout << G4endl
	       << "LArSensitiveDetector::EndOfEvent: in this event there was no"
	       << " energy recorded in the LAr" << G4endl;
      else {
	G4cout << G4endl
	       << "LArSensitiveDetector::EndOfEvent: in this event there are " << nofHits 
	       << " hits in the LAr: " << G4endl;
	for ( G4int i=0; i<nofHits; i++ ) (*m_hitsCollection)[i]->Print();
      } // display hits
    } // if verbose
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace gramsg4
