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

    // Since we may want to sort by EventID (for maps and such),
    // define the "less-than" operator.
    bool operator<(const EventID& e) const
    {
      return this->run < e.run  ||
	    (this->run == e.run && this->event < e.event);      
    }

    // Test for equality is fine for us. 
    bool operator==(const EventID& e) const
    {
      return this->run == e.run  &&
	this->event == e.event;
    }

    // I prefer to include "write" operators in my custom classes to
    // make it easier to examine their contents.
    std::ostream& operator<< (std::ostream& out) {
      out << "run=" << this->run 
	  << " event=" << this->event;
      return out;
    }

    // This method is meant to be used with TTree::BuildIndex and
    // TTree::GetEntryWithIndex. The idea is to generate a unique
    // number for each unique EventID, which can then be used to
    // quickly search for a given entry in a TTree.

    // The default BuildIndex and GetEntryWithIndex routines are
    // designed to work with run/event numbers. The purpose of this
    // routine is to provide an index number even if it turns out that
    // run/event is not a good way to specify an EventID for a given
    // experiment.

    int Index() const {
      // Note that this scheme assumes that a given run won't have
      // more than 1000000 events.
      return run*1000000 + event;
    }

  private:

    int run;    // run number
    int event;  // event number within a run

  };

} // namespace grams

#endif // _grams_eventid_h_
