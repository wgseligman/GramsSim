/// \file ElectronClusters.cc
/// \brief Implementation of the longer ElectronClusters-related methods.
// 01-Jun-2024 WGS

#include "ElectronClusters.h"
#include "iostream"

#include <map>
#include <tuple>

// How to write a ElectronCluster
std::ostream& operator<< (std::ostream& out, const grams::ElectronCluster& acluster) {
  out << "Track ID=" << acluster.TrackID()
      << ", Hit ID=" << acluster.HitID()
      << ", Cluster ID=" << acluster.ClusterID()
      << ", #electrons=" << acluster.NumElectrons()
      << ", at anode: Energy=" << acluster.EAtAnode()
      << " (x,y,z,t)=(" 
      << std::setprecision(3) << std::right
      << std::setw(8) << acluster.XAtAnode() << std::setw(1) << "," 
      << std::setw(8) << acluster.YAtAnode() << std::setw(1) << "," 
      << std::setw(8) << acluster.ZAtAnode() << std::setw(1) << "," 
      << std::setw(8) << acluster.TAtAnode() << std::setw(0) << ")"
      << std::endl;

  return out;
}

// How to write a collection of ElectronClusters
std::ostream& operator<< (std::ostream& out, const grams::ElectronClusters& clusters) {

  for ( const auto& [ key, cluster ] : clusters ) {
    out << cluster;
  }
  out << std::endl;
  
  return out;
}
