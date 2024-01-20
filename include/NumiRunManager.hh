//
// NumiRunManager.hh
//

#ifndef NumiRunManager_h
#define NumiRunManager_h 1

#include "G4RunManager.hh"

class NumiDataInput;
class NumiPrimaryGeneratorAction;

class NumiRunManager : public G4RunManager
{
 public:
  NumiRunManager();
  virtual ~NumiRunManager();

  virtual void BeamOn(G4int n_event,const char* macroFile=0,G4int n_select=-1);
  inline G4int GetNumberOfEvents(){
    return numberOfEventToBeProcessed;
  }

  G4int nEvents;

 protected:
   NumiDataInput* NumiData;
   
   NumiPrimaryGeneratorAction * primaryGeneratorAction;

};

#endif
