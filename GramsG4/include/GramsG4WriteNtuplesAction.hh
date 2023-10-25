/// \file GramsG4WriteNtuplesAction.hh
/// \brief User-action class to write out hits.

/// This is a user-action class that writes out the event hits and
/// truth information. At present, the output takes the form of basic
/// (and inefficient) ROOT n-tuples.

#ifndef GramsG4WriteNtuplesAction_H
#define GramsG4WriteNtuplesAction_H

#include "GramsG4LArHit.hh"
#include "UserAction.h" // in g4util/

#include <vector>

// Forward declarations
namespace util {
  class Options;
}
class G4Run;
class G4Event;

namespace gramsg4 {

  class WriteNtuplesAction : public g4util::UserAction 
  {
  public:

    WriteNtuplesAction();
    virtual ~WriteNtuplesAction();

    // The user-action classes we'll need to define
    // the output file and write hits.

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    virtual void PreTrackingAction (const G4Track*);
    virtual void PostTrackingAction(const G4Track*);
    virtual void SteppingAction(const G4Step*);

  private:

    // Get the collection of hits from the current event. Use
    // templates to avoid having to write this function for each type
    // of hit collection that we might want to fetch from a G4Event.
    template <class HC>
    HC* GetHitsCollection(G4int hcID,
			  const G4Event* event) const;

    // Hit collection ID numbers, assigned by Geant4.
    G4int m_LArHitCollectionID;
    G4int m_ScintillatorHitCollectionID;

    // ID numbers for the n-tuples we'll create. Ntuples are
    // automatically assigned IDs in the order we create them.
    G4int m_LArNTID;
    G4int m_ScintNTID;
    G4int m_TrackNTID;
    G4int m_optionsNTID;

    // Save the debugging state.
    G4bool m_debug;

    // Trajectory fields and methods. Eventually all of the following
    // will probably be some sort of separate Trajectory
    // class. However, as long as we're still using the Geant4
    // Analysis Manager to write our ntuples, let's at least
    // "encapsulate" the trajectory information as best we can.

    // Remove all trajectory information.
    void ClearTrajectory();

    // Add the current track's trajectory information; e.g.,
    // (t,x,y,z), (E,px,py,pz).
    void AddTrajectoryPoint( const G4Track* );

    // Create trajectory information for each track. In this case, a
    // trajectory will be set of (t,x,y,z) and (E,px,py,pz)
    // points. The Geant4 analysis manager won't let us store
    // 4-vectors in an ntuple, much less a vector of four-vectors,
    // so we have to maintain a separate std::vector for each
    // component of the 4-vectors.

    // 15-Mar-2021 WGS: Include the Identifier number for the volumes
    // in the trajectory; see the GDML file's comments for more.

    // 18-Aug-2021 WGS: Save some disk space by only using single
    // precision to save some values.
    std::vector<G4double> m_time;
    std::vector<G4float> m_xpos;
    std::vector<G4float> m_ypos;
    std::vector<G4float> m_zpos;
    std::vector<G4double> m_energy;
    std::vector<G4float> m_xmom;
    std::vector<G4float> m_ymom;
    std::vector<G4float> m_zmom;
    std::vector<int> m_identifier;


    // Pointer to instance of the Options class (see
    // GramsSim/util/README.md).
    util::Options* m_options;

    // Units, from the options XML file.
    double m_timeScale;
    double m_lengthScale;
    double m_energyScale;

    // The name of the ROOT output file.
    G4String m_filename;
  };

} // namespace gramsg4

#endif // GramsG4WriteNtuplesAction_H
