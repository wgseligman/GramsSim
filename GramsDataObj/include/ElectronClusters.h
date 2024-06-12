/// \file ElectronClusters.h
/// \brief Data object that contains a list of energy deposits in a LAr TPC
// 07-Jun-2024 WGS

#ifndef _grams_electronclusters_h_
#define _grams_electronclusters_h_

#include <Math/Vector4D.h>

#include <iostream>
#include <map>
#include <tuple>

namespace grams {

  // Define an electron cluster created by ionization. Instead of
  // drifting each individual ionization, they're divided into
  // clusters to speed up processing.

  struct ElectronCluster {

    // A pointer back to the MCTrack that created the step.
    int trackID;

    // An arbitrary number assigned in Geant4. Note that 'hitID' is
    // completely arbitrary. It does not imply time ordering, nor
    // anything else.
    int hitID;

    // Another completely arbitrary number. Again, it does not imply
    // time ordering, nor anything else.
    int clusterID;

    // This is the cluster energy at the readout anode.
    double energy;

    // This is the number of electrons in the cluster.
    int numElectrons;

    // The (x,y,t) of the cluster at the readout anode. "z" is
    // different; it's the value of z used to calculate the value of
    // t.
    ROOT::Math::XYZTVector position;

    // Provide accessors to avoid confusion between a C++ struct
    // and a C++ class.
    int TrackID() const { return trackID; }
    int HitID() const { return hitID; }
    int ClusterID() const { return clusterID; }
    double EAtAnode() const { return energy; }
    double EnergyAtAnode() const { return energy; } // an extra accessor can't hurt
    int NumElectrons() const { return numElectrons; }

    // For users who prefer to work with ROOT's 4D vectors, and for
    // those who don't:
    const ROOT::Math::XYZTVector PositionAtAnode4D() const { return position; }

    double XAtAnode() const { return position.X(); }
    double YAtAnode() const { return position.Y(); }
    double ZAtAnode() const { return position.Z(); }
    double TAtAnode() const { return position.T(); }

  }; // ElectronCluster

  // Define a list of clusters for an event. It's defined as a
  // map<key, ElectronCluster>. where the key is std::tuple<trackID,hitID,clusterID>.
  // This allows backtracking of the track, hit, and cluster.
  //
  // An example of iterating through a map:

  //    for ( const auto& [ key, cluster ] : clusters ) {
  //        ... do whatever with cluster, ignoring the key, or ...
  //       const auto [ trackID, hitID, clusterID ] = key; // if you need the key fields
  //    }

  typedef std::map< std::tuple<int,int,int>, ElectronCluster > ElectronClusters;

} // namespace grams

// I prefer to define "write" operators for my custom classes to make
// it easier to examine their contents. For these to work in ROOT's
// dictionary-generation system, they must be located outside of any
// namespace.

std::ostream& operator<< (std::ostream& out, const grams::ElectronCluster& cluster);
std::ostream& operator<< (std::ostream& out, const grams::ElectronClusters& clusters);

#endif // _grams_electronclusters_h_
