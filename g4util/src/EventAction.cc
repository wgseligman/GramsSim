#include "EventAction.h"
#include "UserAction.h"

#include "G4UserEventAction.hh"
#include "G4Event.hh"

namespace g4util {

  EventAction::EventAction(UserAction* a)
    : G4UserEventAction()
    , m_action(a)
  {}

  EventAction::~EventAction() {;}

  void EventAction::BeginOfEventAction(const G4Event* a_evt)
  {
    // Initiate record-keeping for this event.
    if (m_action != 0) m_action->BeginOfEventAction(a_evt);
  }

  void EventAction::EndOfEventAction(const G4Event* a_evt)
  {
    // Terminate record-keeping for this event.
    if (m_action != 0) m_action->EndOfEventAction(a_evt);
  }

} // namespace g4util
