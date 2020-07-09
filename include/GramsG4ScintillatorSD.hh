// This is a stub for a sensitive detector that will be
// fully implemented after the GDML code for assigning
// SDs to volumes is debugged. 

#ifndef GramsG4ScintillatorSD_h
#define GramsG4ScintillatorSD_h 1

#include "G4VSensitiveDetector.hh"

#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"

namespace gramsg4 {

  class ScintillatorSD : public G4VSensitiveDetector 
  {
  public:
    ScintillatorSD(const G4String& name)
      : G4VSensitiveDetector(name)
    {}

    virtual void   Initialize(G4HCofThisEvent*) {}
    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) {return true;}
    virtual void   EndOfEvent(G4HCofThisEvent*) {}

  };

} // namespace gramsg4

#endif // GramsG4ScintillatorSD_h
