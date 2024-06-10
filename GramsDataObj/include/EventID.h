/// \file EventID.h
/// \brief Data object to identify an event.
// 29-May-2024 WGS

// This data object encapsulates the details of how should be
// identified in a GramsSim n-tuple. 

// As of May-2024, EventID seems almost too simple: Just a run number
// and an event number. However, the event ID may become more complex
// as experiment details are added.

// For example, in a balloon/satellite experiment, it may make more
// sense to identify events by date/time. Or perhaps we may need to
// introduct the concept of "sub-runs".

// The idea is that no matter how we might have to revise this data
// object, in the code we can simply search, sort, and test against
// EventID types.

#ifndef _grams_eventid_h_
#define _grams_eventid_h_

#include <iostream>
#include <limits>

namespace grams {

  class EventID {

  public:

    // For the default constructor, initialize the fields with values
    // that are not likely to come up in reality.
    EventID()
      : run( std::numeric_limits<int>::min() )
      , event( std::numeric_limits<int>::min() )
    {}

    // Most likely constructor: Set the values explicitly.
    EventID( int a_run, int a_event )
      : run( a_run )
      , event( a_event )
    {}

    // This method is meant to be used with TTree::BuildIndex and
    // TTree::GetEntryWithIndex. The idea is to generate a unique
    // number for each unique EventID, which can then be used to
    // quickly search for a given entry in a TTree.

    // The default BuildIndex and GetEntryWithIndex routines are
    // designed to work with run/event numbers. The purpose of this
    // routine is to provide an index number even if it turns out that
    // run/event is not a good way to specify an EventID for a given
    // experiment.

    // Note that the Index is the basis for our comparison operators <
    // and == below. It's important that, however Index() is defined,
    // that it should be unique among the rows of a tree.

    int Index() const {
      // Note that this scheme assumes that a given run won't have
      // more than 1000000 events.
      return run*1000000 + event;
    }

    // Since we may want to sort by EventID (for maps and such),
    // define the "less-than" operator. 
    bool operator<(const EventID& e) const
    {
      return this->Index() < e.Index();
    }

    // Test for equality, just in case.. 
    bool operator==(const EventID& e) const
    {
      return this->Index() == e.Index();
    }

  private:

    int run;    // run number
    int event;  // event number within a run

    // I prefer to include "write" operators for my custom classes to make
    // it easier to examine their contents. For ROOT's dictionary
    // definition to function properly, this must be located outside of
    // any namespace.
    friend ::std::ostream& operator<< (std::ostream& out, grams::EventID const& e) {
      out << "run=" << e.run
	  << " event=" << e.event;
      return out;
    }

  };

} // namespace grams

#endif // _grams_eventid_h_
