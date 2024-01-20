//
// NumiStackingAction.hh
//

#ifndef NumiStackingAction_H
#define NumiStackingAction_H 1

#include "globals.hh"
#include "G4UserStackingAction.hh"
//#include "G4ThreeVector.hh"

class G4Track;
class NumiDataInput;
class NumiRunManager;

class NumiStackingAction : public G4UserStackingAction
{
  public:
    NumiStackingAction();
    virtual ~NumiStackingAction();

    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    virtual void NewStage();
    virtual void PrepareNewEvent();

  private:
  NumiDataInput * NumiData;
   NumiRunManager * pRunManager; 
};

#endif

