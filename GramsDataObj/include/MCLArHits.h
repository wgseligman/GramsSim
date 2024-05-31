/// \file MCLArHits.h
/// \brief Data object that contains a list of energy deposits in a LAr TPC
// 31-May-2024 WGS

#ifndef _grams_mclarhit_h_
#define _grams_mclarhit_h_

#include <Math/Vector3D.h>
#include <Math/Vector4D.h>

#include <iostream>
#include <string>
#include <vector>

namespace grams {

  // Define a point along a track's trajectory. Provide lots of
  // accessor methods for folks who don't want to look up ROOT's 4D
  // classes.

  struct MCLArHit {

    double energy;

  }; // MCLArHit

  // Define a list of hits for an event.
  typedef std::vector< MCLArHit > MCLArHits;

} // namespace grams

#endif // _grams_mclarhit_h_
