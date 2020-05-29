/// \file persistency/gdml/GramsG4/include/GramsG4PrimaryGeneratorAction.hh
/// \brief Definition of the GramsG4PrimaryGeneratorAction class
//
//
//
//

#ifndef _GramsG4PRIMARYGENERATORACTION_H_
#define _GramsG4PRIMARYGENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"

#include "globals.hh"

class G4Event;
class G4ParticleGun;

/// Minimal primary generator action to demonstrate the use of GDML geometries

class GramsG4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  
  GramsG4PrimaryGeneratorAction();
  ~GramsG4PrimaryGeneratorAction();
  
  virtual void GeneratePrimaries(G4Event* anEvent);
  
private:
  
  G4int nCalls;
  G4ParticleGun* fParticleGun;
};

#endif
