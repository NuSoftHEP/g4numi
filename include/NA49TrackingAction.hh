#ifndef NA49TrackingAction_h
#define NA49TrackingAction_h

#include "globals.hh"
#include "G4UserTrackingAction.hh"

class G4Track;

class NA49TrackingAction : public G4UserTrackingAction {
public:
  NA49TrackingAction();
  virtual ~NA49TrackingAction();

  virtual void PostUserTrackingAction(const G4Track*);

};

#endif
