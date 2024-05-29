#ifdef __ROOTCLING__

// This is a ROOT dictionary link definitions file.  The format is
// described (too briefly) at
// <https://root.cern/manual/io_custom_classes/#selecting-dictionary-entries-linkdefh>.

// There must be at least one line below for every data object that's
// defined in GramsDataObj. If that object contains special C++
// classes of its own (e.g., std::map), those must be defined as well.

#pragma link C++ class EventID+;

// The following statements may not be necessary, but I include them
// for "safety"; see
// https://root.cern.ch/root/htmldoc/guides/users-guide/AddingaClass.html
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#endif // __CLING__
