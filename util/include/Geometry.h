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


    // This is a utility function to take a file in GDML format,
    // convert it to ROOT's TGeoManager format, and append the
    // geometry to a ROOT file.

    // The arguments are:

    // gdmlFile = The name of the input geometry file in GDML
    // format. Note that this file must either have been written by
    // Geant4's G4GDMLParser, or it must be fully ROOT-compatiable (no
    // formulas or loops). If this is empty, this value will be taken
    // from the 'gdmlout' parameter in Options.

    // rootFile = The name of the output ROOT file into which the
    // TGeoManager structure will be written. If this is empty, this
    // will be taken from the 'outputfile' parameter in Options.

    // geometry = The name of the TGeoManager structure when it is
    // written to 'rootFile'. If this is empty, this will be taken
    // from the 'geometry' parameter in Options.

    // Note that both gdmlFile and rootFile should be closed before
    // invoking this routine, as GDML2ROOT will open these files; in
    // particular, rootFile will be opened in "UPDATE" mode.

    // This routine will probably only ever be called from a Geant4
    // simulation after all files are closed anyway, near the end of
    // the 'main' routine. It's primarily within this class just in
    // case there is another use-case for it.

    bool GDML2ROOT( std::string gdmlFile = "",
		    std::string rootFile = "",
		    std::string geometry = "" );

  protected:
    /// Standard null constructor for a singleton class.
    Geometry() {}
  };

} // namespace util

#endif // Geometry_h
