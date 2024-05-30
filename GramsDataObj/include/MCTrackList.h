/// \file MCTrackList.h
/// \brief Data object that contains a list of Geant4 track information
// 30-May-2024 WGS

#ifndef _grams_mctracklist_h_
#define _grams_mctracklist_h_

#include <Math/Vector4D.h>

namespace grams {

  // Define a point along a track's trajectory. Provide lots of
  // accessor methods for folks who don't want to look up ROOT's 4D
  // classes.

  struct MCTrajectoryPoint {

    // (position, momentum)
    std::pair< ROOT::Math::XYZTVector, ROOT::Math::PxPyPzEVector > trajectoryPoint;

    // volume identifier
    int volumeID;

    // This is redundant, but just in case someone wants a function
    // instead of struct member:
    int identifier() const {
      return volumeID;
    }

    ROOT::Math::XYZTVector position() const {
      return trajectoryPoint.first;
    }

    ROOT::Math::PxPyPzEVector momentum() const {
      return trajectoryPoint.second;
    }

    double x() const {
      return trajectoryPoint.first.X();
    }

    double y() const {
      return trajectoryPoint.first.Y();
    }

    double z() const {
      return trajectoryPoint.first.Z();
    }

    double t() const {
      return trajectoryPoint.first.T();
    }

    double px() const {
      return trajectoryPoint.second.Px();
    }

    double py() const {
      return trajectoryPoint.second.Py();
    }

    double pz() const {
      return trajectoryPoint.second.Pz();
    }

    double E() const {
      return trajectoryPoint.second.E();
    }

    // It's not likely that we'll need to sort trajectory points. If
    // we do, doing them by time seems to make sense.
    bool operator<( const MCTrajectoryPoint& tp ) const {
      return this->trajectoryPoint.first.T() < tp.trajectoryPoint.first.T();
    }

  }; // TrajectoryPoint 

  // A trajectory is a sequence of trajectory points.
  typedef std::vector< MCTrajectoryPoint > MCTrajectory;

} // namespace grams

#endif // _grams_mctracklist_h_
