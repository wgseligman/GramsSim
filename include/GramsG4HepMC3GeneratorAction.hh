/// \file GramsG4/include/GramsG4HepMC3GeneratorAction.hh
/// \brief Definition of the GramsG4HepMC3GeneratorAction class
///
/// The user specified an input file. Read it using
/// HepMC3 and ignore any particle-generation commands in the
/// Geant4 macro file.
///
#ifndef _GramsG4HEPMC3GENERATORACTION_H_
#define _GramsG4HEPMC3GENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4String.hh"

// Forward declarations
class G4Event;
namespace HepMC3 {
  class Reader;
  class GenEvent;
}

namespace gramsg4 {

  class HepMC3GeneratorAction : public G4VUserPrimaryGeneratorAction
  {
  public:
  
    HepMC3GeneratorAction(const G4String& inputFile);
    ~HepMC3GeneratorAction();
  
    virtual void GeneratePrimaries(G4Event* anEvent);
  
  protected:

    // Local utility routines.

    // Open, close, and read the input file.
    void OpenFile();
    void CloseFile();
    void ReadEvent();

    // Convert HepMC3 event to Geant4.
    void HepMC2G4( const HepMC3::GenEvent*, G4Event* );

  private:

    // The input file of generated events.
    G4String m_inputFile;

    // The HepMC3 module to read events. The choice of module
    // is based on the input file name extension (the part 
    // after the '.').
    HepMC3::Reader*   m_reader; 

    // The input event information, to be converted
    // and passed on to Geant4.
    HepMC3::GenEvent* m_hepmcEvent;
  };

} // namespace gramsg4

#endif // _GramsG4HEPMC3GENERATORACTION_H_
