// -*- C++ -*-
//
// 08-Dec-2021 WGS: Generate photon events based on spherical
// distributions (i.e., the sky) around the GRAMS detector.
//

#include "ParticleInfo.h"
#include "ParticleGeneration.h"
#include "PositionGenerator.h"
#include "Options.h" // in util
 
#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/Writer.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/FourVector.h"

#include <cmath>
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>

/** Main program */
int main(int argc, char** argv) {

  // Initialize the options from the XML file and the
  // command line. Make sure this happens first!
  auto options = util::Options::GetInstance();

  // Note that in this case, as long as we call the executable
  // "gramssky", we could omit the third argument to ParseOptions. See
  // util/README.md for how this works.
  auto result = options->ParseOptions(argc, argv, "gramssky");

  // Abort if we couldn't parse the job options.
  if (! result) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << argv[0] << ": Aborting job due to failure to parse options"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // We may not be debugging this at this moment, but if we
  // want to use the debug flag later in this routine,
  // let's have it ready.
  bool debug;
  options->GetOption("debug",debug);

  // Check for help message.
  bool help;
  options->GetOption("help",help);
  if (help) {
    options->PrintHelp();
    exit(EXIT_SUCCESS);
  }

  bool verbose;
  options->GetOption("verbose",verbose);
  if (verbose) {
    // Display all program options.
    options->PrintOptions();
  }

  // Get output file name.
  std::string outputFileName;
  options->GetOption("outputfile",outputFileName);

  // Does the output file have an extension (like ".txt")?
  if ( outputFileName.find(".") == std::string::npos ) {
    // The output file name does not contain an extension. Just to
    // keep "mystery files" from proliferating, append ".hepmc3" to
    // the end of the file's name so the user knows what kind of data
    // is in the file.
    outputFileName += std::string(".hepmc3");
  }

  // Pick an output method format for HepMC3. WriterAscii writes files
  // in the standard HepMC3 format, which is the most human-readable,
  // but there are other writer methods (e.g., WriterROOT). Note that
  // instead of using "new" for a standard C++ pointer, I'm using
  // shared_ptr created by make_shared, so I don't have to worry about
  // when to delete the pointer.
  auto writer = std::make_shared<HepMC3::WriterAscii>(outputFileName);

  // Number of events to generate.
  int numberOfEvents;
  options->GetOption("events",numberOfEvents);

  // The ParticleGeneration class will select which procedure we'll
  // use to generate particles. 
  auto pg = std::make_shared<gramssky::ParticleGeneration>();
  // GetGenerator returns std::shared_ptr<PositionGenerator>. 
  auto generator = pg->GetGenerator();

  // Setting up units based on the Options XML file.
  std::string lengthText, energyText;
  options->GetOption("LengthUnit",lengthText);
  options->GetOption("EnergyUnit",energyText);
  
  // Since users are crazy, convert the text to lower case.
  auto lowC = [](unsigned char c){ return std::tolower(c); };
  std::transform(lengthText.begin(), lengthText.end(), lengthText.begin(), lowC);
  std::transform(energyText.begin(), energyText.end(), energyText.begin(), lowC);

  auto lengthUnit = HepMC3::Units::CM;
  auto energyUnit = HepMC3::Units::MEV;

  if (debug) {
    std::cout << "gramssky: lengthText = " << lengthText
	      << ", mm compare = " << lengthText.compare("mm")
	      << ", energyText = " << energyText
	      << ", gev compare = " << energyText.compare("gev")
	      << std::endl;
  }

  if ( lengthText.compare("mm") == 0 )
    lengthUnit = HepMC3::Units::MM;
  if ( energyText.compare("gev") == 0 )
    energyUnit = HepMC3::Units::GEV;

  // Note that the HepMC3 format does not allow for an internal
  // specification of time units. Therefore, subsequent programs in
  // the simulation chain will assume that the time units are whatever
  // is in the Options XML file.

  // For each event:
  for ( int e = 0; e != numberOfEvents; ++e ) {

    // Generate returns a std::shared_ptr<gramssky::ParticleInfo>.
    auto info = generator->Generate();

    // To work with HepMC3, we have to create "vertices" and
    // "particles" separately, as pointers.
    auto vertex = std::make_shared<HepMC3::GenVertex>
      ( HepMC3::FourVector( info->GetX(), 
			    info->GetY(),
			    info->GetZ(),
			    info->GetT() ) );

    // status = 1 (in HepMC3 notation) means this is a final-state particle
    //        from the primary vertex.
    auto particle = std::make_shared<HepMC3::GenParticle>
      ( HepMC3::FourVector( info->GetPx(),
			    info->GetPy(),
			    info->GetPz(),
			    info->GetE() ),
	info->GetPDG(),
	1 );  // status

      // Make sure we set the correct units.
      HepMC3::GenEvent event(energyUnit,lengthUnit);

      // Assign an event number. Strictly speaking you don't have to
      // do this, but then each event would have an ID of zero, which
      // would make this program's output harder to analyze by hand.
      event.set_event_number( e );

      // If we had more than one vertex in this event we'd call this
      // once per vertex.
      event.add_vertex( vertex );

      // If we had more than one outgoing particle, we'd call this
      // once per particle.
      vertex->add_particle_out( particle );

      // Unless you include an incoming particle, the HepMC3 package
      // will have trouble reading HepMC3 events. This is a "dummy"
      // incoming particle; PDG code 0 is a "geantino", a massless
      // uncharged particle that doesn't do anything in Geant4.
      auto p0 = std::make_shared<HepMC3::GenParticle>
	( HepMC3::FourVector( 0.0, 0.0, 0.0, 0.0), 0,  0 );
      vertex->add_particle_in( p0 );
      
      // This is how you handle polarization. In general, you can add
      // arbitrary attributes to any event, vertex, or particle. See
      // HepMC3/Attributes.h for the different types available. Also
      // note that you can only add attributes to a vertex or a
      // particle _after_ you've added it to the event.

      auto theta = info->GetPolTheta();
      auto phi = info->GetPolPhi();
      // Skip if theta or phi polarization has not been set.
      if ( ! ( std::isnan(theta) || std::isnan(phi) ) ) {
	auto thetaAttribute = std::make_shared<HepMC3::DoubleAttribute>(theta);
	auto phiAttribute = std::make_shared<HepMC3::DoubleAttribute>(phi);
	particle->add_attribute("theta",thetaAttribute);
	particle->add_attribute("phi",phiAttribute); 
      }

      // Save event to output file
      writer->write_event(event);
  }

  writer->close();
  exit(EXIT_SUCCESS);
}
