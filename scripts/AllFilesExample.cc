// AllFilesExample.cc
// 29-Jun-2024 William Seligman <seligman@nevis.columbia.edu>

// This is an overly-commented example program to illustrate how to
// read multiple map-based ROOT files produced by GramsSim as if they
// were all part of the same tree. It is _not_ an example of how to
// perform a meaningful analysis on data. For that, see the ROOT
// tutorial at
// https://www.nevis.columbia.edu/~seligman/root-class/html/

// At this moment, you're tempted to copy this program and blindly
// place your analysis code in the main program loop. DON'T DO THIS
// WITHOUT THINKING ABOUT WHAT YOU'RE DOING! Look at the smaller and
// perhaps more realistic examples in SimpleAnalysis, dEdxExample, and
// RadialDistance.

// This programs in GramsSim/scripts will automatically be compiled
// during the cmake/make process for GramsSim. For your own program,
// you'll have to compile it "by hand". If <program-name>.cc is
// located in the build directory you set up according to the
// instructions in
// https://github.com/wgseligman/GramsSim/tree/develop, you can
// compile it with:

/*
g++ -o <program-name> <program-name>.cc \
   `root-config --cflags --libs` \
   -Iinclude -Wl,-rpath,. -L. -lDictionary
*/

// From the GramsDataObj library, include all the data objects that
// we'll read. Since this is an example of how to read all the
// GramsSim trees at once, we'll need all the data objectes.
#include "EventID.h"
#include "MCTrackList.h"
#include "MCLArHits.h"
#include "ElectronClusters.h"
#include "ReadoutMap.h"
#include "ReadoutWaveforms.h"

// ROOT classes that we'll use in this program.
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

// The C++ includes. These are for C++ language features that,
// historically, were not part of the C++ base language.
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>

// Debug flag.
static const bool debug = false;

// Every C++ program must have a main routine.
int main( int, char**  ) {

  // Start by opening up one of the files. In order to keep the most
  // complete record of program options and detector geometry, this
  // should be the last file generated in the analysis chain that we
  // need for our task. See GramsSim/util/README.md for details.

  std::string inputFileName = "gramselecsim.root";
  auto inputFile = TFile::Open(inputFileName.c_str());
  if (inputFile == nullptr || inputFile->IsZombie()) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
              << "AllFilesExample: Could not open file '" << inputFileName << "'"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // Open the tree within that file.
  std::string treeName = "ElecSim";
  auto tree = inputFile->Get<TTree>(treeName.c_str());
  if (tree == nullptr) {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
              << "AllFilesExample: Could not open tree '" << treeName << "'"
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // Now we add additional columns/friends to that tree. (If you ask
  // how I knew the names of all these files and trees, I got them
  // from GramsSim/options.xml. If I were writing a more complete
  // program, I'd get them from using the Options class (see
  // GramsSim/util).)

  tree->AddFriend("ReadoutSim","gramsreadoutsim.root");
  tree->AddFriend("DetSim","gramsdetsim.root");
  tree->AddFriend("gramsg4","gramsg4.root");

  // Define the TTreeReader for this combined tree.
  auto reader = new TTreeReader(tree);

  // Create a TTreeReaderValue for each column in the combined tree
  // whose value we'll use. Note that we have multiple columns named
  // "EventID" in the combined tree, so specify which one to use.  In
  // this overly-ambitious piece of example code, we'll read every
  // column in the combined tree.

  // If you ask how I knew the names of all the columns, I got them
  // from either (a) looking at the program code, or (b) using the
  // ROOT TBrowser in an interactive ROOT session; the latter requires
  // the file GramsSim/rootlogon.C to be present to load the data
  // object dictionaries.

  // TTReaderValue behaves like a pointer. For example, we'll have to
  // use (*EventID) later in the code.
  
  TTreeReaderValue<grams::EventID>          EventID    (*reader, "ElecSim.EventID");
  TTreeReaderValue<grams::MCTrackList>      Tracks     (*reader, "TrackList");
  TTreeReaderValue<grams::MCLArHits>        Hits       (*reader, "LArHits");
  TTreeReaderValue<grams::ElectronClusters> Clusters   (*reader, "ElectronClusters");
  TTreeReaderValue<grams::ReadoutMap>       ReadoutMap (*reader, "ReadoutMap");
  TTreeReaderValue<grams::ReadoutWaveforms> Waveforms  (*reader, "ReadoutWaveforms");

  // For every event in the combined tree:
  while (reader->Next()) {

    // The following code does not represent a legitimate analysis
    // task. It consists of examples to illustrate how one might scan
    // through the maps, sets, and vectors that make up the data
    // objects.

    // IF YOU'RE COPYING THIS CODE LINE-FOR-LINE AND LOOP-FOR-LOOP,
    // STOP! Most of what the code does has no purpose other than to
    // serve as an example. Look at what the code is doing, and ask if
    // that's what you want to do.

    // For a list of the various methods you can use to access the
    // information in the data objects, see the header files in
    // GramsSim/GramsDataObj/include.

    // Let's pick an arbitrary event. How about run=0, event=3?
    static const auto arbitraryEvent = grams::EventID(0,3);

    // For only that event:
    if ( (*EventID) == arbitraryEvent ) {

      // For this event, let's go through all the tracks. Most of the
      // data objects based on maps, which are like Python dicts in
      // that they are (key,value) pairs. This is an example of how to
      // loop over all the (trackID, MCTrack) pairs in a MCTrackList.
      
      // A vector to save track IDs.
      std::vector<int> primaries;
      for ( const auto& [ trackID, track ] : (*Tracks) ) {
	
	// Let's look only for primary particles in the event.
	if ( track.Process() == "Primary" ) {
	  // Save the ID of this track.
	  primaries.push_back( trackID );
	}	  
      } // for each track in the event
      
      // Now look through all the hits.
      for ( const auto& [ hitID, hit ] : (*Hits) ) {

	// See if this hit was produced by a primary particle. (The
	// answer is likely to be "no" if the primary particle is a
	// photon.)
	const auto trackID = hit.TrackID();
	const auto search = std::find(primaries.cbegin(), primaries.cend(), trackID );

	// If we found one (not likely) print it out. Note that the
	// data objects all have output operators defined.
	if ( search != primaries.cend() ) 
	  std::cout << hit << std::endl;

      } // for each hit in the event

      // For all the electron clusters in the event:
      for ( const auto& [ key, cluster ] : (*Clusters) ) {

	// See if this cluster was produced by a primary
	// particle. (Again, probably not.)
	const auto trackID = cluster.TrackID();
	const auto search = std::find(primaries.cbegin(), primaries.cend(), trackID );

	// If we found one, print it out.
	if ( search != primaries.cend() ) 
	  std::cout << cluster << std::endl;

      } // for each cluster in the event.

      // Let's look through all the readout channels in event.
      for ( const auto& [ readoutID, waveform ] : (*Waveforms) ) {

	// Did any primary particle contribute to this waveform?
	// That's a trickier question to answer. There's a separate
	// object, ReadoutMaps, that links the waveforms to the
	// electron clusters. Look for this readoutID in that list.
	const auto search = ReadoutMap->find( readoutID );
	if ( search == ReadoutMap->cend() ) {
	  // This should not happen. It would mean that somehow a
	  // readout waveform exists but the corresponding electron
	  // clusters do not.
	  std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		    << "Inconsist waveform->cluster map"
		    << std::endl;
	  exit(EXIT_FAILURE);
	}

	// 'ReadoutMap' is a map of (readoutID, clusterKeys). The
	// cluster keys are the second element in this pair.
	auto& clusterKeys = (*search).second;

	// Let's look through the list of keys. A "cluster key" is a
	// triplet (std::tuple) of numbers: (trackID, hitID,
	// clusterID). The last two are arbitrary, and we're only
	// interested in the first value.
	for ( auto const& clusterKey : clusterKeys ) {
	  const auto [ trackID, hitID, clusterID ] = clusterKey;
	  const auto result = std::find(primaries.cbegin(), primaries.cend(), trackID );

	  // If the waveform came from a primary particle, print out
	  // the waveform and exit the loop (there's no point in
	  // printing the waveform for every matching cluster).
	  if ( result != primaries.cend() ) {
	    std::cout << waveform << std::endl;
	    break;
	  }
	} // for every cluster key
      } // for every waveform
    } // matched arbitraryEvent

  } // for every event in the combined trees

}
