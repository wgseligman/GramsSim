/// \file GramsG4WriteHitsAction.cc
/// \brief User-action class to write out hits.

/// This is a user-action class that writes out the
/// event hits. At present, the output takes the form
/// of basic (and inefficient) ROOT n-tuple.

#include "GramsG4WriteHitsAction.hh"

#include "UserAction.h" // in g4util/
#include "Analysis.h" // in g4util/

#include "G4Run.hh"
#include "G4Event.hh"

namespace gramsg4 {

  WriteHitsAction::WriteHitsAction()
    : UserAction()
  {}

  WriteHitsAction::~WriteHitsAction() {}

  // The user-action classes we'll need to define
  // the output file and write hits.

  void WriteHitsAction::BeginOfRunAction(const G4Run*) {}
  void WriteHitsAction::EndOfRunAction(const G4Run*) {}
  void WriteHitsAction::BeginOfEventAction(const G4Event*) {}
  void WriteHitsAction::EndOfEventAction(const G4Event*) {}

} // namespace gramsg4
