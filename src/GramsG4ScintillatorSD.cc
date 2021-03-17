/// \file GramsG4ScintillatorSD.cc
/// \brief Implementation of the ScintillatorSD class

/// Much of this code was copied from Geant4 example B2a.

#include "GramsG4ScintillatorSD.hh"
#include "Options.h"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
#include "G4Scintillation.hh"
#include "G4Exception.hh"
#include "G4ios.hh"

namespace gramsg4 {

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  ScintillatorSD::ScintillatorSD(const G4String& name,
				 const G4String& hitsCollectionName) 
    : G4VSensitiveDetector(name),
      m_hitsCollection(NULL)
  {
    collectionName.insert(hitsCollectionName);
  }


  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void ScintillatorSD::Initialize(G4HCofThisEvent* a_hce)
  {
    // Create hits collection
    m_hitsCollection 
      = new ScintillatorHitsCollection(SensitiveDetectorName, collectionName[0]); 

    // Add this collection in "hits collection of this event" (HCE).
    G4int hcID 
      = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    a_hce->AddHitsCollection( hcID, m_hitsCollection ); 
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  G4bool ScintillatorSD::ProcessHits(G4Step* aStep, 
				     G4TouchableHistory*)
  {  
    // energy deposit
    G4double edep = aStep->GetTotalEnergyDeposit();

    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);
    if (debug) {
      G4cout << "ScintillatorSD::ProcessHits - energy="
	     << edep
	     << " trackID=" << aStep->GetTrack()->GetTrackID()
	     << " identifier=" << aStep->GetTrack()->GetVolume()->GetCopyNo()
	     << G4endl;
    }

    if (edep <= 0.) return false;

    auto start = aStep->GetPreStepPoint()->GetPosition();
    auto end   = aStep->GetPostStepPoint()->GetPosition();
    auto position = ( start + end ) / 2.;

    auto tstart = aStep->GetPreStepPoint()->GetGlobalTime();
    auto tend   = aStep->GetPostStepPoint()->GetGlobalTime();
    auto time = ( tstart + tend ) / 2.;

    ScintillatorHit* newHit = new ScintillatorHit();

    newHit->SetTrackID(aStep->GetTrack()->GetTrackID());
    newHit->SetPDGCode(aStep->GetTrack()->
		       GetDynamicParticle()->
		       GetParticleDefinition()->
		       GetPDGEncoding());
    newHit->SetEnergy(edep);
    newHit->SetTime(time);
    newHit->SetPosition(position);
    newHit->SetIdentifier(aStep->GetTrack()->GetVolume()->GetCopyNo());

    m_hitsCollection->insert( newHit );

    return true;
  }


  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void ScintillatorSD::EndOfEvent(G4HCofThisEvent*)
  {
    auto options = util::Options::GetInstance();
    G4bool verbose;
    options->GetOption("verbose",verbose);

    if ( verbose ) { 
      G4int nofHits = m_hitsCollection->entries();
      if ( nofHits == 0 )
	G4cout << G4endl
	       << "ScintillatorSD::EndOfEvent: in this event there was no"
	       << " energy recorded in the Scintillators" << G4endl;
      else {
	G4cout << G4endl
	       << "ScintillatorSD::EndOfEvent: in this event there are " << nofHits 
	       << " hits in the Scintillators: " << G4endl;
	for ( G4int i=0; i<nofHits; i++ ) (*m_hitsCollection)[i]->Print();
      } // display hits
    } // if verbose
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace gramsg4
