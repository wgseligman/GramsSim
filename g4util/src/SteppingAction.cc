#include "SteppingAction.h"
#include "UserAction.h"

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

namespace g4util {

  SteppingAction::SteppingAction( UserAction* a ) 
    : G4UserSteppingAction()
    , m_action(a)
  {}

  void SteppingAction::UserSteppingAction(const G4Step* a_step)
  {
    // Perform any record-keeping for this step.
    if (m_action != 0) m_action->SteppingAction(a_step);
  }

} // namespace g4util
