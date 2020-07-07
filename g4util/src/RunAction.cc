
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
    // For support of multi-threaded applications, according to the G4
    // User's Guide the best practice is to create the analysis
    // manager in the RunAction constructor and delete it in the
    // destructor. The following creates the G4 analysis manager, and
    // writes a trivial message to avoid an "unused variable" warning.

    // If we're running a job that doesn't use the G4AnalysisManager,
    // no harm done (except that we might have load problems if we
    // select ROOT but don't include the ROOT libraries in the
    // CMakeLists.txt).

    auto analysisManager = G4AnalysisManager::Instance();
    std::cout << "g4util::RunAction - Analysis Manger using " 
	      << analysisManager->GetType() << std::endl;
  }

  RunAction::~RunAction() {
    // As noted in the constructor: Clean up the analysis manager.
    delete G4AnalysisManager::Instance();  
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
