/// \file ScintillatorHit.cc
/// \brief Implementation of the ScintillatorHit class

/// As it says in GramsG4ScintillatorHit.hh, this is duplicated
/// from Geant4 example B2a.

#include "GramsG4ScintillatorHit.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

namespace gramsg4 {

  G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator=0;

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // Default constructor
  ScintillatorHit::ScintillatorHit()
    : G4VHit() 
    , m_trackID(-1)
    , m_pdgCode(0)
    , m_energy(0.)
    , m_startTime(0.)
    , m_endTime(0.)
    , m_startPosition(G4ThreeVector())
    , m_endPosition(G4ThreeVector())
    , m_identifier(0)
  {}

  // Constructor with arguments
  ScintillatorHit::ScintillatorHit( 
      G4int trackID,
      G4int PDG,
      G4double energy,
      G4double tstart,
      G4double tend,
      G4ThreeVector start,
      G4ThreeVector end,
      G4int ident)
    : G4VHit() 
    , m_trackID(trackID)
    , m_pdgCode(PDG)
    , m_energy(energy)
    , m_startTime(tstart)
    , m_endTime(tend)
    , m_startPosition(start)
    , m_endPosition(end)
    , m_identifier(ident)
  {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  ScintillatorHit::~ScintillatorHit() {}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  ScintillatorHit::ScintillatorHit(const ScintillatorHit& right)
    : G4VHit()
  {
    m_trackID       = right.m_trackID;
    m_pdgCode       = right.m_pdgCode;
    m_energy        = right.m_energy;
    m_startTime     = right.m_startTime;
    m_endTime       = right.m_endTime;
    m_startPosition = right.m_startPosition;
    m_endPosition   = right.m_endPosition;
    m_identifier    = right.m_identifier;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  const ScintillatorHit& ScintillatorHit::operator=(const ScintillatorHit& right)
  {
    m_trackID       = right.m_trackID;
    m_pdgCode       = right.m_pdgCode;
    m_energy        = right.m_energy;
    m_startTime     = right.m_startTime;
    m_endTime       = right.m_endTime;
    m_startPosition = right.m_startPosition;
    m_endPosition   = right.m_endPosition;
    m_identifier    = right.m_identifier;

    return *this;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  G4bool ScintillatorHit::operator==(const ScintillatorHit& right) const
  {
    return ( this == &right ) ? true : false;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


  // This is standard routine for drawing hits in the visualizer.
  // You'll find almost identical code in chapter 8.5.6 of the 
  // Geant4 Applications Guide. 
  void ScintillatorHit::Draw()
  {
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager)
      {
	G4Circle circle((m_startPosition+m_endPosition)/2.);
	circle.SetScreenSize(4.0);
	circle.SetFillStyle(G4Circle::filled);
	G4Colour colour(1.,0.,0.); // red
	G4VisAttributes attribs(colour);
	circle.SetVisAttributes(attribs);
	pVVisManager->Draw(circle);
      }
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void ScintillatorHit::Print()
  {
    auto eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

    G4cout 
      << " ScintillatorHit: eventID=" << eventID
      << " trackID=" << m_trackID 
      << " PDG=" << m_pdgCode
      << " Edep="
      << std::setw(7) << G4BestUnit(m_energy,"Energy")
      << " Start Time="
      << std::setw(7) << G4BestUnit(m_startTime,"Time")
      << " Start Position="
      << std::setw(7) << G4BestUnit(m_startPosition,"Length")
      << " End Time="
      << std::setw(7) << G4BestUnit(m_endTime,"Time")
      << " End Position="
      << std::setw(7) << G4BestUnit(m_endPosition,"Length")
      << " Identifier="
      << m_identifier
      << G4endl;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace gramsg4
