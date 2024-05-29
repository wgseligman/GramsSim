/// \file EventID.cc
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

#ifndef _gramsg4_eventid_h_
#define _gramsg4_eventid_h_

#include <iostream>
#include <limits>

namespace gramsg4 {

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

    // The auto-generated test for equality is fine for us. 
    bool operator==(const EventID& e) const
    {
      return this->run == e.run  &&
	this->event == e.event;
    }

    // I prefer to include "write" operators in my custom classes to
    // make it easier to examine their contents.
    friend std::ostream& operator<< (std::ostream& out, const EventID& e);

  private:

    int run;    // run number
    int event;  // event number within a run

  };

  // The full definition of operator<<. 
  std::ostream& operator<< (std::ostream& out, const EventID& e) {
    out << "run=" << e.run 
	<< " event=" << e.event;
    return out;
  }

} // namespace gramsg4

#endif // _gramsg4_eventid_h_
