/// \file GramsG4/src/GramsG4HepMC3GeneratorAction.cc
/// \brief Implementation of the GramsG4HepMC3GeneratorAction class
//
// Portions of this code were copied from Beam Delivery Simulation
// (BDSIM), University of London
// 
// ... those portions, in turn, came from Geant4 example HepMCEx01. 

// Is HepMC3 even defined for this system?
#ifdef HEPMC3_INSTALLED

#include "GramsG4HepMC3GeneratorAction.hh"

// Accommodate different Geant4 versions.
#include "G4Version.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Exception.hh"
#include "G4SystemOfUnits.hh"

#include "Options.h" // in util/
 
#include "HepMC3/Attribute.h"
#include "HepMC3/GenRunInfo.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Reader.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/ReaderHEPEVT.h"
#include "HepMC3/ReaderLHEF.h"
#ifdef HEPMC3_ROOTIO_INSTALLED
#include "HepMC3/ReaderRoot.h"
#include "HepMC3/ReaderRootTree.h"
#endif

#include <string> 
#include <memory>

namespace gramsg4 {

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  HepMC3GeneratorAction::HepMC3GeneratorAction(const std::string& a_inputGen)
    : G4VUserPrimaryGeneratorAction()
    , m_inputFile(a_inputGen)
    , m_reader(nullptr)
    , m_hepmcEvent(nullptr)
  {
    // Access the set of program options.
    m_options = util::Options::GetInstance();

    m_options->GetOption("debug",m_debug);
    m_options->GetOption("verbose",m_verbose);

    // Get the units of time. (The length and energy units are read in
    // on an event-by-event basis in the "HepMC2G4" method.)
    std::string units;
    m_options->GetOption("TimeUnit",units);
    m_timeScale = ns;
    if ( units == "s" ) m_timeScale = second;
    if ( units == "ms" ) m_timeScale = millisecond;

    // Do we override the run/event numbers normally assigned by
    // Geant4 with the values in the input file? The answer: If the
    // user has supplied default values for run and starting event
    // number in the GramsG4 section of the options file, then the
    // values in the input file take precedence. If the user has
    // explicitly defined values for the run and starting event number
    // for GramsG4, then the values in the options file override any
    // values in the HepMC3 file.
    m_useHepMC3RunNumber=false;
    m_useHepMC3EventNumber=false;

    int runNumber, startingEventNumber;
    bool result = m_options->GetOption("run",runNumber);
    if ( !result  ||  runNumber < 0 ) {
      m_useHepMC3RunNumber=true;
      if (m_verbose)
	G4cout << "GramsG4HepMC3GeneratorAction::GramsG4HepMC3GeneratorAction() - "
	       << "Override run number with value from HepMC3 file" 
	       << G4endl;
    }

    result = m_options->GetOption("startEvent",startingEventNumber);
    if ( !result  ||  startingEventNumber < 0 ) {
      m_useHepMC3EventNumber=true;
      if (m_verbose) 
	G4cout << "GramsG4HepMC3GeneratorAction::GramsG4HepMC3GeneratorAction() - "
	       << "Override event numbers with values from HepMC3 file" 
	       << G4endl;
    }

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

    if ( m_debug ) {
      G4cout << "GramsG4HepMC3GeneratorAction::OpenFile - "
	     << "extension='" << extension << "'" 
	     << G4endl;
    }

#if G4VERSION_NUMBER<1100
    extension.toLower();
#else
    G4StrUtil::to_lower(extension);
#endif

    if ( extension == "hepmc2" ) 
      m_reader = new HepMC3::ReaderAsciiHepMC2(m_inputFile);
    else if ( extension == "hepmc3" )
      m_reader = new HepMC3::ReaderAscii(m_inputFile);
    else if ( extension == "hpe" )
      m_reader = new HepMC3::ReaderHEPEVT(m_inputFile);
    else if ( extension == "lhef" )
      m_reader = new HepMC3::ReaderLHEF(m_inputFile);
#ifdef HEPMC3_ROOTIO_INSTALLED
    else if ( extension == "root" ) {
      m_CopyOptions(m_inputFile);
      m_reader = new HepMC3::ReaderRoot(m_inputFile);
    }
    else if ( extension == "roottree" ) {
      m_CopyOptions(m_inputFile);
      m_reader = new HepMC3::ReaderRootTree(m_inputFile);
    }
#endif
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
    auto options = util::Options::GetInstance();
    G4bool debug;
    options->GetOption("debug",debug);

    // If the input file has a run number, and we're going to use that
    // number in Geant4, then that run information is associated only
    // with the first event in the input file.
    if ( m_useHepMC3RunNumber ) {
      // Does the event have a GenRunInfo attached to it?
      auto runInfo = a_hepmc->run_info();
      if ( runInfo ) {
	// In HepMC3, there is no dedicated field for run number. In GramsSky,
	// the run number is attached to GenRunInfo as an attribute.
	auto runNumber = runInfo->attribute<HepMC3::IntAttribute>("RunNumber");
	// If that attribute is present, set the G4 Run number. Note
	// that we set the new run number in both the G4RunManager and
	// G4Run; primarily because in multi-threaded applications the
	// relationship between the G4MTRunManager and the run number
	// is not well defined.
	if ( runNumber ) {
	  auto run = runNumber->value();
	  G4RunManager::GetRunManager()->SetRunIDCounter( run );
	  G4RunManager::GetRunManager()->GetNonConstCurrentRun()->SetRunID( run );
	  if (m_verbose)
	    G4cout << "GramsG4HepMC3GeneratorAction::G4HepMC2G4 - "
		   << "Set run number to " << run
		   << G4endl;
	}
      }
      m_useHepMC3RunNumber = false;
    }

    // Do we want Geant4 to use the event number from the HepMC3 file?
    if ( m_useHepMC3EventNumber ) {
      auto eventNumber = a_hepmc->event_number();
      a_event->SetEventID( eventNumber );
      if (m_verbose)
	G4cout << "GramsG4HepMC3GeneratorAction::G4HepMC2G4 - "
	       << "Set event number to " << eventNumber
	       << G4endl;
    }    

    // Note that Geant4 and CLHEP use MeV/mm/ns. Make sure that
    // whatever program generates these primary events uses the same
    // units.  Even so, make some attempt to adjust units; this
    // assumes that the creating program specified the units for the
    // HepMC3 event. "mm", "cm", "MeV", and "GeV" are defined in
    // G4SystemOfUnits.hh.

    auto lengthScale = mm;
    if ( a_hepmc->length_unit() == HepMC3::Units::CM )
      lengthScale = cm;
    auto energyScale = MeV;
    if ( a_hepmc->momentum_unit() == HepMC3::Units::GEV )
      energyScale = GeV;

    // Set up a G4 world-volume test.
    G4Navigator* navigator = G4TransportationManager::GetTransportationManager()
      -> GetNavigatorForTracking();
    G4VPhysicalVolume* world = navigator->GetWorldVolume();
    G4VSolid* worldSolid = world->GetLogicalVolume()->GetSolid();
 
    if ( m_debug ) {
      G4cout << "GramsG4HepMC3GeneratorAction::HepMC2G4 - " 
	     << " Event number from '" << m_inputFile 
	     << "' = " << a_hepmc->event_number() 
	     << "; Geant4 event ID = " << a_event->GetEventID() << G4endl
	     << " Number of vertices = " << (a_hepmc->vertices()).size()
	     << G4endl;
    }

    // For each vertex in the event:
    for (auto vertex: a_hepmc->vertices()) {

      auto position = vertex->position();
      G4ThreeVector xyz(position.x() * lengthScale, 
			position.y() * lengthScale, 
			position.z() * lengthScale );
 
      if ( m_debug ) {
	G4cout << "GramsG4HepMC3GeneratorAction::HepMC2G4 - "
	       << "vertex (" << xyz.x() << "," << xyz.y() << "," << xyz.z()
	       << ")" << G4endl;
      }

      // If the vertex is outside the Geant4 world volume, G4 will
      // crash spectacularly. Let's put in an explicit test to keep
      // the simulation going.
      auto isInside = worldSolid->Inside(xyz);
      if ( isInside != kInside ) {
	  G4ExceptionDescription description;
	  description << "File " << __FILE__ << " Line " << __LINE__ << " " << G4endl
		      << " Event number from '" << m_inputFile 
		      << "' = " << a_hepmc->event_number() 
		      << "; Geant4 event ID = " << a_event->GetEventID() << G4endl
		      << "vertex (" << xyz.x() << "," << xyz.y() << "," << xyz.z()
		      << ") is outside of the World Volume; vertex skipped.";
	  G4Exception("gramsg4::HepMC3GeneratorAction::HepMC2G4","invalid vertex",
		      JustWarning, description);
	  continue;
      }
 
      if ( m_debug ) {
	G4cout << "GramsG4HepMC3GeneratorAction::HepMC2G4 - "
	       << "adding vertex" 
	       << G4endl;
      }

      auto g4vertex = new G4PrimaryVertex( xyz, position.t()*m_timeScale );
      a_event->AddPrimaryVertex(g4vertex);
 
      if ( m_debug ) {
	G4cout << "GramsG4HepMC3GeneratorAction::HepMC2G4 - " 
	       << "   Number of particles in this vertex = " 
	       << (a_hepmc->particles()).size()
	       << G4endl;
      }

      // For the purposes of Geant4, we're only interested in the
      // outgoing particles from this primary event. For each outgoing
      // particle in this vertex:
      for (auto particle: vertex->particles_out()) {

	auto pdgCode = particle->pid();
	auto momentum = particle->momentum();

	auto g4particle 
	  = new G4PrimaryParticle(pdgCode,
				  momentum.px() * energyScale,
				  momentum.py() * energyScale,
				  momentum.pz() * energyScale,
				  momentum.e () * energyScale );
 
	if ( m_debug ) {
	  G4cout << "GramsG4HepMC3GeneratorAction::HepMC2G4 - "
		 << "   PDG ID = " << pdgCode
		 << " momentum = (" 
		 << momentum.px() << "," 
		 << momentum.py() << "," 
		 << momentum.pz()
		 << ")" << G4endl;
	}

	// It's not likely, but what if the user supplied a PDG code
	// that Geant4 does not recognize?
	if ( !g4particle->GetParticleDefinition() ) {
	  G4ExceptionDescription description;
	  description << "File " << __FILE__ << " Line " << __LINE__ << " " << G4endl
		      << " Event number from '" << m_inputFile 
		      << "' = " << a_hepmc->event_number() 
		      << "; Geant4 event ID = " << a_event->GetEventID() << G4endl
		      << "could not interpret PDG code '"
		      << pdgCode << "'; particle skipped";
	  G4Exception("gramsg4::HepMC3GeneratorAction::HepMC2G4","invalid PDG code",
		      JustWarning, description);

	  delete g4particle;
	  continue;
	}

	// Polarization in HepMC3 is not stored in a dedicated
	// variable, but assigned an "attribute".
	auto theta = particle->attribute<HepMC3::DoubleAttribute>("theta");
	auto phi   = particle->attribute<HepMC3::DoubleAttribute>("phi");
	// If these pointers are not null, then fetch the value and
	// assign it to the particle's polarization.
	if (theta && phi) {
	  G4ThreeVector polarization(0,0,1);
	  polarization.setTheta( theta->value() );
	  polarization.setPhi  ( phi->value() );
	  g4particle->SetPolarization( polarization );
	}

	// Note: Weights in HepMC3 are stored as a vector of values.
	// It's not clear that there's a general way to interpret
	// this, so leave weights alone for now.

	if ( m_debug ) {
	  G4cout << "GramsG4HepMC3GeneratorAction::HepMC2G4 - "
		 << "   Adding particle" 
		 << G4endl;
	}

	// Despite the name, this method adds (not sets) a new
	// G4PrimaryParticle to a G4PrimaryVertex.
	g4vertex->SetPrimary( g4particle );

      } // for each outgoing particle

    } // for each vertex 
  }

#ifdef HEPMC3_ROOTIO_INSTALLED
  // If we're reading in a ROOT file, there may be an options ntuple
  // in it too. If there is, copy it to our list of options to
  // maintain a historical record.
  bool HepMC3GeneratorAction::m_CopyOptions(const std::string& inputFile) {
    // Open the file for the Options class, get the options from the
    // file, then close it before we actively start reading from it.
    auto input = new TFile(inputFile.c_str());
    bool success = m_options->CopyInputNtuple(input);
    input->Close();
    return success;
  }
#endif

} // namespace gramsg4

#endif // HepMC3 defined
