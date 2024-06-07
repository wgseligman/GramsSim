/// \file MCScintHits.cc
/// \brief Implementation of the longer MCScintHits-related methods.
// 03-Jun-2024 WGS

#include "MCScintHits.h"
#include "iostream"

#include <map>

// How to write a MCScintHit
std::ostream& operator<< (std::ostream& out, const grams::MCScintHit& ahit) {
  out << "Track ID=" << ahit.TrackID()
      << ", Hit ID=" << ahit.HitID()
      << ", PDG code=" << ahit.PDGCode()
      << ", Energy=" << ahit.E()
      << ", Identifier=" << ahit.Identifier()
      << std::endl;
  
  out << "    (x,y,z,t) Start=(" 
      << std::setprecision(3) << std::right
      << std::setw(8) << ahit.StartX() << std::setw(1) << "," 
      << std::setw(8) << ahit.StartY() << std::setw(1) << "," 
      << std::setw(8) << ahit.StartZ() << std::setw(1) << "," 
      << std::setw(8) << ahit.StartT() << std::setw(0)
      << ") End=("
      << std::setprecision(3) << std::right
      << std::setw(8) << ahit.EndX() << std::setw(1) << "," 
      << std::setw(8) << ahit.EndY() << std::setw(1) << "," 
      << std::setw(8) << ahit.EndZ() << std::setw(1) << "," 
      << std::setw(8) << ahit.EndT() << std::setw(0)
      << std::endl;
  
  return out;
}

// How to write a collection of MCScintHits
std::ostream& operator<< (std::ostream& out, const grams::MCScintHits& hits) {
  
  for ( const auto& [ key, mcScintHit ] : hits ) {
    out << mcScintHit;
  }
  out << std::endl;
  
  return out;
}
