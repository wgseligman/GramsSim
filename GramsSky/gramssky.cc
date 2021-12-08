// -*- C++ -*-
//
// 08-Dec-2021 WGS: Generate photon events based on spherical
// distributions (i.e., the sky) around the GRAMS detector.
//

#include "ParticleInfo.h"
#include "PrimaryGenerator.h"
#include "MonoPrimaryGenerator.h"
#include "Options.h" // in util

// ROOT includes
#include "TRandom.h"
 
#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/Writer.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/FourVector.h"

#include <string>
#include <iostream>
#include <memory>

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

  // Get and set the random number seed.
  int seed;
  options->GetOption("rngseed",seed);
  // Note that the default random-number generator in ROOT is
  // TRandom3.
  gRandom->SetSeed(seed);

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
  // but there are other writer methods (e.g., WriterROOT).
  auto writer = std::make_shared<HepMC3::WriterAscii>(outputFileName);

  // Number of events to generate.
  int numberOfEvents;
  options->GetOption("events",numberOfEvents);

  auto generator = std::make_shared<gramssky::MonoPrimaryGenerator>();

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

      // Use Geant4 units. 
      HepMC3::GenEvent event(HepMC3::Units::MEV,HepMC3::Units::MM);

      // Assign an event number. Strictly speaking you don't have to
      // do this, but then each event would have an ID of zero, which
      // would make the Geant4 output harder to analyze.
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
      // uncharged particle that doesn't do anything.
      auto p0 = std::make_shared<HepMC3::GenParticle>
	( HepMC3::FourVector( 0.0, 0.0, 0.0, 0.0), 0,  0 );
      vertex->add_particle_in( p0 );
      
      // This is how you handle polarization. In general, you can add
      // arbitrary attributes to any event, vertex, or particle. See
      // HepMC3/Attributes.h for the different types available. Also
      // note that you can only add attributes to a vertex or a
      // particle _after_ you've added it to the event.

      auto thetaAttribute = std::make_shared<HepMC3::DoubleAttribute>(info->GetPolTheta());
      particle->add_attribute("theta",thetaAttribute);
      auto phiAttribute = std::make_shared<HepMC3::DoubleAttribute>(info->GetPolPhi());
      particle->add_attribute("phi",phiAttribute); 

      // Save event to output file
      writer->write_event(event);
  }

  writer->close();
  exit(EXIT_SUCCESS);
}
