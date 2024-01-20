//
//NumiEventAction.hh
//

#ifndef NumiEventAction_h
#define NumiEventAction_h 1

//C++
#include <string>
#include <vector>
#include <map>

#include "G4UserEventAction.hh"
#include "globals.hh"


class G4Event;
class G4Track;
class G4TrajectoryContainer;
class NumiDataInput;

class NumiEventAction : public G4UserEventAction
{
   
public:
   NumiEventAction();
   ~NumiEventAction();
   
   void BeginOfEventAction(const G4Event*);
   void EndOfEventAction(const G4Event*);

   void AddTrack(const G4Track* aTrack);
   G4int GetIntExt(const G4Track* aTrack);
   G4double GetZPos(const G4Track* aTrack);

   G4TrajectoryContainer* GetTrajectoryContainer();

   const G4int GetDRayTrackID() const;
   void IncrementDRayTrackID();

private:

   typedef std::map<int, int> IIMap;
   typedef std::map<int, double> IDMap;

   G4int GetIntExt(const G4String &volName);
 
private:

   NumiDataInput* NumiData;
   G4TrajectoryContainer* fTrajectoryContainer;

   IIMap fTrackMap;
   IDMap fZPosMap;

   G4int fSplitDRayIDcounter;
   
};

inline const G4int NumiEventAction::GetDRayTrackID() const {return fSplitDRayIDcounter;}
inline void NumiEventAction::IncrementDRayTrackID() {++fSplitDRayIDcounter;}

#endif

    
