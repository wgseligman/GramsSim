// RestructuredEdx.cc
// 22-Jul-2021 William Seligman <seligman@nevis.columbia.edu>

// This is an example of how to work with an ntuple that has columns
// that contain vectors. In particular, it shows how to append another
// column that is also a vector.

// As with all my other example programs, there are tons of comments
// for just a few lines of code.

// The program reads an ntuple that was created by
// HitRestructre.cc. It appends a column to that dataframe that
// contains the dE/dx for each hit.

// We're going to use the ROOT::RDataFrame class to read and write the ntuple.

// The program will automatically be compiled during the cmake/make
// process for GramsG4. However, it can be compiled stand-alone
// (assuming that ROOT has been set up) with:

// g++ <program-name>.cc `root-config --cflags --libs` -o <program-name>

// Define the ROOT includes.
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>

// The C++ includes.
#include <vector>
#include <iostream>
#include <cstring>
#include <cmath>

// Debug flag.
static const bool debug = false;

// Define a function to compute a physics quantity based on physics
// inputs. In this case, that's dE/dx.

// Note that, as a matter of style, I never use "using namespace
// std". That means I have to explicitly include the std:: namespace
// whenever I use something from it.

// Unfortunately, neither C++ nor ROOT includes a simple
// exponentiation operator; we have to use the "pow" (power) function.
// (Physics note: units are those of Geant4; i.e., distances are mm,
// energy is MeV, time is ns.)

double dEdx(double xStart,
	    double xEnd,
	    double yStart,
	    double yEnd,
	    double zStart,
	    double zEnd,
	    double energy) 
{
  double dx = std::sqrt(std::pow(xStart-xEnd,2) + 
			std::pow(yStart-yEnd,2) + 
			std::pow(zStart-zEnd,2));

  // If dx is close to zero, 1/dx is going to be huge or
  // undefined. Also note that testing a floating-point value for an
  // equality is not a good idea; it's best to express any numeric
  // floating-point tests solely in terms of < or >.
  static const double small = 1.e-20;
  if ( dx < small ) return 0.;

  // Calculate dE/dx.
  return energy/dx;
}

// ----====----====----====----====----====----====----====

// The previous function calculates a single value based on other
// values. However, we're working with an ntuple with columns that
// contain entire vectors, not just single values. Therefore, we need
// another function that accepts vectors as arguments and applies the
// above function to every element in the vector.

// Complication: When reading in vectors using RDataFrame, you have to
// use RVec. This will be fixed in a future version of ROOT (6.26+),
// but right now we're using ROOT 6.22. So our input "vectors" are
// RVec, but the output is a standard vector.

// Just to teach you something about the concept of "C++ scope", I'm
// using some of the same variable names that I used in the above
// physics calculation. The names may be the same, but these are
// _different_ objects.

// To save on typing, define the type for our input vectors. Note the
// use of "const", which means we promise not to change the contents
// of these vectors as we execute this function.
typedef const ROOT::VecOps::RVec<double> inVec_t;

std::vector<double> dEdxVector ( 
	inVec_t xStart,
	inVec_t xEnd,
	inVec_t yStart,
	inVec_t yEnd,
	inVec_t zStart,
	inVec_t zEnd,
	inVec_t energy) 
{
  // Note that we're assuming that all these vectors have the same
  // length. I know this is true because that's how I wrote
  // HitRestructure, but in general it might be a good idea to check.
  auto vectorSize = energy.size();

  // Prepare the vector which we're going to return to the calling
  // program. We already know exactly how big this output vector will
  // be.
  std::vector<double> dEdxResult( vectorSize );

  // Perform the calculation for each hit in the input vectors. 

  // I use "size_t" here to avoid warnings about comparing signed and
  // unsigned integers.
  for ( size_t i = 0; i != vectorSize; ++i ) {
    dEdxResult[i] = dEdx(
	 xStart[i],
	 xEnd[i],
	 yStart[i],
	 yEnd[i],
	 zStart[i],
	 zEnd[i],
	 energy[i] );
  }

  // Return the vector of calculated results.
  return dEdxResult;
}
 
// ----====----====----====----====----====----====----====

// Every C++ program must have a main routine.
int main( int argc, char** argv ) {

  // Determine the name of the input file.
  std::string filename("HitRestructure.root");
  if ( argc > 1 )
    filename = std::string( argv[1] );

  // Determine the name of the output file.
  std::string outputfile("RestructuredEdx.root");
  if ( argc > 2 )
    outputfile = std::string( argv[2] );

  // If we're not debugging...
  if ( ! debug ) {
    // Turn on multi-threaded processing, to (a) speed up the program, and
    // (b) prove we can write thread-safe code. Comment out this line to
    // revert back to standard single-thread execution.
    ROOT::EnableImplicitMT();
  }

  // The input dataframe. Note the name of the input ntuple is taken
  // from the name of the output ntuple in HitRestructure. (If I were
  // being cleverer, I'd make the name of the input ntuple one of the
  // arguments to this program, as I did with the filenames.)
  ROOT::RDataFrame inputNtuple( "comptonNtuple", filename );

  // Add a new column to the ntuple. This new column will contain a
  // vector of dE/dx values. Note that if I were adding multiple
  // columns, I'd need multiple physics calculation functions,
  // multiple functions to fiddle with the vectors, and multiple
  // .Define methods.

  // I'm doing something different with .Define than in my other
  // programs: Instead of a lambda function, instead I'm supplying the
  // name of a function that I've previously defined above. Due to the
  // way RDataFrame works, the arguments to that function will be
  // whatever columns I supply as a list as the third argument to
  // .Define.

  // All these names don't have to be the same (the names I used in
  // the physics calculation, the names I used in the vector
  // calculation, the names I assigned to the columns in
  // HitRestructure). However, I'm (a) lazy, and (b) know how to use
  // search-and-replace in my text editor.

  auto updatedNtuple = inputNtuple.Define("dEdx", dEdxVector, 
	   { "xStart",
	     "xEnd",
	     "yStart",
	     "yEnd",
	     "zStart",
	     "zEnd",
	     "energy" } );

  // After we've added all the columns that we're going to add, write
  // the modified ntuple to an output file. We can rename the ntuple
  // if we wish. Note that if multi-threading is turned on, the order
  // of the rows of in the output ntuple will be unpredictable.
  updatedNtuple.Snapshot("dEdxNtuple",outputfile);

} // end of 'main'
