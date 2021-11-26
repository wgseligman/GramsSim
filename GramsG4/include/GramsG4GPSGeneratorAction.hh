/// \file GramsG4/include/GramsG4GPSGeneratorAction.hh
/// \brief Definition of the GramsG4GPSGeneratorAction class
///
/// Use G4GeneralPrimarySource to generate events. This means that all
/// the control of event generation comes from the Geant4 macro file
/// executed by the main routine.

#ifndef _GramsG4GPSGENERATORACTION_H_
#define _GramsG4GPSGENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"

// Forward declarations
class G4Event;
class G4GeneralParticleSource;

/// Primary generator action; uses a "particle gun" to insert
/// particles into the detector geometry.

namespace gramsg4 {

  class GPSGeneratorAction : public G4VUserPrimaryGeneratorAction
  {
  public:
  
    GPSGeneratorAction();
    ~GPSGeneratorAction();
  
    virtual void GeneratePrimaries(G4Event* anEvent);
  
  private:

    G4GeneralParticleSource* m_ParticleSource;
  };

} // namespace gramsg4

#endif // _GramsG4GPSGENERATORACTION_H_
