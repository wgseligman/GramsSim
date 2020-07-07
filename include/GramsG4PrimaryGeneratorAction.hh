/// \file GramsG4/include/GramsG4PrimaryGeneratorAction.hh
/// \brief Definition of the GramsG4PrimaryGeneratorAction class
///
///  This generator can run in these modes:
///  - Batch mode: The user has supplied commands in a G4 macro file.
///    Assume all generator parameters are set by that file.
///  - UI mode: Similar to batch mode, but set some default parameters
///    so the user can see pretty pictures without thinking much about it.
///  - Command-line mode: Take the generator parameters from the command
///    line. 
///

#ifndef _GramsG4PRIMARYGENERATORACTION_H_
#define _GramsG4PRIMARYGENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ThreeVector.hh"
#include "globals.hh"

class G4Event;
class G4ParticleGun;

/// Primary generator action; uses a "particle gun" to insert
/// particles into the detector geometry.

class GramsG4PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  
  GramsG4PrimaryGeneratorAction();
  ~GramsG4PrimaryGeneratorAction();
  
  virtual void GeneratePrimaries(G4Event* anEvent);
  
private:
  
  G4ParticleGun* fParticleGun;
  G4ThreeVector m_position;
  G4ThreeVector m_momentum;
};

#endif
