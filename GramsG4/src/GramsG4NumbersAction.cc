/// \file GramsG4NumbersAction.cc
/// \brief User-action class to set user-assigned run and starting event numbers..

/// This is a user-action class that accepts the starting run and
/// event numbers from the options XML files and assigns them to the
/// appropriate Geant4 managers.

#include "GramsG4NumbersAction.hh"

#include "Options.h" // in util/
#include "UserAction.h" // in g4util/

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"

namespace gramsg4 {

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Default constructor. 
  NumbersAction::NumbersAction()
    : UserAction()
    , m_verbose(false)
    , m_debug(false)
    , m_startingEventNumber(-1)
  {
    // Set up access to the options from the XML file.
    m_options = util::Options::GetInstance();
    m_options->GetOption("verbose",m_verbose);
    m_options->GetOption("debug",m_debug);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Destructor.
  NumbersAction::~NumbersAction() {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // If the user has specified an alternative run number,
  // use it instead of the standard Geant4 default.
  void NumbersAction::BeginOfRunAction(const G4Run*) {
    // The manager we'll need.
    auto runManager = G4RunManager::GetRunManager();
    
    // Is the user overriding the default G4 run number?
    int runNumber;
    bool result = m_options->GetOption("run",runNumber);
    if ( result  &&  runNumber > 0 ) {
      // We have to set the run number in two different places,
      // primarily because this Geant4 application uses
      // multi-threading; the interaction between multiple threads
      // and setting the run number via G4MTRunManager is not well
      // defined. 
      runManager->SetRunIDCounter( runNumber );
      // To change the run number in the current G4Run we need the
      // non-const version.
      runManager->GetNonConstCurrentRun()->SetRunID( runNumber );
      if (m_verbose) 
	G4cout << "GramsG4NumbersAction::BeginOfRunAction(): Set initial run number to "
	       << runNumber << G4endl;
    }
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // If the user has specified an alternative starting event number,
  // use it instead of the standard Geant4 default.
  void NumbersAction::BeginOfEventAction(const G4Event* a_event) 
  {
    // Did the user specify a starting event number?
    if ( m_startingEventNumber < 0 ) {
      bool result = m_options->GetOption("startEvent",m_startingEventNumber);
      if ( !result  ||  m_startingEventNumber < 0 ) {
	m_startingEventNumber = 0;
      }
    } // if invalid starting event number (e.g., uninitialized)
    
    if ( m_startingEventNumber > 0 ) {
      // To change the event ID number, we need access to the non-const
      // version of the current G4Event.
      auto eventManager = G4EventManager::GetEventManager();
      auto event = eventManager->GetNonconstCurrentEvent();
      
      auto currentEventID = a_event->GetEventID();
      G4int newEventID = currentEventID + m_startingEventNumber;
      event->SetEventID( newEventID );
      if (m_verbose) 
	G4cout << "GramsG4NumbersAction::BeginOfEventAction(): Set event number to "
	       << newEventID << G4endl;
    } // if starting event number > 0
  }

} // namespace gramsg4
