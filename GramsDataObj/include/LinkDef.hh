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

#pragma link C++ struct grams::MCTrajectoryPoint+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCTrajectoryPoint&);
#pragma link C++ class grams::MCTrajectory+;
#pragma link C++ class grams::MCTrack+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCTrack&);
#pragma link C++ class grams::MCTrackList+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCTrackList&);

#pragma link C++ struct grams::MCLArHit+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCLArHit&);
#pragma link C++ class grams::MCLArHits+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCLArHits&);

#pragma link C++ struct grams::MCScintHit+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCScintHit&);
#pragma link C++ class grams::MCScintHits+;
#pragma link C++ function operator<<(std::ostream&, const grams::MCScintHits&);

// The following statements may not be necessary, but I include them
// for "safety"; see
// https://root.cern.ch/root/htmldoc/guides/users-guide/AddingaClass.html
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#endif // __CLING__
