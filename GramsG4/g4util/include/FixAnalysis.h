// A separate function to repair a ROOT file that was created by the
// G4AnalysisManager.
// 24-Oct-2023 WGS

// The G4AnalysisManager has a bug: The files it creates cannot be
// opened in UPDATE mode. This function Post-processes the file
// created by G4AnalysisManager to remove the bug.

// Note that this routine must not be invoked until after the file
// named in the argument is closed. Typically in a Geant4 simulation,
// this is after the G4RunManager has been deleted.

#ifndef FixAnalysis_h
#define FixAnalysis_h 1

#include <string>

namespace g4util {

  bool FixAnalysis( const std::string filename );

}

#endif // FixAnalysis_h
