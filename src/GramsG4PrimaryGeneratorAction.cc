/// \file persistency/gdml/GramsG4/src/GramsG4PrimaryGeneratorAction.cc
/// \brief Implementation of the GramsG4PrimaryGeneratorAction class
//
// 25-May-2020 WGS: Set some preliminary values for testing GRAMS.
//

#include "GramsG4PrimaryGeneratorAction.hh"
#include "Options.h"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"


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
  // For initial studies, shoot photons in a distribution 
  // along the z-axis.

  G4ThreeVector position, momentum;
  position = G4ThreeVector((G4UniformRand()-0.5) * 15.0*cm,
			   (G4UniformRand()-0.5) * 15.0*cm,
			   200*cm);
  momentum = G4ThreeVector(0,0,-1);

  auto options = util::Options::GetInstance();
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
