/// \file MCTrackList.h
/// \brief Data object that contains a list of Geant4 track information
// 30-May-2024 WGS

#ifndef _grams_mctracklist_h_
#define _grams_mctracklist_h_

#include <Math/Vector3D.h>
#include <Math/Vector4D.h>

#include <iostream>
#include <string>
#include <set>

namespace grams {

  // Define a point along a track's trajectory. Provide lots of
  // accessor methods for folks who don't want to look up ROOT's 4D
  // classes.

  struct MCTrajectoryPoint {

    // (position, momentum)
    ROOT::Math::XYZTVector position;
    ROOT::Math::PxPyPzEVector momentum;

    // volume identifier
    int volumeID;

    // This is redundant, but just in case someone wants a function
    // instead of a struct member:
    int identifier() const { return volumeID; }

    ROOT::Math::XYZTVector Position4D() const { return position; }

    ROOT::Math::PxPyPzEVector Momentum4D() const { return momentum; }

    double x() const { return position.X(); }
    double y() const { return position.Y(); }
    double z() const { return position.Z(); }
    double t() const { return position.T(); }

    double px() const { return momentum.Px(); }
    double py() const { return momentum.Py(); }
    double pz() const { return momentum.Pz(); }
    double E()  const { return momentum.E(); }

    // Define a "less-than" operator so we can have a sorted list.
    bool operator<( const MCTrajectoryPoint& tp ) const {
      return this->position.T() < tp.position.T();
    }

  }; // MCTrajectoryPoint 

  // I prefer to define "write" operators for my custom classes to
  // make it easier to examine their contents.  In order to print an
  // MCTrajectoryPoint:
  std::ostream& operator<< (std::ostream& out, const MCTrajectoryPoint& tp);

  // A trajectory is a sequence of trajectory points. It's not likely
  // we'll have two trajectory points in a track with the same time
  // (see MCTrajectoryPoint::operator<); it's more likely that the
  // time difference might be tiny. So allow for multiple instances of
  // points with the same time by using a multiset.
  typedef std::multiset< MCTrajectoryPoint > MCTrajectory;

  // Why is MCTrajectoryPoint a struct but MCTrack is a class? The
  // general C++ coding principle is: If an object validates its data
  // members, use a class; if an object performs no validation, make
  // it a struct.

  // MCTrack contains the information for a single track in the
  // Monte-Carlo simulation.

  class MCTrack {

  public:

    // For the most part, we're going to construct an MCTrack by
    // declaring it, then using "setters" to define its internal
    // values. Supply some defaults on initialization. 
    MCTrack() 
      : trackID(0)
      , polarization(0.,0.,0.)
      , weight(1.0)
    {}

    // The setters and getters: access to the information in this
    // class.

    int TrackID() const { return trackID; }
    void SetTrackID( const int& tid ) { trackID = tid; }

    int PDGCode() const { return pdgCode; }
    void SetPDGCode( const int& pdg ) { pdgCode = pdg; }

    int ParentID() const { return parentID; }
    void SetParentID( const int& pid ) { parentID = pid; }

    std::string Process() const { return process; }
    void SetProcess( const std::string& p ) { process = p; }

    std::string EndProcess() const { return endProcess; }
    void SetEndProcess( const std::string& p ) { endProcess = p; }

    double Weight() const { return weight; }
    void SetWeight( const double& w ) { weight = w; }

    // Providing access to a list of daughters.
    size_t NumDaughters() const { return daughters.size(); }
    // Look at daughter "i". Note that this inefficient, since a set
    // cannot be indexed like a vector.
    int Daughter( const int& i ) const { 
      auto d = daughters.cbegin();
      std::advance(d, i);
      return *d;
    }
    // This is more efficient: Return the set and let the user do the
    // iteration. However, this requires the user to understand how
    // std::set works.
    const std::set<int>& Daughters() const { return daughters; }
    void AddDaughter( const int& i ) { daughters.insert(i); }

    // If the user wants to iterate over the list of trajectory
    // points, by far the most efficient way is to return the list and
    // let the user implement the loop. However, perhaps the user
    // doesn't understand how STL containers work.
    const MCTrajectory& Trajectory() const { return trajectory; }
    // So, as with Daughters, provide access to the individual
    // trajectory points.
    size_t NumTrajectoryPoints() const { return trajectory.size(); }
    const MCTrajectoryPoint& TrajectoryPoint( const int& i ) {
      auto t = trajectory.cbegin();
      std::advance(t, i);
      return *t;
    }
    // Add a point to the trajectory. Typically this will be invoked
    // by something like:
    // track.AddTrajectoryPoint ( {x,y,z,t}, {px,py,pz,E}, identifier} );
    void AddTrajectoryPoint( const ROOT::Math::XYZTVector& pos,
			     const ROOT::Math::XYZTVector& mom,
			     int identifier ) {
      MCTrajectoryPoint mtp;
      mtp.position = pos;
      mtp.momentum = mom;
      mtp.volumeID = identifier;
      trajectory.insert( mtp );
    }

    // Many ways to set and get a vector.
    ROOT::Math::XYZVector Polarization() const { return polarization; }
    void SetPolarization( const ROOT::Math::XYZVector& p ) { polarization = p; }

    double PolarizationX() const { return polarization.X(); }
    double PolarizationY() const { return polarization.Y(); }
    double PolarizationZ() const { return polarization.Z(); }
    void SetPolarizationX( const double& a ) { polarization.SetX(a); }
    void SetPolarizationY( const double& a ) { polarization.SetY(a); }
    void SetPolarizationZ( const double& a ) { polarization.SetZ(a); }

    // If we want to sort tracks (e.g., in a std::set), the natural
    // way seems to be by track ID.
    bool operator<( const MCTrack& track ) const {
      return this->trackID < track.trackID;
    }

  private:

    // Note: While a common standard is for a class's private member
    // name is to use some kind of prefix in the variable name (e.g.,
    // fparentID, m_parentID), when using ROOT I/O with dictionary
    // generation the branches inherit the names of the
    // variables. This becomes awkward in the subsequent analysis.
    // That's why the private members don't have "header prefixes".

    // Note that the trackID is also used to index MCTrackList.
    int trackID;

    // The PDG code for the particle in this track.
    int pdgCode;

    // The parent particle ID of this track. If this is a primary
    // particle, this field should be -1. Note that due to energy
    // cuts, it's possible that this parent ID might not be found in
    // any other track's list of daughter IDs.
    int parentID;

    // This list of daughter particle IDs of this track. Note that due
    // to energy cuts and such, it's possible that not all daughters
    // will be included in this list.
    std::set<int> daughters;

    // The simulation process that created this track. If this is a
    // primary particle, its value will be "primary".
    std::string process;

    // The process that ended this track. 
    std::string endProcess;

    // The list of points that make up the track's trajectory.
    MCTrajectory trajectory;

    // In case we need it.
    ROOT::Math::XYZVector polarization;

    // For some studies, we might want to re-weight tracks.
    double weight;

  }; // MCTrack

  // I prefer to include "write" operators for my custom classes to
  // make it easier to examine their contents. To write an MCTrack:
  std::ostream& operator<< (std::ostream& out, const MCTrack& m);

  // Define a list of tracks for an event.
  typedef std::set< MCTrack > MCTrackList;

} // namespace grams

#endif // _grams_mctracklist_h_
