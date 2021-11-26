#include "TrackingAction.h"
#include "UserAction.h"

#include "G4UserTrackingAction.hh"
#include "G4Step.hh"

namespace g4util {

  TrackingAction::TrackingAction( UserAction* a ) 
    : G4UserTrackingAction()
    , m_action(a)
  {}

  void TrackingAction::PreUserTrackingAction(const G4Track* a_track)
  {
    // Perform any record-keeping for this step.
    if (m_action != 0) m_action->PreTrackingAction(a_track);
  }

  void TrackingAction::PostUserTrackingAction(const G4Track* a_track)
  {
    // Perform any record-keeping for this step.
    if (m_action != 0) m_action->PostTrackingAction(a_track);
  }

} // namespace g4util
