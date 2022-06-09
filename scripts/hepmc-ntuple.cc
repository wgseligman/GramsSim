// -*- C++ -*-
//
// 06-Feb-2022 WGS: a simple conversion program to go
// from an HepMC3 file into a ROOT ntuple. 
// The main purpose of this program is to debug the
// output of GramsSky.
//
/**
 *  @example hepmc-ntuple.cc
 *  @brief Convert from HepMC3 to ROOT..
 */
 
#include "HepMC3/Attribute.h"
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

// ROOT includes
#include "TFile.h"
#include "TTree.h"

// C++ includes
#include <cstring>
#include <iostream>
#include <algorithm>

/** Main program */
int main(int argc, char **argv) {

  // Crude argument parsing: <program> [input-file] [output-file].
  std::string inputFile("gramssky.hepmc3");
  std::string outputFile("hepmc-ntuple.root");
  if( argc > 2 ) {
    outputFile = std::string(argv[2]);
  }
  if( argc > 1 ) {
    inputFile = std::string(argv[1]);
  }

  std::cout << argv[0] << ": Reading from '"
	    << inputFile << "', Writing to '"
	    << outputFile << "'" << std::endl;
  
  // Parse the input file name, looking for its extension.
  auto search = inputFile.find('.');
  if ( search == std::string::npos ) {
    std::cerr 
      << "No '.' in input file name, so I can't determine the HepMC3 file type." 
      << std::endl;
    exit(-1);
  }

  // Get the characters after the '.'.
  std::string extension;
  if ( search + 2 < inputFile.size() )
    extension = inputFile.substr( search+1 );
  else {
    std::cerr 
      << "Nothing after the '.' in input file name, so I can't determine the HepMC3 file type." 
      << std::endl;
    exit(-1);
  }
  
  // Convert to lower case
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);  
  HepMC3::Reader* reader;
  if ( extension == "hepmc2" ) 
    reader = new HepMC3::ReaderAsciiHepMC2(inputFile);
  else if ( extension == "hepmc3" )
    reader = new HepMC3::ReaderAscii(inputFile);
  else if ( extension == "hpe" )
    reader = new HepMC3::ReaderHEPEVT(inputFile);
  else if ( extension == "lhef" )
    reader = new HepMC3::ReaderLHEF(inputFile);
#ifdef HEPMC3_ROOTIO_INSTALLED
  else if ( extension == "root" )
    reader = new HepMC3::ReaderRoot(inputFile);
  else if ( extension == "treeroot" )
    reader = new HepMC3::ReaderRootTree(inputFile);
#endif
  else {
    std::cerr 
      << "Did not recognize extension '"
      << extension << "' as a valid HepMC3 input file type"
      << std::endl;
    exit (-1);
  }
  
  // Create a new TTree within the output file.
  TFile* output = new TFile(outputFile.c_str(), "RECREATE");
  TTree* ntuple = new TTree("hepmc","Values from HepMC3 file");
  
  // Variables for ntuple.
  int Event;
  int pdgCode;
  double x;
  double y;
  double z;
  double t;
  double Px;
  double Py;
  double Pz;
  double energy;
  
  // Define the columns of the ntuple.
  ntuple->Branch("Event", &Event, "Event/I");
  ntuple->Branch("PDG", &pdgCode, "PDG/I");
  ntuple->Branch("x", &x, "x/D");
  ntuple->Branch("y", &y, "y/D");
  ntuple->Branch("z", &z, "z/D");
  ntuple->Branch("t", &t, "t/D");
  ntuple->Branch("Px", &Px, "Px/D");
  ntuple->Branch("Py", &Py, "Py/D");
  ntuple->Branch("Pz", &Pz, "Pz/D");
  ntuple->Branch("E", &energy, "E/D");
  
  int nevents = 0;
  
  // For each event in the input file.  
  while( !reader->failed() ) {

    HepMC3::GenEvent event;

    // Read event from input file
    reader->read_event(event);
    
    // If reading failed - exit loop
    if( reader->failed() ) break;
    
    // Fetch the values from the HepMC record and create a row for the
    // ntuple.
    
    Event = event.event_number();
    
    // For each vertex in the event:
    for (auto vertex: event.vertices()) {
      
      auto position = vertex->position();
      x = position.x();
      y = position.y();
      z = position.z();
      t = position.t();
      
      // For each particle coming out of the vertex:
      for (auto particle: vertex->particles_out()) {
	
        pdgCode = particle->pid();
        auto momentum = particle->momentum();
	Px = momentum.px();
	Py = momentum.py();
	Pz = momentum.pz();
	energy = momentum.e();
	
	// Add the ntuple row. 
	ntuple->Fill();
      }
    }
    nevents++;

  } // end of input?

  std::cout << "Number of HepMC3 events converted = "
	    << nevents << std::endl;

  reader->close();
  ntuple->Write();
  output->Close();
  return 0;
}
