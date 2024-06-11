/// \file ReadoutID.h
/// \brief Data object to identify an event.
// 10-Jun-2024 WGS

// This data object encapsulates the details of how a LArTPC's
// readout cells and channels are identified.

// As of Jun-2024, the LArTPC anode readout looks like it will be
// pixel-based. But a couple of years ago it was going to be
// wire-based, and there's current speculation that the readout
// geometry might change again.

// None of this matters to the overall structure of the
// code. Following the general C++ design principle, when we don't
// know what something might turn into, encapsulate it. ReadoutID
// "hides" the structure of the readout so the readout simulation and
// electronics simulation can focus on the physics.

#ifndef _grams_readoutid_h_
#define _grams_readoutid_h_

#include <iostream>
#include <limits>

namespace grams {

  class ReadoutID {

  public:

    // For the default constructor, initialize the fields with values
    // that are not likely to come up in reality.
    ReadoutID()
      : x_index( std::numeric_limits<int>::min() )
      , y_index( std::numeric_limits<int>::min() )
    {}

    // Most likely constructor: Set the values explicitly.
    ReadoutID( int a_x_index, int a_y_index )
      : x_index( a_x_index )
      , y_index( a_y_index )
    {}

    // In EventID, we assigned each ID a unique index. We do the same
    // thing there, but only for the comparison operators; we don't
    // plan to use the index to organize the rows of a tree.

    // Note that the Index is the basis for our comparison operators <
    // and == below. It's important that, however Index() is defined,
    // that it should be unique among the cells..

    int Index() const {
      // Note that this scheme assumes that a given readout geometry
      // won't have more than 1000000 divisions.
      return y_index*1000000 + x_index;
    }

    // Since we may want to sort by ReadoutID (for maps and such),
    // define the "less-than" operator. 
    bool operator<(const ReadoutID& e) const
    {
      return this->Index() < e.Index();
    }

    // Test for equality, just in case.. 
    bool operator==(const ReadoutID& e) const
    {
      return this->Index() == e.Index();
    }

  private:

    // This assumes that the readout geometry is pixel-based in x and y.
    int x_index;   
    int y_index;   

    // I prefer to include "write" operators for my custom classes to make
    // it easier to examine their contents. For ROOT's dictionary
    // definition to function properly, this must be located outside of
    // any namespace.
    friend ::std::ostream& operator<< (std::ostream& out, grams::ReadoutID const& e) {
      out << "x-index=" << e.x_index
	  << " y-index=" << e.y_index;
      return out;
    }

  };

} // namespace grams

#endif // _grams_readoutid_h_
