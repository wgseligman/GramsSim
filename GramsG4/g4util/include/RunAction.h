#ifndef g4util_RunAction_H
#define g4util_RunAction_H

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

namespace g4util {

  class UserAction;

  class RunAction : public G4UserRunAction
  {
  public:
    // If the constructor is called with an UserAction object,
    // then there will be record-keeping performed in the
    // BeginOfRunAction and EndOfRunAction.
    RunAction(UserAction* a = 0);
    virtual ~RunAction();

  public:
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

  private:
    // Save the UserAction object to be called at the beginning
    // and end of the run.
    UserAction* m_action;
  };

} // namespace g4util

#endif
