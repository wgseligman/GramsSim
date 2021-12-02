/// \file GramsG4ActionInitialization.hh
/// \brief Definition of the GramsG4ActionInitialization class

#ifndef GramsG4ActionInitialization_h
#define GramsG4ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "UserAction.h" // in g4util/

/// This class defines the user action initialization in a
/// multi-threaded environment. It works with our custom user action
/// manager to conform to Geant4's multi-thread requirements.

namespace gramsg4 {

  class ActionInitialization : public G4VUserActionInitialization
  {
  public:
    ActionInitialization();
    virtual ~ActionInitialization();
  
    // In a multithreaded application, this routine is called once, for
    // the "master thread."
    virtual void BuildForMaster() const;

    // In a multithreaded application, this routine is called at the
    // start of each individual thread. In that case, it's the routines
    // called from Build for which you have to think about
    // thread-safety. In a single-thread application, this routine is
    // called only once.
    virtual void Build() const;
  
    // Let the main routine supply the user actions for all the threads.
    void SetUserActionLink(g4util::UserAction* ua = 0)
    { m_userAction = ua; }
  
  private:
    g4util::UserAction* m_userAction;

  };

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace gramsg4

#endif // GramsG4ActionInitialization_h
