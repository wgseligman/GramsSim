// In util/
#include "Options.h"

// In g4util/
#include "RunAction.h"
#include "UserAction.h"
#include "Analysis.h"

#include "G4UserRunAction.hh"
#include "G4Run.hh"

namespace g4util {

  RunAction::RunAction(UserAction* a)
    : G4UserRunAction()
    , m_action(a)
  {
    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);
  }

  RunAction::~RunAction() {
    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);
  }

  void RunAction::BeginOfRunAction(const G4Run* a_run)
  {
    // Initiate record-keeping for this run.
    if (m_action != 0) m_action->BeginOfRunAction(a_run);
  }


  void RunAction::EndOfRunAction(const G4Run* a_run)
  {
    // Terminate record-keeping for this run.
    if (m_action != 0) m_action->EndOfRunAction(a_run);
  }

} // namespace g4util
