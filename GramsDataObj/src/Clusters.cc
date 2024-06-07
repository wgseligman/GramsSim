/// \file Clusters.cc
/// \brief Implementation of the longer Clusters-related methods.
// 01-Jun-2024 WGS

#include "Clusters.h"
#include "iostream"

#include <map>
#include <tuple>

// How to write a Cluster
std::ostream& operator<< (std::ostream& out, const grams::Cluster& acluster) {
  out << "Track ID=" << acluster.TrackID()
      << ", Hit ID=" << acluster.HitID()
      << ", Cluster ID=" << acluster.ClusterID()
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

// How to write a collection of Clusters
std::ostream& operator<< (std::ostream& out, const grams::Clusters& clusters) {

  for ( const auto& [ key, cluster ] : clusters ) {
    out << cluster;
  }
  out << std::endl;
  
  return out;
}
