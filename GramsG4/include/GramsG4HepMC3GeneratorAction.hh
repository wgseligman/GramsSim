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

#include <string> 

// Forward declarations
class G4Event;
namespace util {
  class Options;
}
namespace HepMC3 {
  class Reader;
  class GenEvent;
}

namespace gramsg4 {

  class HepMC3GeneratorAction : public G4VUserPrimaryGeneratorAction
  {
  public:
  
    HepMC3GeneratorAction(const std::string& inputFile);
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
    std::string m_inputFile;

    // The input event information, to be converted
    // and passed on to Geant4.
    HepMC3::GenEvent* m_hepmcEvent;

    // Pointer to instance of the Options class (see
    // GramsSim/util/README.md).
    util::Options* m_options;

    // Output flags set via the options XML file.
    bool m_verbose;
    bool m_debug;

    // Units of time, from the options XML file.
    double m_timeScale;

    // Control the logic of whether the run and event numbers normally
    // assigned by Geant4 are overridden by the contents of the HepMC3
    // file. In general, if the user has specified non-default values
    // for run and event numbers in the Geant4 options, they will take
    // precedence over the values in the HepMC3 file.
    bool m_useHepMC3RunNumber;
    bool m_useHepMC3EventNumber;

#ifdef HEPMC3_ROOTIO_INSTALLED
    // If we're reading in a ROOT file, there may be an options ntuple
    // in it too. If there is, copy it to our list of options to
    // maintain a historical record.
    bool m_CopyOptions(const std::string& inputFile);
#endif
  };

} // namespace gramsg4

#endif // _GramsG4HEPMC3GENERATORACTION_H_
