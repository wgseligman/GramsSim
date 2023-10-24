#include "Geometry.h"

// ROOT includes
#include "TDirectory.h"
#include "TFile.h"
#include "TKey.h"
#include "TGeoManager.h"

// C++ includes
#include <string>
#include <iostream>
#include <memory>

namespace util {

  std::shared_ptr<TGeoManager> Geometry::CopyGeometry(const TDirectory* a_input, 
						      TDirectory* a_output, 
						      const std::string a_geometry)
  {
    // Initialize the options from the XML file.
    auto options = util::Options::GetInstance();

    // Fetch the debug and verbose flags, just in case.
    bool debug;
    options->GetOption("debug",debug);
    bool verbose;
    options->GetOption("verbose",verbose);

    // Copy the input name for the geometry; the argument is const,
    // but we want to manipulate it. ,
    std::string geometry = a_geometry;

    // If it's blank, try look for the geometry name in the options
    // XML file.
    if ( geometry.empty() ) {
      options->GetOption("geometry",geometry);

      if ( geometry.empty() ) {
	// There's no "geometry" option defined in the XML file, so
	// there's nothing for us to copy.

	if (debug) 
	  std::cerr << "Geometry::CopyGeometry WARNING: "
		    <<" File " << __FILE__ << " Line " << __LINE__ << " " 
		    << "No geometry option found"
		    << std::endl;

	return std::shared_ptr<TGeoManager>();
      } // No geometry option found
    } // Called with empty geometry name

    if (debug) 
      std::cout << "Geometry::CopyGeometry: "
		<< "input directory is '" << a_input->GetName()
		<< "'" << std::endl;

    if (debug || verbose)
      gGeoManager->SetVerboseLevel(1);
    else
      gGeoManager->SetVerboseLevel(0); // avoid pointless Info messages

    // Search through all the items in the input file, looking for the first
    // object with the name in 'geometry'.
    TIter next(a_input->GetListOfKeys());
    TKey* key;

    // We have to get a bit fancy here, since HepMC3 .root files store
    // information in a form that doesn't inherit from TObject. This
    // means that the standard key->ReadObj may crash the program; use
    // ReadObjectAny instead.
    auto geoClass = TClass::GetClass("TGeoManager");
    
    // For each item in the file:
    while ( ( key = (TKey*) next() ) ) {
      // Check that the key is valid.
      if (key != nullptr) {

	// Does the key's name match the geometry name?
	if ( std::string( key->GetName() ) == geometry ) {

	  // Read in the item.
	  TGeoManager* geoManager = (TGeoManager*) key->ReadObjectAny(geoClass); 

	  // Is that item a TGeoManager?
	  if ( geoManager != nullptr ) {

	    if (debug) 
	      std::cout << "Geometry::CopyGeometry - found key '"
			<< key->GetName() << "' pointing to TGeoManager '"
			<< geoManager->GetName()
			<< "'" << std::endl;

	    // We found a TGeoManager in the input file whose name
	    // matches. Copy it to the output file.
	    
	    // We have to be careful when navigating ROOT
	    // directories. Save the directory that the calling
	    // routine is in.
	    auto directory = gDirectory->GetDirectory("");

	    if (debug) 
	      std::cout << "Geometry::CopyGeometry: "
			<< "Calling directory is '" << directory->GetName()
			<< "'" << std::endl;

	    // Go to the output file's directory.
	    a_output->cd();

	    if (debug) 
	      std::cout << "Geometry::CopyGeometry: "
			<< "output directory is '" << a_output->GetName()
			<< "'" << std::endl;
	    
	    // Write the TGeoManager to the output directory. ROOT
	    // methods can't accept std::string as input, so convert
	    // 'geometry' into a C-style text string.
	    geoManager->Write(geometry.c_str());

	    if (debug) 
	      std::cout << "Geometry::CopyGeometry: "
			<< "geometry written to '" << a_output->GetName()
			<< "'" << std::endl;
	    
	    // Move back to the original directory;
	    directory->cd();
	    
	    // Return the TGeoManager to the calling routine.
	    // We're finished; we don't have to keep scanning the input file.
	    return std::shared_ptr<TGeoManager>( geoManager );

	  } // key name matches 'geometry'
	} // it's an geoManager
      } // key exists
    } // while looking through items

    // If we get here, we've failed! There are no geoManagers whose name
    // matches 'geometry' in the ROOT file.
    if (debug) 
      std::cerr << "Geometry::CopyGeometry WARNING: "
		<<" File " << __FILE__ << " Line " << __LINE__ << " " 
		<< std::endl
		<< "No TGeoManager with the name '" << geometry
		<< "' found"
		<< std::endl;
    
    return std::shared_ptr<TGeoManager>();
  }


  // See the comments in GramsSim/util/include/Geometry.h

  bool Geometry::GDML2ROOT( const std::string a_gdmlFile,
			    const std::string a_rootFile,
			    const std::string a_geometry )
  {
    // Copy the input arguments, so that we don't try to overwrite
    // them.
    std::string gdmlFile = a_gdmlFile;
    std::string rootFile = a_rootFile;
    std::string geometry = a_geometry;

    // Get access to the parameters in the Options XML file.
    auto options = util::Options::GetInstance();

    // Standard user-supplied debug and verbosity flags.
    bool debug, verbose;
    options->GetOption("debug",debug);
    options->GetOption("verbose",verbose);

    // If no GDML file is given in the argument, try getting it from
    // the Options.
    if ( gdmlFile.empty() )
      options->GetOption("gdmlout",gdmlFile);

    // If we couldn't find it in the options (or the user set it to
    // empty), then return failure.
    if ( gdmlFile.empty() ) {
      if (debug || verbose)
	std::cerr << "Geometry::GDML2ROOT - no value for 'gdmlFile' parameter, "
		  << "or for 'gdmlout' parameter in Options; "
		  << "geometry not appended to ROOT file."
		  << std::endl;
      return false;
    }

    // We need a name for the ROOT file to which the TGeoManager
    // structure will be appended.
    if ( rootFile.empty() ) 
      options->GetOption("outputfile",rootFile);

    // If we couldn't find it in the options (or the user set it to
    // empty), then return failure.
    if ( rootFile.empty() ) {
      if (debug || verbose)
	std::cerr << "Geometry::GDML2ROOT - no value for 'rootFile' parameter, "
		  << "or for 'outputfile' parameter in Options; "
		  << "geometry not appended to ROOT file."
		  << std::endl;
      return false;
    }

    // We need a name for the TGeoManager structure that
    // will be written to the output file.
    if ( geometry.empty() ) 
      options->GetOption("geometry",geometry);

    // If we couldn't find it in the options (or the user set it to
    // empty), then return failure.
    if ( geometry.empty() ) {
      if (debug || verbose)
	std::cerr << "Geometry::GDML2ROOT - no value for 'geometry' parameter, "
		  << "or for 'geometry' parameter in Options; "
		  << "geometry not appended to ROOT file."
		  << std::endl;
      return false;
    }
    
    if (debug)
      std::cout << "Geometry::GDML2ROOT - Opening '"
	     << rootFile << "' in UPDATE mode"
	     << std::endl;
    
    // Note: ROOT routines can't accept std::string objects as
    // arguments. The strings have to be converted to C-style strigs
    // via the std::string::c_str() method.

    // Open the output file for appending a new ROOT object.
    TFile* outputFile = TFile::Open(rootFile.c_str(),"UPDATE");
    if ( outputFile == nullptr ) {
      if (debug || verbose)
	std::cerr << "Geometry::GDML2ROOT - could not open file '"
		  << rootFile << "' in 'UPDATE' mode; "
		  << "geometry not appended to ROOT file."
		  << std::endl;
      return false;
    }

    // gGeoManager, the global TGeoManager object, is defined
    // in TGeoManager.h
    auto geoManager = gGeoManager->Import(gdmlFile.c_str());
    if ( geoManager == nullptr ) {
      if (debug || verbose)
	std::cerr << "Geometry::GDML2ROOT - could not import GDML file '"
		  << gdmlFile << "'; "
		  << "geometry not appended to ROOT file."
		  << std::endl;
      return false;
    }
    
    if (debug || verbose) 
      std::cout << "Geometry::GDML2ROOT - "
		<< "Importing geometry from GDML file '" 
		<< gdmlFile << "' and appending TGeoManager structure '"
		<< geometry << "' to output file '"
		<< rootFile << "'"
		<< std::endl;
    
    // Append the geometry to the output file.
    geoManager->Write(geometry.c_str());

    // Clean up.
    outputFile->Write();
    outputFile->Close();

    return true;
  }

} // namespace util
