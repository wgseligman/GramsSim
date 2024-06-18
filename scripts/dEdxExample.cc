// dEdxExample.cc
// 18-Jun-2024 William Seligman <seligman@nevis.columbia.edu>

// This is an overly-commented example program to illustrate
// how to read the ROOT files produced by GramsSim. 

// This program will automatically be compiled during the cmake/make
// process for GramsSim, from its source in GramsSim/scripts. 

// If you want to copy this program, make changes, and run it, and
// assuming that you're working in a build directory set up according
// to the instructions in
// https://github.com/wgseligman/GramsSim/tree/develop, you can
// compile the program with:

/*
g++ <program-name>.cc `root-config --cflags --libs` \
   -I../GramsSim/GramsDataObj/include \
   ./GramsDataObj/libGramsSimProjectDataObj.so \
   -o <program-name>
*/

// From the GramsDataObj library, include all the data objects that
// we'll read. For this particular example task, calculating dE/dx for
// LAr energy deposits, these are all that we need.
#include "EventID.h"
#include "MCLArHits.h"

// ROOT classes that we'll use in this program.
#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TH1D.h>

// The C++ includes. These are for C++ language features that,
// historically, were not part of the C++ base language.
#include <string>
#include <iostream>

// Debug flag.
static const bool debug = false;

// Every C++ program must have a main routine.
int main( int, char**  ) {

  // This is the standard procedure for reading branches from a tree:

  // - Open the input file.
  auto input = TFile::Open("gramsg4.root");
  if ( input == nullptr ) {
    std::cerr << "Could not open file 'gramsg4.root'" << std::endl;
    exit(EXIT_FAILURE);
  }

  // - Open the tree within the input file.
  auto tree = input->Get<TTree>("gramsg4");
  if ( tree == nullptr ) {
    std::cerr << "Could not access tree 'gramsg4'" << std::endl;
    exit(EXIT_FAILURE);
  }

  // - Define the TTreeReader for this tree.
  auto reader = new TTreeReader(tree);

  // - Define the branches (columns) within the tree that we'll
  // read. Note that the custom classes (e.g., grams::EventID) are
  // defined in GramsDataObj.

  // Also note that the TTreeReaderValue acts like a pointer. For
  // example, we'd have to refer to (*eventID) in the code below.
  TTreeReaderValue<grams::EventID>   eventID( *reader, "EventID" );
  TTreeReaderValue<grams::MCLArHits> mcLArHits( *reader, "LArHits" );

  // The vague purpose of this example program is to calculate
  // dE/dx. What we do with that value is up to us. As an example,
  // we'll just make a histogram. Note that we're not doing anything
  // with that histogram, not even writing or drawing it.

  auto dEdxHistogram = new TH1D("dEdx","Histogram of dE/dx",100,0.0,1.0);

  // Read through the file.
  while ( reader->Next() ) {

    // (*MCLArHits) is a std::map (you can check this by looking at
    // GramsSim/GramsDataObj/include/MCLArHits.h). A map is vaguely
    // like a Python dict, in that it stores information in key-value
    // pairs. Here's how to loop over every value in a map:

    for ( const auto& [ key, mcLArHit ] : (*mcLArHits) ) {

      // For the list of ways to access the information in
      // grams::MCLArHit, again see
      // GramsSim/GramsDataObj/include/MCLArHits.h.

      auto energy = mcLArHit.Energy();
      auto start = mcLArHit.Start4D();
      auto end = mcLArHit.End4D();

      // We've got the "dE" in dE/dx, now we have to get the
      // "dx". Take the different between the 4-vectors:
      auto diff = start - end;

      // Now take the 3D magnitude of the difference.
      auto dx = diff.R();

      if ( dx > 0. ) {
	auto dEdx = energy / dx;
	dEdxHistogram->Fill( dEdx );
      }

    } // for every hit in the map
  } // for every row in the tree

} // end of 'main'
