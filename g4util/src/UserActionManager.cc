// UserActionManager.cc
// 15-Mar-2002 Bill Seligman
// Invoke the appropriate action for each stored user-action class.

#include "UserActionManager.h"
#include "UserAction.h"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include <vector>

namespace g4util {

  std::vector<UserAction*> UserActionManager::m_userActions;

  UserActionManager::UserActionManager() {}

  UserActionManager::~UserActionManager()
  {
    // Since we adopted the pointers for the user-action classes we're
    // managing, delete all of them.
    for ( auto i = m_userActions.begin(); 
	  i != m_userActions.end(); 
	  i++ )
      {
	delete *i;
      }

    m_userActions.erase( m_userActions.begin(), m_userActions.end() );
  }

  // For the rest of the UserAction methods: invoke the corresponding
  // method for each of the user-action classes we're managing.

  // Reminder: i is a vector<UserAction*>::iterator
  //          *i is a UserAction*

  void UserActionManager::BeginOfRunAction(const G4Run* a_run)
  {
    for ( auto i = m_userActions.begin(); 
	  i != m_userActions.end(); 
	  i++ )
      {
	(*i)->BeginOfRunAction(a_run);
      }
  }

  void UserActionManager::EndOfRunAction(const G4Run* a_run)
  {
    for ( auto i = m_userActions.begin(); 
	  i != m_userActions.end(); 
	  i++ )
      {
	(*i)->EndOfRunAction(a_run);
      }
  }

  void UserActionManager::BeginOfEventAction(const G4Event* a_event)
  {
    for ( auto i = m_userActions.begin(); 
	  i != m_userActions.end(); 
	  i++ )
      {
	(*i)->BeginOfEventAction(a_event);
      }
  }

  void UserActionManager::EndOfEventAction(const G4Event* a_event)
  {
    for ( auto i = m_userActions.begin(); 
	  i != m_userActions.end(); 
	  i++ )
      {
	(*i)->EndOfEventAction(a_event);
      }
  }

  void UserActionManager::TrackingAction(const G4Track* a_track)
  {
    for ( auto i = m_userActions.begin(); 
	  i != m_userActions.end(); 
	  i++ )
      {
	(*i)->TrackingAction(a_track);
      }
  }

  void UserActionManager::SteppingAction(const G4Step* a_step)
  {
    for ( auto i = m_userActions.begin(); 
	  i != m_userActions.end(); 
	  i++ )
      {
	(*i)->SteppingAction(a_step);
      }
  }

} // namespace g4util
