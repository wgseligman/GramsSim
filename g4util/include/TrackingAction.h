#ifndef g4util_TrackingAction_H
#define g4util_TrackingAction_H

#include "G4UserTrackingAction.hh"

// Forward declarations.
class G4Track;

namespace g4util {

  class UserAction;

  class TrackingAction : public G4UserTrackingAction {

  public:

    // If the constructor is called with an UserAction argument,
    // then we'll perform some record-keeping in UserTrackingAction.
    TrackingAction(UserAction* a = 0);

    virtual ~TrackingAction() {};
  
    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:

    // Save the UserAction object to be called by the UserTrackingAction.
    UserAction* m_action;
  };

} // namespace g4util

#endif
