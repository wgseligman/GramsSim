// dEdxExample.cc
// 30-Jun-2021 William Seligman <seligman@nevis.columbia.edu>

// This is an overly-commented example program to illustrate
// techniques that can be used to read and analyze the output of
// GramsG4.

// To be fancy, I'm going to use RDataFrame to read the input
// ntuples. This allows for the program's execution speed to be
// improved if we use multiple execution threads. This necessitates a
// bit of compromise, in that faster execution means sacrificing both
// memory use and program clarity.

// To be even fancier, I'm going to use features of the C++ Standard
// Template Library (STL). This seems confusing at first, especially
// if you haven't worked with pointers before. However, pointers are
// fundamental concepts in both C++ and STL; if I didn't each you
// about them here, you'd have to learn them off the street anyway.

// Don't worry; I'll walk you through each step.

// The program will automatically be compiled during the cmake/make
// process for GramsG4. However, it can be compiled stand-alone
// (assuming that ROOT has been set up) with:

// g++ <program-name>.cc `root-config --cflags --libs` -o <program-name>

// Define the ROOT includes. It's typically necessary to #include the
// header file for each ROOT class used in a program.
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

// The C++ includes. These are for C++ language features that,
// historically, were not part of the C++ base language.
#include <vector>
#include <map>
#include <cstring>
#include <iostream>

// Debug flag.
static const bool debug = false;

// Every C++ program must have a main routine.
int main( int argc, char** argv ) {

  // Determine the input file. The default is "gramsg4.root". However,
  // if the user supplied an argument on the command line, use that
  // name instead. Note that argc is the number of arguments on the
  // command line (including the name of the program) and argv is an
  // array of C strings (char*) containing those arguments.
  std::string filename("gramsg4.root");
  if ( argc > 1 )
    filename = std::string( argv[1] );

  // Determine the name of the output file.
  std::string outputfile("dEdxExample.root");
  if ( argc > 2 )
    outputfile = std::string( argv[2] );

  unsigned int poolSize = 1;
  // If we're not debugging...
  if ( ! debug ) {
    // Turn on multi-threaded processing, to (a) speed up the program, and
    // (b) prove we can write thread-safe code. Comment out this line to
    // revert back to standard single-thread execution.
    ROOT::EnableImplicitMT();

    // We'll need this later: Now that we've turned on multi-threading,
    // how many threads is this program using?
    poolSize = ROOT::GetThreadPoolSize();
  }

  // In C++ you can define your own types. Here we define a new type,
  // based on a map; do a web search on "STL map" to learn more about
  // what a map is. For the purposes of this program, a map is similar
  // to an python dictionary, in that it contains key:value pairs.

  // This map will use as a key a 'tuple' (a C++ construct with a
  // fixed number of values) consisting of three numbers: run, event,
  // and track ID. It will map that tuple to a string, which will
  // contain the process name.
  
  // Example: trackMap[{0,3,5}] == "compt" means that run 0, event 3,
  // trackID 5 has a process name of "compt".
  
  // Here we're defining the new type, which I'm calling trackMap_t. We
  // haven't defined an actual trackMap object yet.
  
  typedef std::map< std::tuple< int, int, int >, std::string > trackMap_t;

  // Now we can define the trackMap object. This will contain a subset
  // of the information from the TrackInfo ntuple within our input
  // file.
  trackMap_t trackMap;

  // But that's not quite enough. We know we're going to read the
  // TrackInfo ntuple using RDataFrame with multi-threaded
  // execution. To make sure the code is thread-safe, we'll want each
  // thread to have its own "trackMap_t" object so that different
  // threads don't try to modify the same trackMap at once. Therefore,
  // create a vector of trackMap_t objects, one for each execution
  // thread.
  std::vector< trackMap_t > trackMapThread( poolSize );

  // Define a dataframe containing information from the TrackInfo
  // ntuple within that file. I'm saving a bit of memory and execution
  // speed by only reading those columns (branches) in the ntuple that
  // I plan to use in this particular program. To read in all the
  // columns, just omit the list in the curly brackets.
  ROOT::RDataFrame trackInfo( "TrackInfo", filename, 
			     {"Run", "Event", "TrackID", "ProcessName"} );

  // Now we're going to get information from the trackInfo dataframe
  // and store it in our track maps. The following code looks like a
  // mass of braces {} [], so let's break it down.

  // ForeachSlot is one of the actions that can be applied to an
  // RDataFrame (check the RDataFrame description on the ROOT web
  // site). It applies a function to every row in the dataframe.

  // That function has to be carefully structured: its first argument
  // is an unsigned int, the "slot number"; that is, in a
  // multi-threaded program it will be the number of the thread
  // calling the function. This enables us to write thread-safe code
  // that won't cause problems if two different threads try to use the
  // same area of memory at the same time.

  // The remaining arguments to that function have to correspond to
  // the list of columns in the second argument to ForeachSlot. Let's
  // look at that second argument:

  // {"Run", "Event", "TrackID", "ProcessName"}

  // This is a list of columns in the TrackInfo ntuple. It corresponds
  // to the list of columns in the previous C++ statement, because (as
  // it happens) those are exactly the columns for doing the work I
  // want to do. (Note that in other programs, the lists of columns in
  // these two statements may not match. However, the list in the
  // following statement must be a subset of those in the dataframe
  // definition.)

  // So this carefully-structured function, after the slot argument,
  // has to have one argument for each column in that list. Just for a
  // second, let's pretend that function's name is "f". We'd therefore
  // define something like this (I explain RVec below):

  // void f( unsigned int slot, int run, int event, int trackID, RVec<char> process );

  // While the arguments have to match in number and type, the names
  // don't have to match; I could have written:

  // void f( unsigned int s, int r, int e, int t, RVec<char> p );

  // But there is no explicit "f". When working with dataframes, it's
  // common to define single-use functions that will not be used
  // again. The standard is to define a "lambda expression", an
  // anonymous short function (a web search on "C++ lambda expression"
  // may given a better explanation).

  // A typical lambda expression in C++ looks like:

  // [](<function arguments>){ <function body> }

  // Lambda expressions aren't usually put on a line by themselves,
  // but are supplied as arguments to functions.

  // If you put a variable in the [], it means to "capture" that
  // variable from the surrounding program. Here, we're capturing the
  // _address_ of trackMapThread (the & operator):
  // [&trackMapThread]. If I had put [trackMapThread] (without the &)
  // the entire trackMapThread structure would be copied for every row
  // in the dataframe instead of just the address.

  // There's one more complication: One of the columns in the ntuple
  // is actually a vector (array). It's ProcessName, which in C++ is a
  // vector of char. To read a vector with RDataFrame, you have to use
  // a special class called ROOT::VecOps::RVec<T>, where T is the type
  // of data stored in the array.

  // Therefore, we have to read in ProcessName as an RVec, and convert
  // it into a string as you see below. It's tedious and involves some
  // STL algorithms that, honestly, I had to look up on the web.

  // (Bear in mind that a solution like this will also be necessary if
  // a column in an ntuple contains a vector of numbers, like the
  // trajectory information in TrackInfo.)

  // Enough talk, let's do it:

  trackInfo.ForeachSlot(
	// The lambda expression: capture the address of
        // trackMapThread, and accept a set of arguments beginning
        // with the slot number. This is first argument to
        // ForeachSlot.
	[&trackMapThread](unsigned int slot, 
			  int run, 
			  int event, 
			  int trackID, 
			  ROOT::VecOps::RVec<char> process)
	{
	  // Copy the RVec into a regular string, one character at a
	  // time. (I explain begin() and end() below; 'push_back'
	  // means "add this to the end of the vector".)
	  std::string processName;
	  for ( auto i = process.begin(); i != process.end() && (*i) != 0; ++i )
	    processName.push_back(*i);

	  // In the trackMap corresponding to this slot, and with the
	  // key run/event/trackID, save the name of the process that
	  // created the track.
	  trackMapThread[slot][{run,event,trackID}] = processName;
	}, 

	// The second argument to ForeachSlot: the list of columns to
	// be passed to the function in the first argument.
	{"Run", "Event", "TrackID", "ProcessName"} 
			);

  // We now have a bunch of trackMap_t objects in trackMapThread that we
  // want to merge together. 
  
  // I'm also being fancy with "iterators" here. I could iterate over
  // trackMapThread[0], trackMapThread[1]... and so on. However, when
  // working with STL containers like vectors and maps, it's common to
  // use iterators, which are a fancy form of pointer.
  
  // Note the use of begin() and end(). When used with an STL
  // container, begin() returns an iterator that points to the
  // beginning of the container; end() returns _one step beyond_ the
  // end of the container, and adding 1 to an iterator (here that's
  // ++i) moves a pointer to next position in the container.
  
  // For each map in trackMapThread:
  for ( auto i = trackMapThread.begin(); i != trackMapThread.end(); ++i )
    {
      // Insert the map for that particular thread into our main
      // map. Remember that if a pointer (or iterator) points to
      // something, putting "*" in front of the pointer gives you that
      // something. So here, (*i) gives a single trackMap_t object
      // within trackMapThread. "insert" is a method provided by STL
      // to include one map inside another.
      trackMap.insert( (*i).begin(), (*i).end() );

      // In an attempt to save memory, erase the map that we'll no
      // longer need.
      (*i).clear();
    }

  if (debug) {
    // Display the contents of trackMap. Recall that maps are a set of
    // (key,value) pairs, so (*i).first means the key and (*i).second
    // is the value.
    for ( auto i = trackMap.begin(); i != trackMap.end(); ++i ){
      auto key = (*i).first;
      auto value = (*i).second;
      // std::get is one way to get a particular value from a std::tuple.
      std::cout << " run=" << std::get<0>(key)
		<< " event=" << std::get<1>(key) 
		<< " trackID=" << std::get<2>(key)
		<< " process=" << value << std::endl;
    } // loop over trackMap
  } // if debug

  // Despite the barrage of comments, so far this routine consists of
  // only a dozen-or-so lines of code. At this point we have a
  // structure, trackMap, that contains a subset of information from
  // the TrackInfo ntuple.

  // Now comes the real fun: using that track information against the
  // LArHits ntuple. Create a new dataframe for that ntuple.
  ROOT::RDataFrame larHits( "LArHits", filename );

  // We use trackMap to look up the run/event/trackID for a given LAr
  // Hit, and (in this example) use it to select only electrons
  // produced by Compton scattering. Note that again we're using a
  // lambda expression as an argument to Filter. In this case, the
  // lambda function is returning a boolean (as required of a filter).
  auto comptonHits = larHits.Filter(
    // First argument to Filter: a lambda expression
    [&trackMap](int run, int event, int trackID)
      {
	return trackMap[{run, event, trackID}] == "compt";
      },
    // Second argument: The list of columns in LArHits to be passed to
    // the lambda expression.
    { "Run", "Event", "TrackID" }
			 );

  // Now that we have our Compton-induced hits, let's do something
  // with them. Since the name of this program is dEdxExample, let's
  // compute dE/dx. 

  // We don't have to bother with lambda expressions here; we're not
  // modifying or accessing anything outside the scope of the
  // variables available in the dataframe.

  // Unfortunately, neither C++ nor ROOT includes a simple
  // expontiation operator; we have to use the "pow" (power) function.
  // (Physics note: units are those of Geant4; i.e., distances are mm,
  // energy is MeV, time is ns.)
  auto dEdx = comptonHits
    .Define("dx","sqrt(pow(xStart-xEnd,2) + pow(yStart-yEnd,2) + pow(zStart-zEnd,2))")
    .Define("dEdx","energy / dx");

  if (debug) {
    // Print out some of the columns for the first 100 hits.
    auto display = dEdx.Display({"Run","Event","TrackID","tStart","dx","dEdx"},100);
    display->Print();
  }

  // Write the modified ntuple to an output file. We can rename the
  // ntuple if we wish. Note that if multi-threading is turned on, the
  // order of the rows of in the output ntuple will be unpredictable.
  dEdx.Snapshot("dEdxNtuple",outputfile);

} // end of 'main'
