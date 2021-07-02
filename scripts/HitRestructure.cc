// HitRestructure.cc
// 01-Jul-2021 William Seligman <seligman@nevis.columbia.edu>

// This is an example of how to restructure the LArHits ntuple within
// a GramsG4 output file.

// In the LArHits ntuple, each hit is written to its own row. This
// program restructures those hits into an ntuple with only one row
// for each run/event/trackID combination, with the hit information
// stored in vectors in that row.

// Note that does NOT exactly match the ntuple proposal circulated to
// the GRAMS analyzers. I have to leave some work for the students to
// do!

// The initial part of this program is similar to another program in
// this directory, dEdxExample.cc. That routine is filled with
// comments. To save your sanity, I've omitted the comments here. If
// you have questions about the following code, check dEdxExample; the
// answer is probably there.

// The program will automatically be compiled during the cmake/make
// process for GramsG4. However, it can be compiled stand-alone
// (assuming that ROOT has been set up) with:

// g++ <program-name>.cc `root-config --cflags --libs` -o <program-name>

// Define the ROOT includes.
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TFile.h>
#include <TTree.h>

// The C++ includes.
#include <vector>
#include <map>
#include <cstring>
#include <iostream>

// Debug flag.
static const bool debug = false;

// Every C++ program must have a main routine.
int main( int argc, char** argv ) {

  // Determine the input file.
  std::string filename("gramsg4.root");
  if ( argc > 1 )
    filename = std::string( argv[1] );

  // Determine the name of the output file.
  std::string outputfile("HitRestructure.root");
  if ( argc > 2 )
    outputfile = std::string( argv[2] );

  // Note: For the LArHits processing, we _cannot_ use multiple
  // threads.

  // A container for track information. 
  typedef std::map< std::tuple< int, int, int >, std::string > trackMap_t;
  trackMap_t trackMap;

  // Read the TrackInfo ntuple.
  ROOT::RDataFrame trackInfo( "TrackInfo", filename, 
			     {"Run", "Event", "TrackID", "ProcessName"} );

  // Copy TrackInfo's information into trackMap. (Since we don't have
  // multiple threads, we're using Foreach instead of ForeachSlot.)
  trackInfo.Foreach(
	[&trackMap]( int run, 
		     int event, 
		     int trackID, 
		     ROOT::VecOps::RVec<char> process)
	{
	  std::string processName;
	  for ( auto i = process.begin(); i != process.end() && (*i) != 0; ++i )
	    processName.push_back(*i);
	  trackMap[{run,event,trackID}] = processName;
	}, 
	{"Run", "Event", "TrackID", "ProcessName"} 
			);

  if (debug) {
    for ( auto i = trackMap.begin(); i != trackMap.end(); ++i ){
      auto key = (*i).first;
      auto value = (*i).second;
      std::cout << " run=" << std::get<0>(key)
		<< " event=" << std::get<1>(key) 
		<< " trackID=" << std::get<2>(key)
		<< " process=" << value << std::endl;
    } // loop over trackMap
  } // if debug

  // Go through LArHits and select those rows that associated with
  // Compton scattering.
  ROOT::RDataFrame larHits( "LArHits", filename );
  auto comptonHits = larHits.Filter(
    [&trackMap](int run, int event, int trackID)
      {
	return trackMap[{run, event, trackID}] == "compt";
      },
    { "Run", "Event", "TrackID" }
			 );

  // At this point we deviate from the procedure in dEdxExample. We're
  // going to read in all the hit information in the comptonHits
  // dataframe and put it into a map, appending to vectors as we go.

  // Note that is procedure is very memory inefficient; we're
  // basically reading in the entire hits ntuple (at least, those hits
  // that are part of Compton scatters).

  // The vectors that will be written to the ntuple, one row per
  // run/event/trackID.
  typedef struct hitVectors 
  {
    std::vector<int>    PDGCode;
    std::vector<int>    numPhotons;
    std::vector<double> energy;
    std::vector<double> tStart;
    std::vector<double> xStart;
    std::vector<double> yStart;
    std::vector<double> zStart;
    std::vector<double> tEnd;
    std::vector<double> xEnd;
    std::vector<double> yEnd;
    std::vector<double> zEnd;
    std::vector<int>    identifier;
  } hitInfo;

  std::map< std::tuple< int, int, int >, hitInfo > hitMap;

  // Go through the Compton-induced hits and include them in the
  // hitMap. (Yes, this is a big lambda expression, but the
  // alternative is to define a function at the top of this code.)
  comptonHits.Foreach(
      [&hitMap]( int run,
		 int event,
		 int trackID,
		 int    PDGCode,
		 int    numPhotons,
		 double energy,
		 double tStart,
		 double xStart,
		 double yStart,
		 double zStart,
		 double tEnd,
		 double xEnd,
		 double yEnd,
		 double zEnd,
		 int    identifier)
      {
	// To save a bit on typing, copy the address of the structure
	// pointed to by this run/event/trackID.
	auto &hit = hitMap[{run, event, trackID}];

	// Append all the information in this row of the ntuple to the
	// hitInfo vectors.
	hit.PDGCode.push_back( PDGCode );
	hit.numPhotons.push_back( numPhotons );
	hit.energy.push_back( energy );
	hit.tStart.push_back( tStart );
	hit.xStart.push_back( xStart );
	hit.yStart.push_back( yStart );
	hit.zStart.push_back( zStart );
	hit.tEnd.push_back( tEnd );
	hit.xEnd.push_back( xEnd );
	hit.yEnd.push_back( yEnd );
	hit.zEnd.push_back( zEnd );
	hit.identifier.push_back( identifier);
      },
      // The list of all the columns in the ntuple. In theory we don't
      // have to supply this, since we're looking all the
      // columns. However, what if someone switches the columns
      // around?)
      { "Run", 
        "Event", 
	"TrackID", 
	"PDGCode",
	"numPhotons",
	"energy",
	"tStart",
	"xStart",
	"yStart",
	"zStart",
	"tEnd",
	"xEnd",
	"yEnd",
	"zEnd",
	"identifier"}
		      );
	       
  // At this point, all the information in the comptonHits dataframe
  // have been loaded into hitMap (probably several megabytes worth).

  // Create the new output ntuple. This time we have to do it the
  // old-fashioned way; dataframes aren't really suited for this.
  TFile* output = new TFile(outputfile.c_str(), "RECREATE");
  TTree* ntuple = new TTree("comptonNtuple","Energy deposited in compton scattering");

  // Define the variables to be access when writing the branches.
  int Run;
  int Event;
  int TrackID;
  std::vector<int> PDGCode;
  std::vector<int> numPhotons;
  std::vector<double> energy;
  std::vector<double> tStart;
  std::vector<double> xStart;
  std::vector<double> yStart;
  std::vector<double> zStart;
  std::vector<double> tEnd;
  std::vector<double> xEnd;
  std::vector<double> yEnd;
  std::vector<double> zEnd;
  std::vector<int> identifier;

  // In an n-tuple, we assign each variable to its own branch.
  ntuple->Branch("Run", &Run, "Run/I");
  ntuple->Branch("Event", &Event, "Event/I");
  ntuple->Branch("TrackID", &TrackID, "TrackID/I");
  ntuple->Branch("PDGCode", &PDGCode);
  ntuple->Branch("numPhotons", &numPhotons);
  ntuple->Branch("energy", &energy);
  ntuple->Branch("tStart", &tStart);
  ntuple->Branch("xStart", &xStart);
  ntuple->Branch("yStart", &yStart);
  ntuple->Branch("zStart", &zStart);
  ntuple->Branch("tEnd", &tEnd);
  ntuple->Branch("xEnd", &xEnd);
  ntuple->Branch("yEnd", &yEnd);
  ntuple->Branch("zEnd", &zEnd);
  ntuple->Branch("identifier", &identifier);

  // Loop over the map, writing each entry to the output ntuple.
  for ( auto i = hitMap.begin(); i != hitMap.end(); ++i ) 
    {
      auto key = (*i).first;
      Run = std::get<0>(key);
      Event = std::get<1>(key);
      TrackID = std::get<2>(key);

      auto &vectors = (*i).second;
      PDGCode = vectors.PDGCode;
      numPhotons = vectors.numPhotons;
      energy = vectors.energy;
      tStart = vectors.tStart;
      xStart = vectors.xStart;
      yStart = vectors.yStart;
      zStart = vectors.zStart;
      tEnd = vectors.tEnd;
      xEnd = vectors.xEnd;
      yEnd = vectors.yEnd;
      zEnd = vectors.zEnd;
      identifier = vectors.identifier;

      ntuple->Fill();
    }


  // Wrap things up.
  ntuple->Write();
  output->Close();

} // end of 'main'
