
#ifndef NA49PrimaryGeneratorAction_h
#define NA49PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NA49PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  NA49PrimaryGeneratorAction();
  virtual ~NA49PrimaryGeneratorAction();

  void GeneratePrimaries(G4Event*);
  G4ParticleGun* GetParticleGun() {return particleGun;};

private:

  NA49PrimaryGeneratorAction & operator=(const NA49PrimaryGeneratorAction &right);
  NA49PrimaryGeneratorAction(const NA49PrimaryGeneratorAction&);

  G4ParticleGun*   particleGun;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


