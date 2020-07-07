/// \file persistency/gdml/GramsG4/src/GramsG4PrimaryGeneratorAction.cc
/// \brief Implementation of the GramsG4PrimaryGeneratorAction class
//
// 25-May-2020 WGS: Set some preliminary values for testing GRAMS.
//

#include "GramsG4PrimaryGeneratorAction.hh"
#include "GramsG4RunMode.hh"
#include "Options.h"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"

namespace gramsg4 {

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction()
    , fParticleGun(0)
  {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Set up the particle gun based on the run mode.
    switch ( gramsg4::RunMode::GetInstance()->GetRunMode() ) 
      {
      case batchMode:
	// Do nothing. The particle type and energy will
	// come from the macrofile.
	break;

      case uiMode:
	// Start with a "pretty" default particle, but let
	// the user override the choice with the uimacrofile
	// or interactive "/gun" commands.
	fParticleGun->SetParticleDefinition(particleTable->FindParticle("gamma"));
	fParticleGun->SetParticleEnergy(1.0*MeV);
	m_position = G4ThreeVector(5.0*cm, 5.0*cm, 200*cm);
	m_momentum = G4ThreeVector(0,0,-1);
	fParticleGun->SetParticlePosition(m_position);
	fParticleGun->SetParticleMomentumDirection(m_momentum);

      case commandMode:
	// Get the particle type and energy from the Options XML file.
	auto options = util::Options::GetInstance();

	G4int pdgCode;
	options->GetOption("pdgcode",pdgCode);
	// I'm skipping the error checking here; what if the
	// user puts in a non-existent code?
	fParticleGun->SetParticleDefinition(particleTable->FindParticle(pdgCode));

	G4double energy;
	options->GetOption("energy",energy);
	fParticleGun->SetParticleEnergy(energy*MeV);
      }
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  PrimaryGeneratorAction::~PrimaryGeneratorAction()
  {
    delete fParticleGun;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
  {
    auto runMode = gramsg4::RunMode::GetInstance()->GetRunMode();

    switch ( runMode ) 
      {
      case batchMode:
	// Do nothing. The particle position and momentum
	// come from the macrofile.
	break;

      case uiMode:
	// Again, do nothing. Stick with the defaults in the
	// constructor, and allow the user to override in 
	// .mac file or via the UI.
	break;

      case commandMode:

	// For initial studies, shoot photons in a distribution 
	// along the z-axis. At some point we may want to put
	// lots more parameters in the XML file to control
	// position and momentum.

	m_position = G4ThreeVector((G4UniformRand()-0.5) * 15.0*cm,
				   (G4UniformRand()-0.5) * 15.0*cm,
				   200*cm);
	m_momentum = G4ThreeVector(0,0,-1);
	fParticleGun->SetParticlePosition(m_position);
	fParticleGun->SetParticleMomentumDirection(m_momentum);
      }      

    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);

    // This can be a mis-leading debug mode, since there isn't any
    // easy way to get at the ParticleGun parameters if the user
    // sets them via the command file or UI. 
    if (debug  &&  ( runMode == uiMode  ||  runMode == commandMode )) {
      std::cout << "GramsG4PrimaryGeneratorAction::GeneratePrimaries: "
		<< "position=(" 
		<< m_position.x() << "," 
		<< m_position.y() << ","
		<< m_position.z() << ") "
		<< "momentum=(" 
		<< m_momentum.x() 
		<< "," << m_momentum.y() 
		<< "," << m_momentum.z() << ") "
		<< std::endl;
    }

    fParticleGun->GeneratePrimaryVertex(anEvent);
  }

} // namespace gramsg4
