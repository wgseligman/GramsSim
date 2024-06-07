/// \file MCLArHits.h
/// \brief Data object that contains a list of energy deposits in a LAr TPC
// 31-May-2024 WGS

#ifndef _grams_mclarhits_h_
#define _grams_mclarhits_h_

#include <Math/Vector4D.h>

#include <iostream>
#include <map>
#include <tuple>

namespace grams {

  // Define an energy deposit (a "hit" in MC terminology) in in the
  // LAr TPC. This is effectively a record of a "step" within
  // LAr. 

  struct MCLArHit {

    // A pointer back to the MCTrack that created the step.
    int trackID;

    // An arbitrary number assigned in Geant4. Note that 'hitID' is
    // completely arbitrary. It does not imply time ordering, nor
    // anything else.
    int hitID;

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
    int HitID() const { return hitID; }
    int PDGCode() const { return pdgCode; }
    int NumPhotons() const { return numPhotons; }
    int CerPhotons() const { return cerPhotons; }
    int Identifier() const { return volumeID; }
    double E() const { return energy; }
    double Energy() const { return energy; } // an extra accessor can't hurt

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

  // Define a list of LAr hits for an event. It's defined as a
  // map<key, MCLArHit>. where the key is std::tuple<trackID,hitID>.
  // This allows backtracking of which which track and which hit
  // an energy deposit comes from.
  //
  // An example of iterating through a map:

  //    for ( const auto& [ key, mcLArHit ] : mcLArHits ) {
  //        ... do whatever with mcLArHit, ignoring the key, or ...
  //       const auto [ trackID, hitID ] = key; // if you need the key fields
  //    }

  typedef std::map< std::tuple<int,int>, MCLArHit > MCLArHits;

} // namespace grams

// I prefer to define "write" operators for my custom classes to make
// it easier to examine their contents. For these to work in ROOT's
// dictionary-generation system, they must be located outside of any
// namespace.

std::ostream& operator<< (std::ostream& out, const grams::MCLArHit& mcLArHit);
std::ostream& operator<< (std::ostream& out, const grams::MCLArHits& mcLArHits);

#endif // _grams_mclarhits_h_
