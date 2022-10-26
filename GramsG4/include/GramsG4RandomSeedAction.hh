/// \file GramsG4RandomSeedAction.hh
/// \brief User-action class to handle random-number engine and seeds.

/// This is a user-action class that handles the initialization and
/// saving of the random-number engine state for this
/// simulation. Contrary to what the name suggests, it does not
/// generate a seed at random!

#ifndef GramsG4RandomSeedAction_H
#define GramsG4RandomSeedAction_H

#include "UserAction.h" // in g4util/

// Forward declarations
namespace util {
  class Options;
}
class G4Run;

namespace gramsg4 {

  class RandomSeedAction : public g4util::UserAction 
  {
  public:

    RandomSeedAction();
    virtual ~RandomSeedAction();

    // The user-action classes we'll need to set up and initialize the
    // random-number engine.
    virtual void BeginOfRunAction(const G4Run*);

  private:

    // To access options XML flags.
    util::Options* m_options;
    G4bool m_verbose;
    G4bool m_debug;
  };

} // namespace gramsg4

#endif // GramsG4RandomSeedAction_H
