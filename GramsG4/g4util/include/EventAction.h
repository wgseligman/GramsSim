#ifndef g4util_EventAction_H
#define g4util_EventAction_H

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;

namespace g4util {

  class UserAction;

  class EventAction : public G4UserEventAction
  {
  public:
    // If the constructor is called with a UserAction object,
    // then there will be record-keeping performed in the
    // BeginOfEventAction and EndOfEventAction.
    EventAction(UserAction* a = 0);

    virtual ~EventAction();

  public:
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    
  private:
    // Save the UserAction object to be called at the beginning
    // and end of the event.
    UserAction* m_action;
  };

} // namespace g4util

#endif
