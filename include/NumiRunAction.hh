//
// NumiRunAction.hh
//

#ifndef NumiRunAction_h
#define NumiRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "NumiTrajectory.hh"

class G4Run;
class NumiDataInput;
class NumiRunActionMessenger;

class NumiRunAction : public G4UserRunAction
{
public:
  NumiRunAction();
  ~NumiRunAction();

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  
private:

   NumiDataInput* fNDI;
   NumiRunActionMessenger* runMessenger;
};

#endif

