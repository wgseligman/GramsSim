/// \file GramsG4ActionInitialization.cc
/// \brief Implementation of the GramsG4ActionInitialization class

#include "GramsG4ActionInitialization.hh"
#include "GramsG4GPSGeneratorAction.hh"
#include "GramsG4HepMC3GeneratorAction.hh"
#include "Options.h" // in util
#include "UserAction.h" // in g4util/
#include "RunAction.h" // in g4util/
#include "EventAction.h" // in g4util/
#include "TrackingAction.h" // in g4util/
#include "SteppingAction.h" // in g4util/

namespace gramsg4 {

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  ActionInitialization::ActionInitialization()
    : G4VUserActionInitialization()
  {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  ActionInitialization::~ActionInitialization()
  {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void ActionInitialization::BuildForMaster() const
  {
    // This routine is only called for a multi-threaded
    // application.
    SetUserAction(new g4util::RunAction( m_userAction ));
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void ActionInitialization::Build() const
  {
    // This routine is either called only once, for a single-thread
    // application, or at the start of each new thread in a
    // multi-threaded application. Here's where you have to start
    // thinking about thread-safety.

    // Define the event generator. Check if the user has specified an
    // input file of previously-generated primary events.
    auto options = util::Options::GetInstance();
    G4String inputFile;
    auto success = options->GetOption("inputgen",inputFile);
    if ( !success || inputFile.empty() )
      // There is no input file of generated events, so let the GPS 
      // commands in the macro file control event generation.
      SetUserAction(new gramsg4::GPSGeneratorAction);
    else
      // Read the input file of generated events with HepMC3.
      SetUserAction(new gramsg4::HepMC3GeneratorAction(inputFile));
  
    // Define the links between Geant4's user-action classes and the
    // UserAction's classes.
    SetUserAction(new g4util::RunAction(m_userAction));
    SetUserAction(new g4util::EventAction(m_userAction));
    SetUserAction(new g4util::TrackingAction(m_userAction));
    SetUserAction(new g4util::SteppingAction(m_userAction));
  }  

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace gramsg4
