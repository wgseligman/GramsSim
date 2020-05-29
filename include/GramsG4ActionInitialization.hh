/// \file GramsG4ActionInitialization.hh
/// \brief Definition of the GramsG4ActionInitialization class

#ifndef GramsG4ActionInitialization_h
#define GramsG4ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class GramsG4ActionInitialization : public G4VUserActionInitialization
{
  public:
    GramsG4ActionInitialization();
    virtual ~GramsG4ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
