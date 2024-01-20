
#include "NA49PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "NA49Analysis.hh"
#include <time.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NA49PrimaryGeneratorAction::NA49PrimaryGeneratorAction()
{
  particleGun  = new G4ParticleGun(1);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NA49PrimaryGeneratorAction::~NA49PrimaryGeneratorAction()
{
  delete particleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void NA49PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4double zVertex = -6.5*mm;
  particleGun->SetParticlePosition(G4ThreeVector(0.,0.,zVertex));
  particleGun->GeneratePrimaryVertex(anEvent);

  NA49Analysis* analysis = NA49Analysis::getInstance();
  Double_t ener = particleGun->GetParticleEnergy();
  G4ParticleDefinition* part  = particleGun->GetParticleDefinition();

  analysis->GetPrimGenInfo(ener,part);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
