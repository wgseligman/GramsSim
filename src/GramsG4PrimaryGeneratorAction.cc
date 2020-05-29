/// \file persistency/gdml/GramsG4/src/GramsG4PrimaryGeneratorAction.cc
/// \brief Implementation of the GramsG4PrimaryGeneratorAction class
//
// 25-May-2020 WGS: Set some preliminary values for testing GRAMS.
//

#include "GramsG4PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "GramsG4Options.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GramsG4PrimaryGeneratorAction::GramsG4PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction()
 , nCalls(0)
 , fParticleGun(0)
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  fParticleGun->SetParticleDefinition(
               particleTable->FindParticle(particleName="gamma"));
  fParticleGun->SetParticleEnergy(2.0*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GramsG4PrimaryGeneratorAction::~GramsG4PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GramsG4PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // For some reason, we're not incrementing the event ID
  // when we're using the UI. So base the switch on the number
  // of calls to this routine.
  // G4int i = anEvent->GetEventID() % 3;
  G4int i = nCalls++ % 3;
  G4ThreeVector position, momentum;
  switch(i)
  {
    case 0:
      position = G4ThreeVector(0.5*cm,0.5*cm,200*cm);
      momentum = G4ThreeVector(0,0,-1);
      break;
    case 1:
      position = G4ThreeVector(0.5*cm,-180*cm,-10*cm);
      momentum = G4ThreeVector(0,1,0);
      break;
    case 2:
      position = G4ThreeVector(-115*cm,0.5*cm,-10*cm);
      momentum = G4ThreeVector(1,0,0);
      break;
  }

  auto options = GramsG4Options::GetInstance();
  G4bool debug;
  options->GetOption("debug",debug);

  if (debug) {
    std::cout << "GramsG4PrimaryGeneratorAction::GeneratePrimaries: "
	      << "position=(" << position.x() << "," << position.y() << ","<< position.z() << ") "
	      << "momentum=(" << momentum.x() << "," << momentum.y() << ","<< momentum.z() << ") "
	      << std::endl;
  }

  fParticleGun->SetParticlePosition(position);
  fParticleGun->SetParticleMomentumDirection(momentum);
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
