
#ifndef NA49EventActionMessenger_h
#define NA49EventActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class NA49EventAction;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class NA49EventActionMessenger: public G4UImessenger
{
public:

  NA49EventActionMessenger(NA49EventAction*);
  virtual ~NA49EventActionMessenger();
    
  void SetNewValue(G4UIcommand*, G4String);
    
private:

  NA49EventAction*          eventAction;   
  G4UIcmdWithAString*   drawCmd;
  G4UIcmdWithAnInteger* printCmd;    
  G4UIcmdWithAnInteger* dCmd;    

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
