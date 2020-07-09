//
/// \file GramsG4/include/GramsG4DetectorConstruction.hh
/// \brief Definition of the GramsG4DetectorConstruction class

#ifndef _GramsG4DETECTORCONSTRUCTION_H_
#define _GramsG4DETECTORCONSTRUCTION_H_

#include "G4VUserDetectorConstruction.hh"
#include "G4GDMLParser.hh"

/// Detector construction using the geometry read from the GDML file

namespace gramsg4 {

  class DetectorConstruction : public G4VUserDetectorConstruction
  {
  public:
    // Constructor
    DetectorConstruction();
    // Return a pointer to the world volume.
    virtual G4VPhysicalVolume* Construct();
    // Define the sensitive detectors.
    void ConstructSDandField();

  private:
    // The contents of the detector GDML file. 
    G4GDMLParser fGDMLparser;
  };

} // namespace gramsg4

#endif // _GramsG4DETECTORCONSTRUCTION_H_
