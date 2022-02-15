
#include "GramsG4DetectorConstruction.hh"
#include "GramsG4ScintillatorSD.hh"
#include "GramsG4LArSensitiveDetector.hh"
#include "Options.h"

#include "G4Version.hh"
#include "G4Exception.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4GDMLParser.hh"
#include "G4String.hh"
#include "G4ios.hh"

//#include <cstdio> // for std::remove
//#include <sys/stat.h> // for stat()
//#include <memory> // std::unique_ptr()

namespace gramsg4 {

  DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction()
  {   
    auto options = util::Options::GetInstance();
    G4bool verbose;
    options->GetOption("verbose",verbose);

    // Uncomment the following if wish to avoid names stripping
    // fGDMLparser.SetStripFlag(false);
  
    // Fetch the GDML file with the detector description. 
    G4String gdmlFile;
    auto success = options->GetOption("gdmlfile",gdmlFile);
    if (success)
      fGDMLparser.Read(gdmlFile);
    else {
      G4ExceptionDescription description;
      description << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		  << "gdmlfile option not recognized ";
      G4Exception("gramsg4::DetectorConstruction()","invalid option",
		  FatalException, description);
    }

    //
    // Assign properties (like visibility and colors) to volumes.
    //

    // Set up any special conditions for LAr TPC volumes. For now,
    // that's just the step size within the LAr TPC.
    G4double larTPCStepSize(0.);
    bool haveTPCStepSize = options->GetOption("larstepsize",larTPCStepSize);
    auto stepLimit( new G4UserLimits(larTPCStepSize)) ;

    G4cout << G4endl;
   
    // For each G4LogicalVolume...
    G4LogicalVolumeStore* lvs = G4LogicalVolumeStore::GetInstance();
    for( auto lviter = lvs->begin(); lviter != lvs->end(); lviter++ )
      {
	auto logVol = *lviter;

	G4GDMLAuxListType auxInfo = fGDMLparser.GetVolumeAuxiliaryInformation(logVol);
      
	// Are there any <auxiliary/> tags for this volume?
	if (auxInfo.size()>0)
	  {
	    // For each <auxiliary/> tag found:
	    for (auto iaux = auxInfo.cbegin(); iaux != auxInfo.cend(); iaux++)
	      {
		G4String str=iaux->type;
		G4String val=iaux->value;
		str.toLower();

		// Check for any step limits.
		if ( str == "steplimit" ) {
		  try {
		    double stepSize = std::stod(val);
		    auto volStepLimit( new G4UserLimits(stepSize) );
		    logVol->SetUserLimits(volStepLimit);
		    if (verbose)
		      G4cout << "Set maximum step size of '" 
			     << logVol->GetName() << "' to " 
			     << stepSize << G4endl;
		  } catch ( std::invalid_argument& e ) {
		    G4ExceptionDescription description;
		    description << "File " << __FILE__ << " Line " << __LINE__ 
				<< " " << G4endl
				<< "could not convert '" << val
				<< "' to a number" << G4endl
				<< "Step limit of volume '"
				<< logVol->GetName() << "' unchanged";
		    G4Exception("gramsg4::DetectorConstruction()","invalid value",
				JustWarning, description);
		  }
		}

		// Check for any visibility changes.
		if ( str == "color" || str == "colour" ) {
		  val.toLower();
		  if ( val == "none" ) {
		    logVol->SetVisAttributes(G4VisAttributes::GetInvisible());
		    if (verbose)
		      G4cout << "Set visiblity of '" 
			     << logVol->GetName() << "' to none" << G4endl;
		  }
		  else {
		    G4Colour color;
		    if ( G4Colour::GetColour(val, color) ) {
		      logVol->SetVisAttributes(G4VisAttributes(true, color));
		      if (verbose)
			G4cout << "Set color of '" 
			       << logVol->GetName() 
			       << "' to " << val << G4endl;
		    }
		    else
		      G4cerr << "Color '" << val << "' not found" << G4endl;
		  }
		} // if aux type is color
	      } // for each aux tag
	  } // if aux tags

	// Is this the active LAr TPC volume? And is there an option
	// in the XML file that specifies the step limit for the LAr
	// TPC? If so, override any step limits that came from the
	// GDML file. 
	if ( haveTPCStepSize  &&  logVol->GetName() == "volTPCActive" ) {
	  // Get any existing G4UserLimits stored in this volume 
	  // and delete it. 
	  auto oldLimits = logVol->GetUserLimits();
	  delete oldLimits;
	  // Attach the step limit from the options XML file. 
	  logVol->SetUserLimits(stepLimit);
	  if (verbose)
	    G4cout << "Override: Set maximum step size of '" 
		   << logVol->GetName() << "' to " << larTPCStepSize << G4endl;
	}

      } // for each logical volume
  
    G4cout << std::endl;
  }

  G4VPhysicalVolume* DetectorConstruction::Construct()
  {
    return fGDMLparser.GetWorldVolume();
  }

  void DetectorConstruction::ConstructSDandField()
  {
    auto options = util::Options::GetInstance();

    // Define the sensitive detectors and their names that
    // are recognized by this simulation. 
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    // Make sure the hit collection name (the second argument) agrees
    // with the name in GramsG4WriteHitsAction.cc and the 
    // auxiliary tag for the volume in the GDML file. .
    SDman->AddNewDetector( new ScintillatorSD("ScintillatorSD","ScintillatorHits") );
    SDman->AddNewDetector( new LArSensitiveDetector("LArSensitiveDetector","LArHits") );

    // Use the <auxiliary/> tages in the GDML file to assign sensitive
    // detectors to volumes.

    // For each volume with an <auxiliary/> tag:
    const G4GDMLAuxMapType* auxmap = fGDMLparser.GetAuxMap();
    for (auto lvinter = auxmap->cbegin(); lvinter!=auxmap->cend(); lvinter++) 
      {
	// For each <auxiliary/> tag for this volume.
	for ( auto auxiter = (*lvinter).second.cbegin();
	      auxiter != (*lvinter).second.cend(); auxiter++)
	  {
	    if ((*auxiter).type == "SensDet")
	      {
		G4cout << "Attaching sensitive detector " << (*auxiter).value
		       << " to volume " << ((*lvinter).first)->GetName()
		       <<  G4endl;

		G4VSensitiveDetector* mydet = 
		  SDman->FindSensitiveDetector((*auxiter).value);
		if(mydet) 
		  {
		    G4LogicalVolume* myvol = (*lvinter).first;
		    myvol->SetSensitiveDetector(mydet);
		  }
		else
		  {
		    G4cout << "Sensitive detector name '" 
			   << (*auxiter).value << "' not found" << G4endl;
		  }
	      } // if type == "SensDet"
	  } // for each <auxiliary/> tab
      } // for each volume 

    // Write out the parsed GDML geometry if the user supplied a GDML
    // output file name.
    G4String gdmlOutput;
    options->GetOption("gdmlout",gdmlOutput);
    if ( ! gdmlOutput.empty() ) {
      G4cout << "Writing geometry to '" << gdmlOutput << "'" << G4endl;
#if G4VERSION_NUMBER>=1070
      // Added in Geant4.10.7: If the GDML output file already exists,
      // overwrite it. 
      fGDMLparser.SetOutputFileOverwrite(true);
#endif
      fGDMLparser.Write(gdmlOutput, fGDMLparser.GetWorldVolume());
    } // write gdml
  }

} // namespace gramsg4
