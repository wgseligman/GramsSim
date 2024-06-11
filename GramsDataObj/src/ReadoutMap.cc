/// \file ElectronClusters.cc
/// \brief Implementation of the longer ElectronClusters-related methods.
// 01-Jun-2024 WGS

#include "ReadoutID.h"
#include "ReadoutMap.h"
#include "ElectronClusters.h"
#include "iostream"

#include <map>
#include <set>
#include <tuple>

// How to display a list of cluster keys.
std::ostream& operator<< (std::ostream& out, grams::ClusterKeys const& ck) {

  for ( const auto& clusterKey : ck ) {
    // Separate the cluster key into its components.
    const auto& [ trackID, hitID, clusterID ] = clusterKey;
    out << "  trackID=" << trackID
	<< " hitID=" << hitID
	<< " clusterID=" << clusterID
	<< std::endl;
  }

  return out;
}

// How to display a ReadoutMap.
std::ostream& operator<< (std::ostream& out, grams::ReadoutMap const& rm) {
  for ( const auto& [ readoutID, clusterIDList ] : rm ) {
    // Note how this depends on previous definitions of operator<<,
    out << readoutID << std::endl;
    out << clusterIDList << std::endl;
  }
  return out;
}
