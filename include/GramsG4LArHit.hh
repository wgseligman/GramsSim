/// \file LArHit.hh
/// \brief Definition of the LArHit class

#ifndef LArHit_h
#define LArHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh" // G4 thread-local storage

/// LAr Active TPC hit class
///
/// This is copied from Geant4 B2a, with appropriate changes.

namespace gramsg4 {

  class LArHit : public G4VHit
  {
  public:
    // Default constructor
    LArHit();

    // Constructor with arguments
    LArHit(G4int trackID,
	   G4int PDG,
	   G4int nPhotons,
	   G4double energy,
	   G4double tstart,
	   G4double tend,
	   G4ThreeVector start,
	   G4ThreeVector end, 
	   G4int ident);

    // Copy constructor
    LArHit(const LArHit&);

    // Destructor
    virtual ~LArHit();
  
    // operators
    const LArHit& operator=(const LArHit&);
    G4bool operator==(const LArHit&) const;
  
    inline void* operator new(size_t);
    inline void  operator delete(void*);
  
    // methods from base class
    virtual void Draw();
    virtual void Print();
  
    // Set methods
    void SetTrackID    (G4int track)      { m_trackID = track; };
    void SetPDGCode    (G4int pdg)        { m_pdgCode = pdg; };
    void SetNumPhotons (G4int numPhotons) { m_numPhotons = numPhotons; };
    void SetEnergy     (G4double de)      { m_energy = de; };
    void SetStartTime  (G4double t)       { m_startTime = t; };
    void SetEndTime    (G4double t)       { m_endTime = t; };
    void SetStartPosition (G4ThreeVector xyz) { m_startPosition = xyz; };
    void SetEndPosition   (G4ThreeVector xyz) { m_endPosition = xyz; };
    void SetIdentifier (G4int identifier ){ m_identifier = identifier; };
  
    // Get methods
    G4int GetTrackID() const          { return m_trackID; };
    G4int GetPDGCode() const          { return m_pdgCode; };
    G4int GetNumPhotons() const       { return m_numPhotons; };
    G4double GetEnergy() const        { return m_energy; };
    G4double GetStartTime() const     { return m_startTime; };
    G4double GetEndTime() const       { return m_endTime; };
    G4double GetTime() const          { return (m_startTime + m_endTime)/2.; };
    G4ThreeVector GetStartPosition() const { return m_startPosition; };
    G4ThreeVector GetEndPosition() const { return m_endPosition; };
    G4ThreeVector GetPosition() const    { return (m_startPosition + m_endPosition)/2.; };
    G4int GetIdentifier() const       { return m_identifier; };
  
  private:
  
    G4int         m_trackID; 
    G4int         m_pdgCode;
    G4int         m_numPhotons;
    G4double      m_energy;        ///< units MeV
    G4double      m_startTime;     ///< units ns
    G4double      m_endTime;       ///< units ns
    G4ThreeVector m_startPosition; ///< units mm
    G4ThreeVector m_endPosition;   ///< units mm
    G4int         m_identifier;    ///< cell ID
  };

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  typedef G4THitsCollection<LArHit> LArHitsCollection;

  extern G4ThreadLocal G4Allocator<LArHit>* LArHitAllocator;

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  inline void* LArHit::operator new(size_t)
  {
    if(!LArHitAllocator)
      LArHitAllocator = new G4Allocator<LArHit>;
    return (void *) LArHitAllocator->MallocSingle();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  inline void LArHit::operator delete(void *hit)
  {
    LArHitAllocator->FreeSingle((LArHit*) hit);
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

} // namespace gramsg4

#endif
