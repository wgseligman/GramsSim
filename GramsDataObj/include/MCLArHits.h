/// \file MCLArHits.h
/// \brief Data object that contains a list of energy deposits in a LAr TPC
// 31-May-2024 WGS

#ifndef _grams_mclarhits_h_
#define _grams_mclarhits_h_

#include <Math/Vector4D.h>

#include <iostream>
#include <map>

namespace grams {

  // Define an energy deposit (a "hit" in MC terminology) in in the
  // LAr TPC. This is effectively a record of a "step" within
  // LAr. 

  struct MCLArHit {

    // A pointer back to the MCTrack that created the step.
    int trackID;

    // For conveniece, the PDG code of the particle that created the
    // step. (This information is also in the MCTrack, but omitting
    // this variable requires the users to do a lookup in
    // MCTrackList.)
    int pdgCode;

    // Number of scintillations photons. 
    int numPhotons;

    // Number of Cerenkov photons.
    int cerPhotons;

    // Volume identifier for the step.
    int volumeID;

    // The ionization energy deposited in the step.
    double energy;

    // The (x,y,z,t) at the start and end of the step. 
    ROOT::Math::XYZTVector start;
    ROOT::Math::XYZTVector end;

    // Provide accessors to avoid confusion between a C++ struct
    // and a C++ class.
    int TrackID() const { return trackID; }
    int PDGCode() const { return pdgCode; }
    int NumPhotons() const { return numPhotons; }
    int CerPhotons() const { return cerPhotons; }
    int Identifier() const { return volumeID; }
    int E() const { return energy; }
    int Energy() const { return energy; } // an extra accessor can't hurt

    // For users who prefer to work with ROOT's 4D vectors, and for
    // those who don't:
    const ROOT::Math::XYZTVector Start4D() const { return start; }
    const ROOT::Math::XYZTVector End4D() const { return end; }

    double StartX() const { return start.X(); }
    double StartY() const { return start.Y(); }
    double StartZ() const { return start.Z(); }
    double StartT() const { return start.T(); }

    double EndX() const { return end.X(); }
    double EndY() const { return end.Y(); }
    double EndZ() const { return end.Z(); }
    double EndT() const { return end.T(); }

  }; // MCLArHit

  // I like to define output operations for my custom classes.
  std::ostream& operator<< (std::ostream& out, const MCLArHit& mcLArHit);

  // Define a list of LAr hits for an event. It's defined as a
  // map<TrackID, MCLArHit> just in case a user needs to know which
  // hits are associated with a given track.
  //
  // An example of iterating through a map without worrying about
  // where the hits came from (which is more "realistic" if we're
  // trying to simulate data aquisition):

  //    for ( const auto& [ trackID, mcLArHit ] : mcLArHits ) {
  //        ... do whatever with mcLArHit, ignoring trackID ...
  //    }

  // Note that this example applies even if you want to use the value
  // of trackID.

  typedef std::map< int, MCLArHit > MCLArHits;

  // I like to define output operations for my custom classes.
  std::ostream& operator<< (std::ostream& out, const MCLArHits& mcLArHits);

} // namespace grams

#endif // _grams_mclarhits_h_
