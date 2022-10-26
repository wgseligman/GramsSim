/// \file Analysis.h
/// \brief Selection of the analysis technology

/// This header has one purpose: To select the output format of the
/// Geant4 Analysis Manager. See Geant4 example B4c for more
/// information.

#ifndef g4util_Analysis_h
#define g4util_Analysis_h 1

// Accommodate different versions of Geant4. 
#include "G4Version.hh"

#if G4VERSION_NUMBER<1100
#include "g4root.hh"
//#include "g4csv.hh"
//#include "g4xml.hh"
#else
#include "G4RootAnalysisManager.hh"
using G4AnalysisManager = G4RootAnalysisManager;
// Note that one could replace "Root" with Generic, Csv, Hdf5, or Xml
#endif

#endif // g4util_Analysis_h
