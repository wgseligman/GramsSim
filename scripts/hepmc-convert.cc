// -*- C++ -*-
//
// 13-Jan-2021 WGS: a simple conversion program to go
// from one HepMC3 file format to another.
//
/**
 *  @example hepmc-convert.cc
 *  @brief Convert between HepMC3 file format.
 *
 *  This is a simple HepMC3 file converter. See the README.md in this
 *  directory for more information.
 *
 */
 
#include "HepMC3/Attribute.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Reader.h"
#include "HepMC3/ReaderAscii.h"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/ReaderHEPEVT.h"
#include "HepMC3/ReaderLHEF.h"
#include "HepMC3/Writer.h"
#include "HepMC3/WriterAscii.h"
#include "HepMC3/WriterAsciiHepMC2.h"
#include "HepMC3/WriterHEPEVT.h"
#ifdef HEPMC3_ROOTIO_INSTALLED
#include "HepMC3/ReaderRoot.h"
#include "HepMC3/ReaderRootTree.h"
#include "HepMC3/WriterRoot.h"
#include "HepMC3/WriterRootTree.h"
#endif

#include <cstring>
#include <iostream>
#include <algorithm>

/** Main program */
int main(int argc, char **argv) {

  if( argc<3 ) {
    std::cout << "Usage: " << argv[0] << " <HepMC3_input_file> <HepMC3_output_file>" << std::endl;
    exit(-1);
  }

  // Parse the input file name, looking for its extension.
  std::string inputFile(argv[1]);
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


  // Parse the output file name, looking for its extension.
  std::string outputFile(argv[2]);
  search = outputFile.find('.');
  if ( search == std::string::npos ) {
    std::cerr 
      << "No '.' in output file name, so I can't determine the HepMC3 file type." 
      << std::endl;
    exit(-1);
  }

  // Get the characters after the '.'.
  if ( search + 2 < outputFile.size() )
    extension = outputFile.substr( search+1 );
  else {
    std::cerr 
      << "Nothing after the '.' in output file name, so I can't determine the HepMC3 file type." 
      << std::endl;
    exit(-1);
  }
  
  // Convert to lower case
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);  
  HepMC3::Writer* writer;
  if ( extension == "hepmc2" ) 
    writer = new HepMC3::WriterAsciiHepMC2(outputFile);
  else if ( extension == "hepmc3" )
    writer = new HepMC3::WriterAscii(outputFile);
  else if ( extension == "hpe" )
    writer = new HepMC3::WriterHEPEVT(outputFile);
#ifdef HEPMC3_ROOTIO_INSTALLED
  else if ( extension == "root" )
    writer = new HepMC3::WriterRoot(outputFile);
  else if ( extension == "treeroot" )
    writer = new HepMC3::WriterRootTree(outputFile);
#endif
  else {
    std::cerr 
      << "Did not recognize extension '"
      << extension << "' as a valid HepMC3 output file type"
      << std::endl;
    exit (-1);
  }

  int nevents = 0;

  while( !reader->failed() ) {

    HepMC3::GenEvent event;

    // Read event from input file
    reader->read_event(event);

    // If reading failed - exit loop
    if( reader->failed() ) break;

    // Save event to output file
    writer->write_event(event);

    nevents++;

  } // end of input?

  std::cout << "Number of HepMC3 events converted = "
	    << nevents << std::endl;

  reader->close();
  writer->close();
  return 0;
}
