/// \file GramsG4ActionInitialization.cc
/// \brief Implementation of the GramsG4ActionInitialization class

#include "GramsG4ActionInitialization.hh"
#include "GramsG4PrimaryGeneratorAction.hh"
#include "UserAction.h" // in g4util/
#include "RunAction.h" // in g4util/
#include "EventAction.h" // in g4util/
#include "SteppingAction.h" // in g4util/


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GramsG4ActionInitialization::GramsG4ActionInitialization()
 : G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GramsG4ActionInitialization::~GramsG4ActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GramsG4ActionInitialization::BuildForMaster() const
{
  // This routine is only called for a multi-threaded application.
  SetUserAction(new g4util::RunAction( m_userAction ));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GramsG4ActionInitialization::Build() const
{
  // This routine is either called only once, for a single-thread
  // application, or at the start of each new thread in a
  // multi-threaded application. Here's where you have to start
  // thinking about thread-safety.

  // Define the event generator.
  SetUserAction(new GramsG4PrimaryGeneratorAction);

  // Define the links between Geant4's user-action classes and the
  // UserAction's classes.
  SetUserAction(new g4util::RunAction(m_userAction));
  SetUserAction(new g4util::EventAction(m_userAction));
  SetUserAction(new g4util::SteppingAction(m_userAction));
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
