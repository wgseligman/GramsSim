/// \file MCTrackList.cc
/// \brief Implementation of the longer MCTrackList-related methods.
// 31-May-2024 WGS

#include "MCTrackList.h"
#include "iostream"

namespace grams {

  // How to write a MCTrajectoryPoint.
  std::ostream& operator<< (std::ostream& out, const MCTrajectoryPoint& tp) {
    out << "(x,y,z,t)=(" 
	<< std::setprecision(5) << std::right
	<< std::setw(11) << tp.x() << std::setw(1) << "," 
	<< std::setw(11) << tp.y() << std::setw(1) << "," 
	<< std::setw(11) << tp.z() << std::setw(1) << "," 
	<< std::setw(11) << tp.t() << std::setw(0)
	<< ") (px,py,pz,E)=("
	<< std::setprecision(5) << std::right
	<< std::setw(11) << tp.px() << std::setw(1) << "," 
	<< std::setw(11) << tp.py() << std::setw(1) << "," 
	<< std::setw(11) << tp.pz() << std::setw(1) << "," 
	<< std::setw(11) << tp.E() << std::setw(0)
	<< ") id="
	<< tp.identifier();
    return out;
  }

  // The full definition of operator<< for MCTrack.
  std::ostream& operator<< (std::ostream& out, const MCTrack& track) {
    out << "Track ID " << track.TrackID()
	<< " PDG code " << track.PDGCode()
	<< " parent ID " << track.ParentID()
	<< " process (start) " << track.Process()
	<< " process (end) " << track.EndProcess()
	<< std::endl;
    out << "   daughters:";
    for ( const auto& d : track.Daughters() ) {
      out << " " << d;
    }
    out << std::endl;
    out << "   trajectory points:" << std::endl;
    for ( const auto& tp : track.Trajectory() ) {
      out << tp << std::endl;
    }
    return out;
  }


} // namespace grams
