// -*- C++ -*-
//
// 13-Jan-2021 WGS: Create a few HepMC3 events to
// test the GramsG4 processing.
//
/**
 *  @example hepmc-grams-example.cc
 *  @brief Create example events.
 *
 *  An extremely simple example of creating events for HepMC3 and
 *  GramsG4 testing. It's so simple (or perhaps the programmer is so
 *  simple) that it isn't even automatically compiled by CMake; the
 *  compilation command is:
 *
 *  g++ hepmc-grams-example.cc `HepMC3-config --cflags --libs` -o hepmc-grams-example
 *
 */
 
#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/Writer.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/FourVector.h"

#include <cstring>
#include <iostream>
#include <vector>

/** Main program */
int main() {

  // Fixed output file name. 
  std::string outputFile("example.hepmc3");

  auto writer = new HepMC3::WriterAscii(outputFile);

  // In this example, there's only one particle for each vertex.
  typedef std::pair < HepMC3::GenVertexPtr, HepMC3::GenParticlePtr > event_type;
  std::vector< event_type > events;

  // Creating vertices and particles "by hand". The first three 
  // are 1-MeV photons coming from above the detector. The last
  // is an anti-deuteron appearing just off-center in the detector. 

  //                       x    y       z    t
  auto v1 = std::make_shared<HepMC3::GenVertex>
    ( HepMC3::FourVector( 0.0, 0.0, 2000.0, 0.0 ) );
  auto v2 = std::make_shared<HepMC3::GenVertex>
    ( HepMC3::FourVector( 0.0, 0.0, 2000.0, 0.0 ) );
  auto v3 = std::make_shared<HepMC3::GenVertex>
    ( HepMC3::FourVector( 0.0, 0.0, 2000.0, 0.0 ) );
  auto v4 = std::make_shared<HepMC3::GenVertex>
    ( HepMC3::FourVector(10.0,10.0, -100.0, 0.0 ) );

  // pdg id 22 = photon
  // Nuclear PDG codes are of the form ±10LZZZAAAI; 
  //        the anti-deuteron is therefore -1000010020.
  // status = 1 (in HepMC3 notation) means this is a final-state particle
  //        from the primary vertex.

  //                       px   py   pz   e     pdgid status
  auto p1 = std::make_shared<HepMC3::GenParticle>
    ( HepMC3::FourVector( 0.0, 0.0, 1.0, 1.0  ),  22,  1 );
  auto p2 = std::make_shared<HepMC3::GenParticle>
    ( HepMC3::FourVector( 0.0, 0.0, 1.0, 1.0  ),  22,  1 );
  auto p3 = std::make_shared<HepMC3::GenParticle>
    ( HepMC3::FourVector( 0.0, 0.0, 1.0, 1.0  ),  22,  1 );
  auto p4 = std::make_shared<HepMC3::GenParticle>
    ( HepMC3::FourVector( 0.0, 0.0, 1.0, 1875.), -1000010020,  1 );

  events.push_back( event_type(v1, p1) );
  events.push_back( event_type(v2, p2) );
  events.push_back( event_type(v3, p3) );
  events.push_back( event_type(v4, p4) );

  for ( auto i = 0; i < events.size(); ++i )
    {
      auto vp = events[i];
      auto vertex = vp.first;
      auto particle = vp.second;

      // Use Geant4 units for this example. 
      HepMC3::GenEvent event(HepMC3::Units::MEV,HepMC3::Units::MM);

      // If we had more than one outgoing particle, we'd call this
      // once per particle.
      vertex->add_particle_out( particle );

      // Similarly, if we had more than one vertex in this event we'd
      // call this once per vertex.
      event.add_vertex( vertex );
      
      // Assign an event number. You don't have to do this, but then
      // each event has an ID of zero.
      event.set_event_number( i );

      // This is how you handle polarization. Of course, since we're
      // boring, we'll set it to zero. (In general, you can add
      // arbitrary attributes any event, vertex, or particle. See
      // HepMC3/Attributes.h for the different types available. Also
      // note that you can only add attributes to a vertex or a
      // particle _after_ you've added it to the event.)

      if ( i == 2 ) {
	double theta = 0.0;
	particle->add_attribute("theta",std::make_shared<HepMC3::DoubleAttribute>(theta));
	double phi = 0.0;
	particle->add_attribute("phi",std::make_shared<HepMC3::DoubleAttribute>(phi)); 
      }

      // Save event to output file
      writer->write_event(event);
    }

  writer->close();
  delete writer;
  return 0;
}
