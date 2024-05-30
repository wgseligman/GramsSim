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

  struct TrajectoryPoint {
    std::pair< ROOT::Math::XYZTVector, ROOT::Math::PxPyPzEVector > trajectoryPoint;

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

  };

} // namespace grams

#endif // _grams_mctracklist_h_
