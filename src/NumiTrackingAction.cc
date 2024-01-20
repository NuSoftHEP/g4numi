//----------------------------------------------------------------------
// NumiTrackingAction.cc
// $Id: NumiTrackingAction.cc,v 1.8.4.7 2014/09/16 16:21:41 laliaga Exp $
//----------------------------------------------------------------------

#include "NumiTrackInformation.hh"
#include "NumiTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4Trajectory.hh"
#include "NumiRunManager.hh"
#include "NumiRunAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "NumiAnalysis.hh"
#include "NumiTrajectory.hh"
#include "NumiDataInput.hh"
#include "NumiPrimaryGeneratorAction.hh"
#include "G4EventManager.hh"
#include "NumiEventAction.hh"

NumiTrackingAction::NumiTrackingAction()
{
  pRunManager=(NumiRunManager*)NumiRunManager::GetRunManager();
  NPGA=(NumiPrimaryGeneratorAction*)pRunManager->GetUserPrimaryGeneratorAction();
  ND=NumiDataInput::GetNumiDataInput();

  if(ND->GetDebugLevel() > 0)
  {
     std::cout << "NumiTrackingAction Constructor Called." << std::endl;
  }
}

NumiTrackingAction::~NumiTrackingAction()
{
   if(ND->GetDebugLevel() > 0)
   {
      std::cout << "NumiTrackingAction Destructor Called." << std::endl;
   }
}

void NumiTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
        //Store tracks in trajectory container
    fpTrackingManager->SetStoreTrajectory(true);
    fpTrackingManager->SetTrajectory(new NumiTrajectory(aTrack));
    
   if(ND->GetDebugLevel() > 1)
   {
      G4int evtno = pRunManager->GetCurrentEvent()->GetEventID();
      std::cout << "Event " << evtno << ": NumiTrackingAction::PreUserTrackingAction() Called." << std::endl;
      if(ND->GetDebugLevel() > 2)
      {
	 NumiTrackInformation* trackinfo=new NumiTrackInformation(); 
	 trackinfo -> Print(aTrack);
	 delete trackinfo;
      }
   }
      


  //set tgen (and weight for fluka nad mars input) 
   if (aTrack->GetTrackID()==1) 
   {   
       NumiTrackInformation* info = new NumiTrackInformation();
       if (ND->useFlukaInput||ND->useMarsInput){
           info->SetTgen(NPGA->GetTgen());
           info->SetNImpWt(NPGA->GetWeight());
       }
       else{
           info->SetTgen(1);
           info->SetNImpWt(1.);
       }
       G4Track* theTrack = (G4Track*)aTrack;
       theTrack->SetUserInformation(info);
   }
  
  
  //if a particle is a neutrino then analyse and store in ntuple
  G4ParticleDefinition * particleDefinition = aTrack->GetDefinition();
  if ((particleDefinition == G4NeutrinoE::NeutrinoEDefinition())||
      (particleDefinition == G4NeutrinoMu::NeutrinoMuDefinition()) ||
      (particleDefinition == G4NeutrinoTau::NeutrinoTauDefinition()) ||
      (particleDefinition == G4AntiNeutrinoE::AntiNeutrinoEDefinition()) ||
      (particleDefinition == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()) ||
      (particleDefinition == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()))
    {
        
        const G4Event* event = pRunManager->GetCurrentEvent();
        G4TrajectoryContainer* trajectories = event->GetTrajectoryContainer();
            // Make a temporary map to make it easier to go up the trajectory stack
        std::map<int,G4VTrajectory*> trajectoryMap;
        for (std::size_t i = 0; i < trajectories->size(); ++i) {
            G4VTrajectory* traj = (*trajectories)[i];
            trajectoryMap[traj->GetTrackID()] = traj;
        }

        std::vector<G4VTrajectory*> nuHistory;
        G4VTrajectory* neutrino = fpTrackingManager->GimmeTrajectory();
        nuHistory.push_back(neutrino);
        int parentId = aTrack->GetParentID();
        while (parentId  != 0) {
            G4VTrajectory* parentTraj = trajectoryMap[parentId];
            if (!parentTraj) {
                G4cerr << "Invalid trajectory object" << G4endl;
                continue;
            }
            nuHistory.push_back(parentTraj);
            NumiTrajectory* numiTrajectory = dynamic_cast<NumiTrajectory*>(parentTraj);
            if (!numiTrajectory) continue;
            G4String startVol = numiTrajectory->GetPreStepVolumeName(0);
            G4String stopVol = numiTrajectory->GetPreStepVolumeName(numiTrajectory->GetPointEntries()-1);
            parentId = parentTraj->GetParentID();
        }
        
      NumiAnalysis* analysis = NumiAnalysis::getInstance();
      analysis->FillNeutrinoNtuple(*aTrack,nuHistory);
    }


  
  if (ND->useMuonBeam && ND->GetSimDRays())
  {
     EvtManager = G4EventManager::GetEventManager();
     NumiEvtAct = (NumiEventAction*)(EvtManager -> GetUserEventAction());
     NumiEvtAct -> AddTrack(aTrack);
  }

  
}

void NumiTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
   if(ND->GetDebugLevel() > 1)
   {
      G4int evtno = pRunManager->GetCurrentEvent()->GetEventID();
      std::cout << "Event " << evtno << ": NumiTrackingAction::PostUserTrackingAction() Called." << std::endl;
      if(ND->GetDebugLevel() > 2)
      {
	 NumiTrackInformation* trackinfo=new NumiTrackInformation(); 
	 trackinfo -> Print(aTrack);
	 delete trackinfo;
      }
   }

   
  // set tgen(secondary) = tgen(parent)+1
  NumiTrackInformation* info = (NumiTrackInformation*)(aTrack->GetUserInformation());
    if (info!=0) {
       G4int tgen = info->GetTgen();
       G4double nimpwt = info->GetNImpWt();
       G4TrackVector* SecVector=fpTrackingManager->GimmeSecondaries();
       for (size_t ii=0;ii<(*SecVector).size();ii++){
           G4Track* secTrack  = (*SecVector)[ii];
               // Check if the track information object already exists. Remember that
               // track information objects are also created for secondaries at the end
               // of UserSteppingAction() method. If yes, access the existing info object,
               // then set the data members. Otherwise, create new track information object.
           if (!secTrack->GetUserInformation()) {
               NumiTrackInformation* newinfo = new NumiTrackInformation();
               newinfo->SetTgen(tgen+1); // set generation of daughter particles
               newinfo->SetNImpWt(nimpwt); // set weight of the new track equal to parent weight
               (*SecVector)[ii]->SetUserInformation(newinfo);
           } else {
               NumiTrackInformation* trackInformation
                   = dynamic_cast<NumiTrackInformation*>(secTrack->GetUserInformation());
               trackInformation->SetTgen(tgen+1); // set generation of daughter particles
               trackInformation->SetNImpWt(nimpwt); // set weight of the new track equal to parent weight
           }
           
       }

    }

}






