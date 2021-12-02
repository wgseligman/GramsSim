// This is a stub for a sensitive detector that will be
// fully implemented after the GDML code for assigning
// SDs to volumes is debugged. 

#ifndef GramsG4ScintillatorSD_h
#define GramsG4ScintillatorSD_h 1

#include "GramsG4ScintillatorHit.hh"

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"

namespace gramsg4 {

  class ScintillatorSD : public G4VSensitiveDetector 
  {
  public:
    ScintillatorSD(const G4String& name, const G4String& );

    virtual void   Initialize(G4HCofThisEvent*);
    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    virtual void   EndOfEvent(G4HCofThisEvent*);

  private:
    ScintillatorHitsCollection* m_hitsCollection;

  };

} // namespace gramsg4

#endif // GramsG4ScintillatorSD_h
