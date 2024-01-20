
#ifndef NA49EventAction_h
#define NA49EventAction_h 1
 
#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>
#include "TrackInfo_t.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class G4Event;
class G4Track;
class NA49EventActionMessenger;
class G4UImanager;

class NA49EventAction : public G4UserEventAction
{
public: // Without description

  NA49EventAction();
  virtual ~NA49EventAction();

  void BeginOfEventAction(const G4Event*);
  void   EndOfEventAction(const G4Event*);

  void SetPrintModulo(G4int val)   {printModulo = val;};
  void SetDrawFlag(G4String val)   {drawFlag = val;};
  void AddEventToDebug(G4int val)  {selectedEvents.push_back(val);
                                    nSelected++;};
  void AddTrack(const G4Track* aTrack);


private:

  NA49EventActionMessenger* eventMessenger;
  G4UImanager*          UI;
  std::vector<G4int>    selectedEvents;

  G4int        printModulo;
  G4int        nSelected;

  // drawFlags = all, charged, neutral, charged+n
  G4String     drawFlag;
  G4bool       debugStarted;
  std::vector<TrackInfo_t> TrackInfoVec;
  
};

#endif


