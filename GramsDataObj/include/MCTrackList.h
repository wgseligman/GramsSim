/// \file MCTrackList.h
/// \brief Data object that contains a list of Geant4 track information
// 30-May-2024 WGS

#ifndef _grams_mctracklist_h_
#define _grams_mctracklist_h_

#include <TLorentzVector.h>

namespace grams {

  typedef std::pair< TLorentzVector, TLorentzVector > TrajectoryPoint;

} // namespace grams

#endif // _grams_mctracklist_h_
