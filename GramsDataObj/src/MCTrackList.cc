/// \file MCTrackList.cc
/// \brief Implementation of the longer MCTrackList-related methods.
// 31-May-2024 WGS

#include "MCTrackList.h"
#include "iostream"

#include <set>
#include <map>

namespace grams {

  // How to write a MCTrajectoryPoint.
  std::ostream& operator<< (std::ostream& out, const MCTrajectoryPoint& tp) {

    // In an effort to keep the output neat, use the C++ formatting
    // functions. This might distort subsequent output outside this
    // routine, so save the current format.
    std::ios savedFormat(nullptr);
    savedFormat.copyfmt( out );

    out << "(x,y,z,t)=(" 
	<< std::setprecision(5) << std::right
	<< std::setw(11) << tp.X() << std::setw(1) << "," 
	<< std::setw(11) << tp.Y() << std::setw(1) << "," 
	<< std::setw(11) << tp.Z() << std::setw(1) << "," 
	<< std::setw(11) << tp.T() << std::setw(0)
	<< ") (px,py,pz,E)=("
	<< std::setprecision(5) << std::right
	<< std::setw(11) << tp.Px() << std::setw(1) << "," 
	<< std::setw(11) << tp.Py() << std::setw(1) << "," 
	<< std::setw(11) << tp.Pz() << std::setw(1) << "," 
	<< std::setw(11) << tp.E() << std::setw(0)
	<< ") id="
	<< tp.Identifier();

    // Restore the saved output format.
    out.copyfmt( savedFormat );

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

    // Perhaps there should be some sort of "verbose" flag to control
    // whether the following is printed. However, let's keep this code
    // independent of the Options class, at least for now.

    out << "   daughters:";
    for ( const auto& d : track.Daughters() ) {
      out << " " << d;
    }
    out << std::endl;

    out << "   trajectory points:" << std::endl;
    for ( const auto& tp : track.Trajectory() ) {
      // Note how the following statement depends on the definition of
      // operator<< for MCTrajectoryPoint above.
      out << "      " << tp << std::endl;
    }

    // At least for now, don't print out the polarization or the
    // weight.

    return out;
  }

  // How to print an entire MCTrackList at once. 
  std::ostream& operator<< (std::ostream& out, const MCTrackList& trackList) {
    // Loop over every entry in the map and print it.
    for ( const auto& [ trackID, track ] : trackList ) {

      // Note how the following statement can be simple because of the
      // above definition of operator<< for an MCTrack.
      out << track << std::endl;

      // Separate each track with a blank line.
      out << std::endl;
    }
    return out;
  }

} // namespace grams
