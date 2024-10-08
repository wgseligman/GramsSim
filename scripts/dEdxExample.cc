// dEdxExample.cc
// 18-Jun-2024 William Seligman <seligman@nevis.columbia.edu>

// This is an overly-commented example program to illustrate how to
// read the map-based ROOT files produced by GramsSim. It is _not_ an
// example of how to perform a meaningful analysis on data. For that,
// see the ROOT tutorial at
// https://www.nevis.columbia.edu/~seligman/root-class/html/

// This program will automatically be compiled during the cmake/make
// process for GramsSim, from its source in GramsSim/scripts. 

// You can copy this program and make changes to suit your work. If
// you do, you'll have to compile it "by hand". If <program-name>.cc
// is located in the build directory you set up according to the
// instructions in
// https://github.com/wgseligman/GramsSim/tree/develop, you can
// compile it with:

/*
g++ -o <program-name> <program-name>.cc \
   `root-config --cflags --libs` \
   -Iinclude -Wl,-rpath,. -L. -lDictionary
*/

// From the GramsDataObj library, include all the data objects that
// we'll read. For this particular example task, calculating dE/dx for
// LAr energy deposits, this is all that we need.
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

  // The vague purpose of this example program is to calculate
  // dE/dx. What we do with that value is up to us. As an example,
  // we'll just make a histogram. Note that we're not doing anything
  // with that histogram, not even writing or drawing it.

  auto dEdxHistogram = new TH1D("dEdx","Histogram of dE/dx",100,0.0,0.005);

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
  // read. Note that the custom classes (e.g., grams::MCLArHits) are
  // defined in GramsDataObj.

  // Also note that the TTreeReaderValue acts like a pointer. For
  // example, we'd have to refer to (*mcLArHits) in the code below.
  TTreeReaderValue<grams::MCLArHits> mcLArHits( *reader, "LArHits" );

  // Read through the file.
  while ( reader->Next() ) {

    // (*MCLArHits) is a std::map (you can check this by looking at
    // include/MCLArHits.h). A map is vaguely like a Python dict, in
    // that it stores information in key-value pairs. Here's how to
    // loop over every (key,value) pair in a map:

    for ( const auto& [ key, mcLArHit ] : (*mcLArHits) ) {

      // For the list of ways to access the information in
      // grams::MCLArHit, again see include/MCLArHits.h.

      auto energy = mcLArHit.Energy();
      auto start = mcLArHit.Start4D();
      auto end = mcLArHit.End4D();

      // We've got the "dE" in dE/dx, now we have to compute the
      // "dx". Take the different between the 4-vectors:
      auto diff = start - end;

      // Now take the 3D magnitude of the difference.
      auto dx = diff.R();

      // Accumulate the dE/dx values in a histogram. 
      if ( dx > 0. ) {
	auto dEdx = energy / dx;
	dEdxHistogram->Fill( dEdx );
      }

    } // for every hit in the map
  } // for every row in the tree

} // end of 'main'
