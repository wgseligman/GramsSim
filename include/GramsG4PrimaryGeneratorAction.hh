/// \file GramsG4/include/GramsG4PrimaryGeneratorAction.hh
/// \brief Definition of the GramsG4PrimaryGeneratorAction class
///
/// Use G4GeneralPrimarySource to generate events. This means that all
/// the control of event generation comes from the Geant4 macro file
/// executed by the main routine.

#ifndef _GramsG4PRIMARYGENERATORACTION_H_
#define _GramsG4PRIMARYGENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"

class G4Event;
class G4GeneralParticleSource;

/// Primary generator action; uses a "particle gun" to insert
/// particles into the detector geometry.

namespace gramsg4 {

  class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
  {
  public:
  
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction();
  
    virtual void GeneratePrimaries(G4Event* anEvent);
  
  private:
  
    G4GeneralParticleSource* fParticleSource;
  };

} // namespace gramsg4

#endif
