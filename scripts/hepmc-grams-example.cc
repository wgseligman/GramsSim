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
 *  GramsG4 testing. See README.md in this directory for more
 *  information.
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
#include <cmath>

/** Main program */
int main() {

  // Fixed output file name. 
  std::string outputFile("example.hepmc3");

  // Pick an output method format for HepMC3. WriterAscii
  // writes files in the standard HepMC3 format, but there are
  // other writer methods (e.g., WriterROOT).
  auto writer = new HepMC3::WriterAscii(outputFile);

  // In this example, there's only one particle for each vertex.
  // To create code that might be used generally for future examples, create
  // them as (vertex,particle) pairs. Define a special type and
  // vector for this. 
  typedef std::pair < HepMC3::GenVertexPtr, HepMC3::GenParticlePtr > event_type;
  std::vector< event_type > events;

  // Create vertices and particles "by hand". The first three 
  // are 1-MeV photons coming from above the detector. The last
  // is an anti-deuteron appearing just off-center in the detector. 

  // The elements of an HepMC3::FourVector are:
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

  // The elements of an HepMC3::GenParticle are:
  //                       px   py   pz   e     pdgid status
  auto p1 = std::make_shared<HepMC3::GenParticle>
    ( HepMC3::FourVector( 0.0, 0.0, -1.0, 1.0  ),  22,  1 );
  auto p2 = std::make_shared<HepMC3::GenParticle>
    ( HepMC3::FourVector( 0.0, 0.0, -1.0, 1.0  ),  22,  1 );
  auto p3 = std::make_shared<HepMC3::GenParticle>
    ( HepMC3::FourVector( 0.0, 0.0, -1.0, 1.0  ),  22,  1 );
  auto p4 = std::make_shared<HepMC3::GenParticle>
    ( HepMC3::FourVector( 0.0, 0.0, 1.0, 1875.), -1000010020,  1 );

  // Create a vector of our custom-built (vertex,particle) pairs.
  events.push_back( event_type(v1, p1) );
  events.push_back( event_type(v2, p2) );
  events.push_back( event_type(v3, p3) );
  events.push_back( event_type(v4, p4) );

  // For each of the (vertex,particle) pairs we've created:
  for ( size_t i = 0; i < events.size(); ++i )
    {
      // Extract the vertex and the particle from the vector. 
      auto vp = events[i];
      auto vertex = vp.first;
      auto particle = vp.second;

      // Use Geant4 units for this example. 
      HepMC3::GenEvent event(HepMC3::Units::MEV,HepMC3::Units::MM);
      
      // Assign an event number. Strictly speaking ou don't have to do 
      // this, but then each event would have an ID of zero, which would make
      // the Geant4 output harder to analyze. 
      event.set_event_number( i );

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

      // Do this for one particular event, let's say ID = 2. We'll set
      // the polarization to be along the y-axis.
      if ( i == 2 ) {
	    double theta = M_PI / 2.0;
	    auto thetaAttribute = std::make_shared<HepMC3::DoubleAttribute>(theta);
	    particle->add_attribute("theta",thetaAttribute);
	    
	    double phi = 0.0;
	    auto phiAttribute = std::make_shared<HepMC3::DoubleAttribute>(phi);
	    particle->add_attribute("phi",phiAttribute); 
      }

      // Save event to output file
      writer->write_event(event);
      
    } // for each (vertex,particle) in our vector of examples.

  writer->close();
  delete writer;
  return 0;
}
