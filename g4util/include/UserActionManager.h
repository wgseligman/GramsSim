/// UserActionManager.hh
/// 13-Mar-2002 Bill Seligman

/// In my experience, people barely understand what the UserAction
/// interface does.  So why do we need a UserActionManager class?

/// Suppose I've written a class that inherits from UserAction that
/// makes histograms.  You've written a class that inherits from
/// UserAction to write events to a disk file.  Jane has written an
/// UserAction that makes ntuples.  A big massive 500-CPU-hour run of
/// G4 is planned, and we're all planning how to put our user-action
/// classes together.

/// By using a UserActionManager class, each one of us can develop our
/// own user-action classes independently.  Then, when we have the big
/// run, the user-action classes can be put successively called by the
/// UserActionManager without changing any of the classes.

/// Another feature is the ability to separate different user-action
/// functions.  For example, you don't have to mix your code that
/// writes hits with the code that makes histograms; the code can be
/// put into separate classes.

#ifndef g4util_UserActionManager_H
#define g4util_UserActionManager_H

#include "UserAction.h"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include <vector>

namespace g4util {

  class UserActionManager : public UserAction 
  {
  public:
  
    UserActionManager();
    virtual ~UserActionManager();
  
    G4int GetSize() { return m_userActions.size(); }
    UserAction* GetAction(G4int i) { return m_userActions[i]; }
    static void AddAndAdoptAction(UserAction* a) { m_userActions.push_back(a); }

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    virtual void TrackingAction(const G4Track*);
    virtual void SteppingAction(const G4Step*);

  private:
    static std::vector<UserAction*> m_userActions;
  
  };

} // namespace g4util

#endif // g4util_UserActionManager_H
