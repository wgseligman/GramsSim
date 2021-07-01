/// \file ScintillatorHit.hh
/// \brief Definition of the ScintillatorHit class

/// In actual scintillators, these hits will have to be
/// accumulated and interpreted as relative signals from
/// SiPMs (or a similar device) at both ends of the scintillator
/// bars. Here we just store each individual energy deposit
/// in the scintillator. 

#ifndef ScintillatorHit_h
#define ScintillatorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh" // G4 thread-local storage

/// Scintillator hit class
///
/// This is copied from Geant4 B2a, with appropriate changes.

namespace gramsg4 {

  class ScintillatorHit : public G4VHit
  {
  public:
    // Default constructor
    ScintillatorHit();

    // Constructor with arguments.
    ScintillatorHit( G4int trackID,
		     G4int PDG,
		     G4double energy,
		     G4double tstart,
		     G4double tend,
		     G4ThreeVector start,
		     G4ThreeVector end,
		     G4int ident
		     );

    // Copy constructor
    ScintillatorHit(const ScintillatorHit&);
    // Destructor
    virtual ~ScintillatorHit();
  
    // operators
    const ScintillatorHit& operator=(const ScintillatorHit&);
    G4bool operator==(const ScintillatorHit&) const;
  
    inline void* operator new(size_t);
    inline void  operator delete(void*);
  
    // methods from base class
    virtual void Draw();
    virtual void Print();
  
    // Set methods
    void SetTrackID    (G4int track)      { m_trackID = track; };
    void SetPDGCode    (G4int pdg)        { m_pdgCode = pdg; };
    void SetEnergy     (G4double de)      { m_energy = de; };
    void SetStartTime  (G4double t)       { m_startTime = t; };
    void SetEndTime    (G4double t)       { m_endTime = t; };
    void SetStartPosition (G4ThreeVector xyz) { m_startPosition = xyz; };
    void SetEndPosition   (G4ThreeVector xyz) { m_endPosition = xyz; };
    void SetIdentifier (G4int identifier ){ m_identifier = identifier; };
  
    // Get methods
    G4int GetTrackID() const          { return m_trackID; };
    G4int GetPDGCode() const          { return m_pdgCode; };
    G4double GetEnergy() const        { return m_energy; };
    G4double GetStartTime() const     { return m_startTime; };
    G4double GetEndTime() const       { return m_endTime; };
    G4ThreeVector GetStartPosition() const { return m_startPosition; };
    G4ThreeVector GetEndPosition() const   { return m_endPosition; };
    G4int GetIdentifier() const       { return m_identifier; };
  
  private:
  
    G4int         m_trackID; 
    G4int         m_pdgCode;
    G4double      m_energy;          ///< units MeV
    G4double      m_startTime  ;     ///< units ns
    G4double      m_endTime    ;     ///< units ns
    G4ThreeVector m_startPosition;   ///< units mm
    G4ThreeVector m_endPosition;     ///< units mm
    G4int         m_identifier;      ///< scintillator ID
  };

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  typedef G4THitsCollection<ScintillatorHit> ScintillatorHitsCollection;

  extern G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator;

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  inline void* ScintillatorHit::operator new(size_t)
  {
    if(!ScintillatorHitAllocator)
      ScintillatorHitAllocator = new G4Allocator<ScintillatorHit>;
    return (void *) ScintillatorHitAllocator->MallocSingle();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  inline void ScintillatorHit::operator delete(void *hit)
  {
    ScintillatorHitAllocator->FreeSingle((ScintillatorHit*) hit);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace gramsg4

#endif
