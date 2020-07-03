/// \file GramsG4LArSensitiveDetector.hh
/// \brief The sensitive detector that stores LArHits.

/// Store the energy/photon deposits in the 
/// Active LAr TPC volume. 

#ifndef LArSensitiveDetector_h
#define LArSensitiveDetector_h 1

#include "GramsG4LArHit.hh"

#include "G4VSensitiveDetector.hh"

#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"

class LArSensitiveDetector : public G4VSensitiveDetector 
{
public:
  LArSensitiveDetector(const G4String& name,
		       const G4String& hitsCollectionName);

  virtual void   Initialize(G4HCofThisEvent*);
  virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  virtual void   EndOfEvent(G4HCofThisEvent*);

private:
  LArHitsCollection* m_hitsCollection;

};

#endif // LArSensitiveDetector_h
