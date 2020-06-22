#include "SteppingAction.h"
#include "UserAction.h"

#include "G4Step.hh"

namespace g4util {

  void SteppingAction::UserSteppingAction(const G4Step* a_step)
  {
    // Perform any record-keeping for this step.
    if (m_action != 0) m_action->SteppingAction(a_step);
  }

} // namespace g4util
