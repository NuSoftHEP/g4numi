//
// NumiTrackingAction.hh
//
#ifndef NumiTrackingAction_h
#define NumiTrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "NumiTrajectory.hh"

class NumiDataInput;
class NumiRunManager;
class NumiPrimaryGeneratorAction;
class G4EventManager;
class NumiEventAction;

class NumiTrackingAction : public G4UserTrackingAction
{
  public:
    NumiTrackingAction();
    virtual ~NumiTrackingAction();

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

  private:
   NumiDataInput *ND;
   NumiRunManager *pRunManager;
   NumiPrimaryGeneratorAction *NPGA;
   G4EventManager *EvtManager;
   NumiEventAction *NumiEvtAct;
};

#endif
