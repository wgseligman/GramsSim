/// \file GramsG4ActionInitialization.cc
/// \brief Implementation of the GramsG4ActionInitialization class

#include "GramsG4ActionInitialization.hh"
#include "GramsG4PrimaryGeneratorAction.hh"

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
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GramsG4ActionInitialization::Build() const
{
  SetUserAction(new GramsG4PrimaryGeneratorAction);
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
