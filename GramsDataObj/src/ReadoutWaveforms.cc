/// \file ReadoutWaveforms.cc
/// \brief Implementation of the longer ReadoutWaveforms-related methods.
// 01-Jun-2024 WGS

#include "ReadoutID.h"
#include "ReadoutWaveforms.h"
#include "iostream"

#include <map>
#include <vector>

// How to display the waveforms for a single cell..
std::ostream& operator<< (std::ostream& out, grams::ReadoutWaveform const& rw) {

  out << rw.readoutID << std::endl;

  // A waveform struct contains two vectors, one analog and
  // digital. For now, print them separately.
  out << "   Analog nbins=" << rw.analog.size() << ":";
  for ( const auto value : rw.analog ) {
    out << " " << value; 
  }
  out << std::endl;

  out << "   Digital nbins=" << rw.digital.size() << ":";
  for ( const auto value : rw.digital ) {
    out << " " << value; 
  }
  out << std::endl;

  return out;
}

// How to display the entire list of waveforms..
std::ostream& operator<< (std::ostream& out, grams::ReadoutWaveforms const& rws) {
  for ( const auto& [ readoutID, waveforms ] : rws ) {
    // Note how this depends on previous definitions of operator<<,
    out << waveforms << std::endl;
  }
  out << std::endl;
  return out;
}
