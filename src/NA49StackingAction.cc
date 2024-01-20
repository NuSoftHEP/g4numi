
#include "NA49StackingAction.hh"
#include "NA49Analysis.hh"
#include "NA49TrackInfo.hh"
#include "G4Track.hh"
#include "G4HadronicProcessStore.hh"
#include "G4NistManager.hh"
#include "NA49EventAction.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include <cassert>
#include <iostream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NA49StackingAction::NA49StackingAction()
{
  killSecondary  = false;
  pname          = ""; 
  elm            = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NA49StackingAction::~NA49StackingAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack
NA49StackingAction::ClassifyNewTrack(const G4Track* aTrack)
{
  // when is this called with respect to UserTrackingAction
 G4ClassificationOfNewTrack status = fUrgent;
 EvtManager = G4EventManager::GetEventManager();
 NA49EvtAct = (NA49EventAction*)(EvtManager -> GetUserEventAction());
 // const G4ParticleDefinition* pd = aTrack->GetDefinition();
 if (aTrack->GetTrackStatus() == fAlive) {
   NA49TrackInfo* info = 
     dynamic_cast<NA49TrackInfo*>(aTrack->GetUserInformation());
   //   assert(info);
   if(!info &&  aTrack->GetParentID()==0) { // do nothing 
   }
   else if(!info && aTrack->GetParentID()!=0){
     // this is a problem
     std::cout<<"!info TrackID:ParentID:Name = "<<
       aTrack->GetTrackID()<<":"<<
       aTrack->GetParentID()<<":"<<
       aTrack->GetDefinition()->GetParticleName()<<std::endl;
     abort();
   }
   else{
     // OK, we want to save any track that has primary_chain and !fast_decay
     // the track info is filled in NA49TrackingAction
     if(info->primary_chain==true && info->fast_decay==false){
       NA49EvtAct->AddTrack(aTrack);
#ifdef DEBUG
       std::cout<<"Storing "<< aTrack->GetDefinition()->GetParticleName()
		<<"("<<aTrack->GetTrackID()<<")"<<std::endl;
#endif
     }
   }
 }

  //stack or delete secondaries
 if (killSecondary)      status = fKill;

  return status;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
