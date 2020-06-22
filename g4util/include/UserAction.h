/// g4util::UserAction.h
/// 1-Sep-1999 Bill Seligman

/// This is an abstract base class to be used with Geant 4.

/// Why is this interface useful?  Answer: Geant4 provides several
/// classes that are meant to be "user hooks" in G4 processing.  A
/// couple of examples are G4UserRunAction and G4EventAction.  The user
/// is meant to publically inherit from these classes in order to
/// perform tasks at the beginning and ending of run or event
/// processing.

/// However, typical tasks that physicists perform generally involve
/// more than one user-hook class.  For example, to make histograms, a
/// physicist might define the histograms at the beginning of a run,
/// fill the histograms after each event, and write the histograms at
/// the end of a run.

/// It's handy to have all the code for such tasks (making histograms,
/// studying G4 tracking, event persistency) all in one class, rather
/// than split between two or three different classes.  That's where
/// UserAction comes in.  It gathers all the G4 user-hook or
/// user-action classes into one place.

/// For an example of how this class can be used, see the class
/// LArRootPersistency in the LArG4Sim package.  This class implements
/// the UserAction interface to make NTuples and write ROOT trees.

#ifndef g4util_UserAction_H
#define g4util_UserAction_H

// The following objects are the arguments to the methods
// invoked in the user action classes.  In other words, they
// contain the variables that we are normally able to record
// in Geant.

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"

namespace g4util {

  class UserAction {

  public:

    virtual ~UserAction() {};

    // The following a list of methods that correspond to the available 
    // user action classes in Geant 4.0.1.   In this base class, the
    // methods are defined to do nothing.

    // The method names begin with "Record" because almost every time
    // this class is used, it has to do with 'recording' information
    // from Geant4.

    // Note: I don't implement a hook for the user stacking action since
    // that method is not typically used for I/O.

    virtual void BeginOfRunAction(const G4Run*) {};
    virtual void EndOfRunAction(const G4Run*) {};
    virtual void BeginOfEventAction(const G4Event*) {};
    virtual void EndOfEventAction(const G4Event*) {};
    virtual void SteppingAction(const G4Step*) {};
    virtual void TrackingAction(const G4Track*) {};

  };

} // namespace g4util

#endif // g4util_UserAction_H
