/// \file GramsG4WriteNtuplesAction.hh
/// \brief User-action class to write out hits.

/// This is a user-action class that writes out the event hits and
/// truth information. At present, the output takes the form of a ROOT
/// Tree with several branches, with each branch containing a data
/// object defined in GramsDataObj.

#ifndef GramsG4WriteNtuplesAction_H
#define GramsG4WriteNtuplesAction_H

// From GramsDataObj: The TTree branches we'll write.
#include "EventID.h"
#include "MCTrackList.h"
#include "MCLArHits.h"
#include "MCScintHits.h"

// For Geant4
#include "GramsG4LArHit.hh"
#include "UserAction.h" // in g4util/

#include <vector>

// Forward declarations.
namespace util {
  class Options;
}
class G4Run;
class G4Event;
class TFile;
class TTree;

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

    // Save the output states.
    G4bool m_verbose;
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

    // Pointer to instance of the Options class (see
    // GramsSim/util/README.md).
    util::Options* m_options;

    // Units, from the options XML file.
    double m_timeScale;
    double m_lengthScale;
    double m_energyScale;

    // The name of the ROOT output file.
    G4String m_filename;

    // The name of the ROOT tree (or n-tuple) that will contain the
    // branches for our output objects.
    G4String m_treeName;

    // The objects (defined in GramsDataObj) that will be written to
    // the above n-tuple.
    grams::EventID*     m_eventID;
    grams::MCTrack      m_mcTrack; // Does not have to be a pointer, since it's not written to a branch directly.
    grams::MCTrackList* m_mcTrackList;
    grams::MCLArHits*   m_mcLArHits;
    grams::MCScintHits* m_mcScintHits;
  };

} // namespace gramsg4

#endif // GramsG4WriteNtuplesAction_H
