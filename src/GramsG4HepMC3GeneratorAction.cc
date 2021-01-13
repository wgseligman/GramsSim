/// \file persistency/gdml/GramsG4/src/GramsG4HepMC3GeneratorAction.cc
/// \brief Implementation of the GramsG4HepMC3GeneratorAction class
//
// Portions of this code were copied from Beam Delivery Simulation
// (BDSIM), University of London
// 
// ... those portions, in turn, came from Geant4 example HepMCEx01. 

#include "GramsG4HepMC3GeneratorAction.hh"

#include "G4Event.hh"
#include "G4Exception.hh"
#include "G4String.hh"

#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Reader.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/ReaderHEPEVT.h"
#include "HepMC3/ReaderLHEF.h"
#include "HepMC3/ReaderRoot.h"
#include "HepMC3/ReaderRootTree.h"
 

namespace gramsg4 {

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  HepMC3GeneratorAction::HepMC3GeneratorAction(const G4String& a_inputGen)
    : G4VUserPrimaryGeneratorAction()
    , m_inputFile(a_inputGen)
    , m_reader(nullptr)
    , m_hepmcEvent(nullptr)
  {
    OpenFile();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  HepMC3GeneratorAction::~HepMC3GeneratorAction()
  {
    CloseFile();
    delete m_hepmcEvent;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void HepMC3GeneratorAction::GeneratePrimaries(G4Event* anEvent)
  {
    ReadEvent();
    HepMC2G4(m_hepmcEvent, anEvent);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  //
  // Local utility routines.

  void HepMC3GeneratorAction::OpenFile() 
  {
    // Parse the input file name, looking for its extension.
    auto search = m_inputFile.find('.');
    if ( search == std::string::npos ) {
      // There is no file name extension (the part after the '.').
      G4ExceptionDescription description;
      description << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
                  << "could not find an extension or filetype at the end of '"
		  << m_inputFile << "'";
      G4Exception("gramsg4::HepMC3GeneratorAction::OpenFile","invalid file name",
                  FatalException, description);
    }
    // Get the characters after the '.'.
    G4String extension;
    if ( search + 2 < m_inputFile.size() )
      extension = m_inputFile.substr( search+1 );
    else {
      G4ExceptionDescription description;
      description << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
                  << "could not find anything after the '.' in '"
		  << m_inputFile << "'";
      G4Exception("gramsg4::HepMC3GeneratorAction::OpenFile","invalid file name",
                  FatalException, description);
    }
    
    extension.toLower();
    if ( extension == "hepmc2" ) 
      m_reader = new HepMC3::ReaderAsciiHepMC2(m_inputFile);
    else if ( extension == "hepmc3" )
      m_reader = new HepMC3::ReaderAscii(m_inputFile);
    else if ( extension == "hpe" )
      m_reader = new HepMC3::ReaderHEPEVT(m_inputFile);
    else if ( extension == "root" )
      m_reader = new HepMC3::ReaderRoot(m_inputFile);
    else if ( extension == "treeroot" )
      m_reader = new HepMC3::ReaderRootTree(m_inputFile);
    else if ( extension == "lhef" )
      m_reader = new HepMC3::ReaderLHEF(m_inputFile);
    else {
      G4ExceptionDescription description;
      description << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
                  << "did not recognize '" << extension 
		  << "' as a valid file type in '"
		  << m_inputFile << "'";
      G4Exception("gramsg4::HepMC3GeneratorAction::OpenFile","invalid file extension",
                  FatalException, description);
    }
  }

  void HepMC3GeneratorAction::CloseFile()
  {
    if ( m_reader )
      m_reader->close();
    delete m_reader;
  }

  void HepMC3GeneratorAction::ReadEvent()
  {
    if ( !m_reader ) {
      G4ExceptionDescription description;
      description << "File " << __FILE__ << " Line " << __LINE__ << " " << G4endl
		  << "HepMC3 reader not initialiazed for file '"
		  << m_inputFile << "'";
      G4Exception("gramsg4::HepMC3GeneratorAction::ReadEvent","read error",
		  FatalException, description);
    }

    delete m_hepmcEvent;
    m_hepmcEvent = new HepMC3::GenEvent();
    
    bool readEventOK = m_reader->read_event(*m_hepmcEvent);
    if ( !readEventOK ) {
      G4ExceptionDescription description;
      description << "File " << __FILE__ << " Line " << __LINE__ << " " << G4endl
		  << "could not read file '"
		  << m_inputFile << "'";
      G4Exception("gramsg4::HepMC3GeneratorAction::ReadEvent","read error",
		  FatalException, description);
    }
    
    // Have we reached the end of the file?
    if ( m_reader->failed() )
      {
	G4cout << "gramsg4::HepMC3GeneratorAction::ReadEvent - " << G4endl 
	       << " End of input generated events file '" << m_inputFile 
	       << "' reached." << G4endl
	       << " Return to beginning of file for next event." << G4endl;

	CloseFile();
	OpenFile();
	delete m_hepmcEvent;
	m_hepmcEvent = new HepMC3::GenEvent();

	readEventOK = m_reader->read_event(*m_hepmcEvent);
	if ( !readEventOK ) {
	  G4ExceptionDescription description;
	  description << "File " << __FILE__ << " Line " << __LINE__ << " " << G4endl
		      << "could not re-read file '"
		      << m_inputFile << "'";
	  G4Exception("gramsg4::HepMC3GeneratorAction::ReadSingleEvent","read error",
		      FatalException, description);
	}
      } // end of file
  }

  void HepMC3GeneratorAction::HepMC2G4( const HepMC3::GenEvent* a_hepmc, G4Event* a_event )
  {
    // Note that Geant4 and CLHEP use MeV/mm/ns. Make sure that
    // whatever program produces these events uses the same units.
    // Even so, make some attempt to adjust units; this assume that
    // the creating program specified the units for the HepMC3 event.

    auto pscale = 1.0;
    if ( a_hepmc->length_unit() == HepMC3::Units::CM )
      pscale = 10.;
    auto escale = 1.0;
    if ( a_hepmc->momentum_unit() == HepMC3::Units::GEV )
      escale = 1000.;

    for (auto vertex: a_hepmc->vertices()) {

      auto position = vertex->position();

      auto g4vertex
	= new G4PrimaryVertex(position.x() * pscale, 
			      position.y() * pscale, 
			      position.z() * pscale, 
			      position.t() );
      a_event->AddPrimaryVertex(g4vertex);

      // For the purposes of Geant4, we're only interested in the
      // outgoing particles from this primary event.
      for (auto particle: vertex->particles_out()) {

	auto pdgCode = particle->pid();
	auto momentum = particle->momentum();

	auto g4particle 
	  = new G4PrimaryParticle(pdgCode,
				  momentum.px() * escale,
				  momentum.py() * escale,
				  momentum.pz() * escale,
				  momentum.e () * escale );

	// It's not likely, but what if the user supplied a PDG code
	// that Geant4 does not recognize?
	if ( !g4particle->GetParticleDefinition() ) {
	  G4ExceptionDescription description;
	  description << "File " << __FILE__ << " Line " << __LINE__ << " " << G4endl
		      << "could not interpret PDG code '"
		      << pdgCode << "'; particle skipped";
	  G4Exception("gramsg4::HepMC3GeneratorAction::HepMC2G4","invalid PDG code",
		      JustWarning, description);
	}

	// Despite the name, this method adds a new G4PrimaryParticle
	// to a G4PrimaryVertex.
	g4vertex->SetPrimary( g4particle );

      } // for each outgoing particle

    } // for each vertex 
  }

} // namespace gramsg4
