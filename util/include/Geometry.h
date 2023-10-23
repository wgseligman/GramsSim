/// 20-Oct-2023 WGS 
/// Some handy routines for manipulating geometries.

/// See README.md for documentation.

#ifndef Geometry_h
#define Geometry_h 1

// For processing command-line and XML file options.
#include "Options.h" // in util/

#include <TGeoManager.h>
#include <TDirectory.h>
#include <memory>
#include <string>

namespace util {

  class Geometry 
  {
  public:
    
    /// This is a singleton class.
    /// According to <https://stackoverflow.com/questions/12248747/singleton-with-multithreads>
    /// this method is compatible with multi-threaded running. 
    static Geometry* GetInstance()
    {
      static Geometry instance;
      return &instance;
    }

    // Copy a TGeoManager structure from TDirectory "input" to
    // TDirectory "output" (almost certainly TFiles). If an explicit
    // name is not given, try getting the geometry name from the
    // options XML file. Return a pointer to the geometry we just
    // copied.
    std::shared_ptr<TGeoManager> CopyGeometry(const TDirectory* input, 
					      TDirectory* output, 
					      std::string geometry = "");

    // As we all know, Python users have a problem telling the
    // difference between an object and a pointer. So include other
    // signatures, just in case they pass an object to CopyGeometry
    // instead of the TDirectory pointer.
    std::shared_ptr<TGeoManager> CopyGeometry(const TDirectory* input, 
					      TDirectory output, 
					      const std::string geometry = "")
    {
      return this->CopyGeometry(input, &output, geometry);
    }

    std::shared_ptr<TGeoManager> CopyGeometry(const TDirectory input, 
					      TDirectory* output, 
					      const std::string geometry = "")
    {
      return this->CopyGeometry(&input, output, geometry);
    }

    std::shared_ptr<TGeoManager> CopyGeometry(const TDirectory input, 
					      TDirectory output, 
					      const std::string geometry = "")
    {
      return this->CopyGeometry(&input, &output, geometry);
    }

  protected:
    /// Standard null constructor for a singleton class.
    Geometry() {}
  };

} // namespace util

#endif // Geometry_h
