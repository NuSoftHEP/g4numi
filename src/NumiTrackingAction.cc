//----------------------------------------------------------------------
// NumiTrackingAction.cc
// $Id: NumiTrackingAction.cc,v 1.8.2.3 2009/09/24 16:37:35 martens Exp $
//----------------------------------------------------------------------

#include "NumiTrackInformation.hh"
#include "NumiTrackingAction.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4Trajectory.hh"
#include "NumiRunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "NumiAnalysis.hh"
#include "NumiTrajectory.hh"
#include "NumiDataInput.hh"
#include "NumiPrimaryGeneratorAction.hh"

NumiTrackingAction::NumiTrackingAction()
{
  pRunManager=(NumiRunManager*)NumiRunManager::GetRunManager();
  NPGA=(NumiPrimaryGeneratorAction*)pRunManager->GetUserPrimaryGeneratorAction();
  ND=NumiDataInput::GetNumiDataInput();
}

NumiTrackingAction::~NumiTrackingAction()
{;}

void NumiTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
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
  
  
  //Store tracks in trajectory container
  fpTrackingManager->SetStoreTrajectory(true);
  fpTrackingManager->SetTrajectory(new NumiTrajectory(aTrack));
   
  //if a particle is a neutrino then analyse and store in ntuple
  G4ParticleDefinition * particleDefinition = aTrack->GetDefinition();
  if ((particleDefinition == G4NeutrinoE::NeutrinoEDefinition())||
      (particleDefinition == G4NeutrinoMu::NeutrinoMuDefinition()) ||
      (particleDefinition == G4NeutrinoTau::NeutrinoTauDefinition()) ||
      (particleDefinition == G4AntiNeutrinoE::AntiNeutrinoEDefinition()) ||
      (particleDefinition == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()) ||
      (particleDefinition == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()))
    {
      NumiAnalysis* analysis = NumiAnalysis::getInstance();
      analysis->FillNeutrinoNtuple(*aTrack);
    }
}

void NumiTrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  // Jtest
  //---
  NumiAnalysis* analysis = NumiAnalysis::getInstance();
  analysis->WriteHadmmNtuple(aTrack);
  
  //---


  // set tgen(secondary) = tgen(parent)+1
  NumiTrackInformation* info = (NumiTrackInformation*)(aTrack->GetUserInformation());
    if (info!=0) {
       G4int tgen = info->GetTgen();
       G4double nimpwt = info->GetNImpWt();
       G4TrackVector* SecVector=fpTrackingManager->GimmeSecondaries();
       for (size_t ii=0;ii<(*SecVector).size();ii++){
	 NumiTrackInformation* newinfo = new NumiTrackInformation();
	 newinfo->SetTgen(tgen+1); // set generation of daughter particles
	 newinfo->SetNImpWt(nimpwt); // set weight of the new track equal to parent weight
	 (*SecVector)[ii]->SetUserInformation(newinfo);
       }
    }
}






