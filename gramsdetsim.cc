// gramsdetsim.cc
// Take the hit output from GramsG4 and apply detector-response effects.
// 25-Nov-2011 William Seligman

// For processing command-line and XML file options.
#include "Options.h" // in util/

// C++ includes
#include <exception>
#include <iostream>

int main(int argc,char **argv)
{
  // Initialize the options from the XML file and the
  // command line. Make sure this happens first!
  auto options = util::Options::GetInstance();

  // The third argument of ParseOptions is the name of the tag to use
  // for this program's options. See options.xml and/or util/README.md
  // to see how this works.
  auto result = options->ParseOptions(argc, argv, "gramsdetsim");

  // Abort if we couldn't parse the job options.
  if (result) std::cout << "ParseOptions succeeded" << std::endl;
  else {
    std::cerr << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
	      << "GramsDetSim: Aborting job due to failure to parse options"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // We may not be debugging this at this moment, but if we
  // want to use the debug flag later in this routine,
  // let's have it ready.
  bool debug;
  options->GetOption("debug",debug);

  // Check for help message.
  bool help;
  options->GetOption("help",help);
  if (help) {
    options->PrintHelp();
    exit(EXIT_SUCCESS);
  }

}
