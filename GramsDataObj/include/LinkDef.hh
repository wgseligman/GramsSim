#ifdef __CLING__

// This is a ROOT dictionary link definitions file.  The format is
// described (too briefly) at
// <https://root.cern/manual/io_custom_classes/#selecting-dictionary-entries-linkdefh>.

// There must be at least one line below for every data object that's
// used in each header. If an object contains special C++
// classes of its own (e.g., std::map), those must be defined as well.

// In addition, if operator<< is used for any classes, then it must be
// explicitly included below, and it must _not_ be part of any
// namespace.

#pragma link C++ class grams::EventID+;
#pragma link C++ function operator<<(std::ostream&, const grams::EventID&);

// Track objects
#pragma link C++ class std::string+;
#pragma link C++ struct grams::MCTrajectoryPoint+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCTrajectoryPoint&);
#pragma link C++ class grams::MCTrajectory+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCTrajectory&);
#pragma link C++ class grams::MCTrack+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCTrack&);
#pragma link C++ class grams::MCTrackList+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCTrackList&);

// Needed for the hit and cluster objects.
#pragma link C++ class std::tuple<int,int>+;
#pragma link C++ class std::tuple<int,int,int>+;

// Hit objects
#pragma link C++ struct grams::MCLArHit+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCLArHit&);
#pragma link C++ class grams::MCLArHits+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCLArHits&);

#pragma link C++ struct grams::MCScintHit+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCScintHit&);
#pragma link C++ class grams::MCScintHits+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCScintHits&);

// ElectronClusters
#pragma link C++ struct grams::ElectronCluster+;
#pragma link C++ function operator<<(std::ostream&, const grams::ElectronCluster&);
#pragma link C++ class grams::ElectronClusters+;
#pragma link C++ function operator<<(std::ostream&, const grams::ElectronClusters&);

// Readout geometry
#pragma link C++ class grams::ReadoutID+;
#pragma link C++ function operator<<(std::ostream&, const grams::ReadoutID&);

// Readout maps.
#pragma link C++ class grams::ClusterKeys+;
#pragma link C++ function operator<<(std::ostream&, const grams::ClusterKeys&);
#pragma link C++ class grams::ReadoutMap+;
#pragma link C++ function operator<<(std::ostream&, const grams::ReadoutMap&);

// The following statements may not be necessary, but I include them
// for "safety"; see
// https://root.cern.ch/root/htmldoc/guides/users-guide/AddingaClass.html
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#endif // __CLING__
