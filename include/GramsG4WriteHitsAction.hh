/// \file GramsG4WriteHitsAction.hh
/// \brief User-action class to write out hits.

/// This is a user-action class that writes out the
/// event hits. At present, the output takes the form
/// of basic (and inefficient) ROOT n-tuple.

#ifndef GramsG4WriteHitsAction_H
#define GramsG4WriteHitsAction_H

#include "GramsG4LArHit.hh"
#include "UserAction.h" // in g4util/

class G4Run;
class G4Event;

namespace gramsg4 {

  class WriteHitsAction : public g4util::UserAction 
  {
  public:

    WriteHitsAction();
    virtual ~WriteHitsAction();

    // The user-action classes we'll need to define
    // the output file and write hits.

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

  private:

    // Get the collection of hits from the current event.
    LArHitsCollection* GetHitsCollection(G4int hcID,
					 const G4Event* event) const;

    // Hit collection ID number, assigned by Geant4.
    G4int m_LArHitCollectionID;

    // ID numbers for the n-tuples we'll create. Ntuples are
    // automatically assigned IDs in the order we create them.
    G4int m_LArNTID;
    G4int m_TrackNTID;
    G4int m_ScintNTID;
  };

} // namespace gramsg4

#endif // GramsG4WriteHitsAction_H
