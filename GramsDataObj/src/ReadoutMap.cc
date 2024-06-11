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

// How to display a ReadoutMap.

std::ostream& operator<< (std::ostream& out, grams::ReadoutMap const& rm) {
  for ( const auto& [ readoutID, clusterIDlist ] : rm ) {

    // Note how this depends on the definition of operator<< in ReadoutID.h.
    out << readoutID << std::endl;

    for ( const auto& clusterKey : clusterIDlist ) {
      // Separate the cluster key into its components.
      const auto& [ trackID, hitID, clusterID ] = clusterKey;
      out << "  trackID=" << trackID
	  << " hitID=" << hitID
	  << " clusterID=" << clusterID
	  << std::endl;
    }
    out << std::endl;
  }
  return out;
}
