
#ifndef NA49PhysicsListMessenger_h
#define NA49PhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class NA49PhysicsList;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NA49PhysicsListMessenger: public G4UImessenger
{
public:
  
  NA49PhysicsListMessenger(NA49PhysicsList* );
  virtual ~NA49PhysicsListMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:
  
  NA49PhysicsList* pPhysicsList;
    
  G4UIcmdWithADoubleAndUnit* gammaCutCmd;
  G4UIcmdWithADoubleAndUnit* electCutCmd;
  G4UIcmdWithADoubleAndUnit* posCutCmd;
  G4UIcmdWithADoubleAndUnit* allCutCmd;
  G4UIcmdWithAString*        pListCmd;
  G4UIcmdWithoutParameter*   listCmd;  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

