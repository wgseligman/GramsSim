/// \file MCLArHits.cc
/// \brief Implementation of the longer MCLArHits-related methods.
// 01-Jun-2024 WGS

#include "MCLArHits.h"
#include "iostream"

#include <map>
#include <tuple>

// How to write a MCLArHit
std::ostream& operator<< (std::ostream& out, const grams::MCLArHit& ahit) {
  out << "Track ID=" << ahit.TrackID()
      << ", Hit ID=" << ahit.HitID()
      << ", PDG code=" << ahit.PDGCode()
      << ", Energy=" << ahit.E()
      << ", Scintillation Photons=" << ahit.NumPhotons()
      << ", Cerenkov Photons=" << ahit.CerPhotons()
      << ", Identifier=" << ahit.Identifier()
      << std::endl;

  out << "    (x,y,z,t) Start=(" 
      << std::setprecision(5) << std::right
      << std::setw(11) << ahit.StartX() << std::setw(1) << "," 
      << std::setw(11) << ahit.StartY() << std::setw(1) << "," 
      << std::setw(11) << ahit.StartZ() << std::setw(1) << "," 
      << std::setw(11) << ahit.StartT() << std::setw(0)
      << ") End=("
      << std::setprecision(5) << std::right
      << std::setw(11) << ahit.EndX() << std::setw(1) << "," 
      << std::setw(11) << ahit.EndY() << std::setw(1) << "," 
      << std::setw(11) << ahit.EndZ() << std::setw(1) << "," 
      << std::setw(11) << ahit.EndT() << std::setw(0)
      << std::endl;

  return out;
}

// How to write a collection of MCLArHits
std::ostream& operator<< (std::ostream& out, const grams::MCLArHits& hits) {

  for ( const auto& [ key, mcLArHit ] : hits ) {
    out << mcLArHit;
  }
  out << std::endl;
  
  return out;
}
