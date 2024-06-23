/// \file ReadoutWaveforms.h
/// \brief Data object that contains the waveforms from each readout channel
// 1-Jun-2024 WGS

#ifndef _grams_readoutwaveforms_h_
#define _grams_readoutwaveforms_h_

// From GramsDataObj
#include "ReadoutID.h"

#include <iostream>
#include <map>
#include <vector>

namespace grams {

  // Define the waveforms produced by an individual readout channel,

  struct ReadoutWaveform {

    // Include the readout ID, to be consistent with how
    // ElectronClusters works.
    ReadoutID readoutID;

    // Note that the lengths of these two vectors are, in general,
    // _not_ the same.

    // The output analog waveform from a given readout channel.
    std::vector<double> analog;

    // The output digital waveform from a given readout channel.
    std::vector<int> digital;

    // Provide accessors to avoid confusion between a C++ struct
    // and a C++ class.
    const ReadoutID& ID() const { return readoutID; }
    const std::vector<double>& Analog() const { return analog; }
    const std::vector<int>& Digital() const { return digital; }

  }; // ReadoutWaveform

  // Define a list of waveforms for the readout channels in the
  // event. Note that to backtrack from readout channel to cluster to
  // energy deposit, one must read multiple trees and set them up as
  // friends.
  //
  // See GramsElecsim/gramselecsim.cc for an example of how to read
  // tree columns that have been distributed among different files.

  typedef std::map< ReadoutID, ReadoutWaveform > ReadoutWaveforms;

} // namespace grams

// I prefer to define "write" operators for my custom classes to make
// it easier to examine their contents. For these to work in ROOT's
// dictionary-generation system, they must be located outside of any
// namespace.

std::ostream& operator<< (std::ostream& out, const grams::ReadoutWaveform& rw);
std::ostream& operator<< (std::ostream& out, const grams::ReadoutWaveforms& rws);

#endif // _grams_readoutwaveforms_h_
