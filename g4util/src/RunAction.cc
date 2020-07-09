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

    // For support of multi-threaded applications, according to the G4
    // User's Guide the best practice is to create the analysis
    // manager in the RunAction constructor and delete it in the
    // destructor. The following creates the G4 analysis manager, and
    // writes a trivial message to avoid an "unused variable" warning.

    // If we're running a job that doesn't use the G4AnalysisManager,
    // no harm done.

    auto analysisManager = G4AnalysisManager::Instance();
    if (debug)
      std::cout << "g4util::RunAction - Analysis Manager using " 
		<< analysisManager->GetType() << std::endl;
  }

  RunAction::~RunAction() {
    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);

    // As noted in the constructor: Clean up the analysis manager.
    if (debug)
      std::cout << "g4util::~RunAction - final deletion of Analysis Manager"
		<< std::endl;

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
