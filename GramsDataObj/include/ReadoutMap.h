/// \file ReadoutMap.h
/// \brief A map of the clusters to the readout elements.
// 11-Jun-2024 WGS

// This data object is a map between the readout cells and which
// clusters have drifted to each cell.

// The detector-response simulation (DetSim) creates the clusters. The
// electronics simulation (ElecSim) turns the cluster arrival times
// and energies into a waveform. This map, and ReadoutSim, are the
// link between the two.

#ifndef _grams_readoutmap_h_
#define _grams_readoutmap_h_

#include <iostream>
#include <map>
#include <set>

#include "ReadoutID.h"
#include "ElectronClusters.h"

namespace grams {

  // This may be a mistake, but let's see if users can learn enough
  // about C++ and working with STL to understand it. This a
  // "three-dimensional" list. The first "dimension" is the readout
  // ID. The second dimension is a list of the cluster IDs associated
  // with that readoutID. The third dimension are the cluster IDs
  // themselves.

  // We don't store the clusters themselves in this list. We just
  // store their keys, which act as a "pointer" into the full
  // ElectronClusters list.

  typedef std::map< ReadoutID, 
		    std::set< ElectronClusters::key_type > > ReadoutMap;


  // I prefer to define "write" operators for my custom classes to make
  // it easier to examine their contents. For these to work in ROOT's
  // dictionary-generation system, they must be located outside of any
  // namespace.

  ::std::ostream& operator<< (std::ostream& out, grams::ReadoutMap const& rm);

} // namespace grams

#endif // _grams_readoutmap_h_
