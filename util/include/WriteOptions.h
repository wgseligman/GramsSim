// 26-Nov-2021 WGS 
// Write out the options to an ntuple.

// This utility class write out the contents of the internal Options
// information to an ntuple. The idea is that this ntuple can be
// subsequently inspected to see what options were used to generate a
// particular file.

#ifndef WriteOptions_h
#define WriteOptions_h

#include "TDirectory.h" // ROOT include
#include <string>       // C++ include

namespace util {

  class WriteOptions 
  {
  public:

    // Strictly speaking, this structure doesn't have to be a class at
    // all. However, things have a way of getting extended, so let's
    // set this up as a class rather than a simple function.
    WriteOptions() {}
    virtual ~WriteOptions() {}

    // The argument to this method is the output directory for the
    // ntuple. Presumably this will be a ROOT file.
    bool WriteNtuple(TDirectory* output, std::string outputNtuple = "Options");
  };

} // namespace util

#endif // WriteOptions_h
