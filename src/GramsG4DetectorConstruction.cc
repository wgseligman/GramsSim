
#include "GramsG4DetectorConstruction.hh"
#include "GramsG4ScintillatorSD.hh"
#include "GramsG4LArSensitiveDetector.hh"
#include "GramsG4LArSensitiveDetector.hh"
#include "Options.hh"

#include "G4Exception.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4GDMLParser.hh"
#include "G4String.hh"
#include "G4ios.hh"

#include <stdio.h>

GramsG4DetectorConstruction::GramsG4DetectorConstruction()
  : G4VUserDetectorConstruction()
{   
  auto options = util::Options::GetInstance();
  G4bool verbose;
  options->GetOption("verbose",verbose);

  // Uncomment the following if wish to avoid names stripping
  // fGDMLparser.SetStripFlag(false);
  
  // Fetch the GDML file with the detector description. 
  G4String gdmlFile;
  auto success = util::Options::GetInstance()->GetOption("gdmlfile",gdmlFile);
  if (success)
    fGDMLparser.Read(gdmlFile);
  else {
    G4ExceptionDescription description;
    description << "File " << __FILE__ << " Line " << __LINE__ << " " << std::endl
		<< "gdmlfile option not recognized ";
    G4Exception("GramsG4DetectorConstruction()","invalid option",
		FatalException, description);
  }

  //
  // Assign visibility and colors to volumes.
  //

  G4cout << G4endl;
   
  // For each G4LogicalVolume...
  G4LogicalVolumeStore* lvs = G4LogicalVolumeStore::GetInstance();
  for( auto lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++ )
    {
      G4GDMLAuxListType auxInfo = fGDMLparser.GetVolumeAuxiliaryInformation(*lvciter);
      
      // Are there any <auxiliary/> tags for this volume?
      if (auxInfo.size()>0)
	{
	  // For each <auxiliary/> tag found:
	  for (auto iaux = auxInfo.cbegin(); iaux != auxInfo.cend(); iaux++)
	    {
	      G4String str=iaux->type;
	      G4String val=iaux->value;
	      str.toLower();
	      if ( str == "color" || str == "colour" ) {
		val.toLower();
		if ( val == "none" ) {
		  (*lvciter)->SetVisAttributes(G4VisAttributes::GetInvisible());
		  if (verbose)
		    G4cout << "Set visiblity of '" 
			   << (*lvciter)->GetName() << "' to none" << G4endl;
		}
		else {
		  G4Colour color;
		  if ( G4Colour::GetColour(val, color) ) {
		    (*lvciter)->SetVisAttributes(G4VisAttributes(true, color));
		    if (verbose)
		      G4cout << "Set color of '" 
			     << (*lvciter)->GetName() 
			     << "' to " << val << G4endl;
		  }
		  else
		    G4cerr << "Color '" << val << "' not found" << G4endl;
		}
	      } // if aux type is color
	    } // for each aux tag
	} // if aux tags
    } // for each logical volume
  
  G4cout << std::endl;
}

G4VPhysicalVolume* GramsG4DetectorConstruction::Construct()
{
  return fGDMLparser.GetWorldVolume();
}

void GramsG4DetectorConstruction::ConstructSDandField()
{
  auto options = util::Options::GetInstance();

  // Define the sensitive detectors and their names that
  // are recognized by this simulation. 
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  SDman->AddNewDetector( new GramsG4ScintillatorSD("ScintillatorSD") );
  SDman->AddNewDetector( new GramsG4LArSensitiveDetector("LArSensitiveDetector") );

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
  // output file name. .
  G4String gdmlOutput;
  options->GetOption("gdmlout",gdmlOutput);
  if (! gdmlOutput.empty() ) {
    // For some reason (possibly to prevent overwriting the GDML input
    // file), G4GDMLParser will halt the program if gdmlOutput already
    // exists. Since that's annoying if you're making repeated runs,
    // delete any previous file with the same name.
    G4cout << "Deleting any previous " << gdmlOutput << G4endl;
    remove( gdmlOutput );

    G4cout << "Writing geometry to " << gdmlOutput << G4endl;
    fGDMLparser.SetRegionExport(true);
    fGDMLparser.Write(gdmlOutput, G4TransportationManager::GetTransportationManager()
		      ->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume());
  } // write gdml
}
