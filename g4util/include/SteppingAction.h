#ifndef g4util_SteppingAction_H
#define g4util_SteppingAction_H

#include "G4UserSteppingAction.hh"

// Forward declarations.
class G4Step;

namespace g4util {

  class UserAction;

  class SteppingAction : public G4UserSteppingAction {

  public:

    // If the constructor is called with an UserAction argument,
    // then we'll perform some record-keeping in UserSteppingAction.
    SteppingAction(UserAction* a = 0) {m_action = a;}

    virtual ~SteppingAction() {};
  
    virtual void UserSteppingAction(const G4Step*);

  private:

    // Save the UserAction object to be called by the UserSteppingAction.
    UserAction* m_action;
  };

} // namespace g4util

#endif
