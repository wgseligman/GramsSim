/// \file GramsG4NumbersAction.hh
/// \brief User-action class to set user-assigned run and starting event numbers..

/// This is a user-action class that accepts the starting run and
/// event numbers from the options XML files and assigns them to the
/// appropriate managers.

#ifndef GramsG4NumbersAction_H
#define GramsG4NumbersAction_H

#include "UserAction.h" // in g4util/

// Forward declarations
namespace util {
  class Options;
}
class G4Run;
class G4Event;

namespace gramsg4 {

  class NumbersAction : public g4util::UserAction 
  {
  public:

    NumbersAction();
    virtual ~NumbersAction();

    // The user-action classes we'll need to set up the Geant4
    // managers.
    virtual void BeginOfRunAction(const G4Run*);
    virtual void BeginOfEventAction(const G4Event*);

  private:

    // To access options XML flags.
    util::Options* m_options;
    G4bool m_verbose;
    G4bool m_debug;

    // The starting event number selected by the user.
    G4int m_startingEventNumber;
  };

} // namespace gramsg4

#endif // GramsG4NumbersAction_H
