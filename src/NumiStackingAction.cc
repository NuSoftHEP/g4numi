//----------------------------------------------------------------------
// NumiStackingAction.cc
// $Id: NumiStackingAction.cc,v 1.8.2.1 2009/09/24 16:39:13 martens Exp $
//----------------------------------------------------------------------

#include "NumiStackingAction.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4ios.hh"
#include "NumiImpWeight.hh"
#include "NumiTrackInformation.hh"
#include "NumiDataInput.hh"
#include "NumiTrajectory.hh"
#include "NumiAnalysis.hh"

NumiStackingAction::NumiStackingAction()
{ 
  NumiData = NumiDataInput::GetNumiDataInput();
}

NumiStackingAction::~NumiStackingAction()
{
}

G4ClassificationOfNewTrack 
NumiStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
  G4ClassificationOfNewTrack classification = fUrgent;
  G4ParticleDefinition * particleType = aTrack->GetDefinition();

  // Discard Gammas, Electrons, ...
  if ((particleType==G4Gamma::GammaDefinition()) ||
      (particleType==G4Electron::ElectronDefinition()) ||
      (particleType==G4Positron::PositronDefinition()) &&
      (classification != fKill))
    {classification = fKill;}

  //Discard particles with pz<0
  G4ThreeVector momentum=aTrack->GetMomentumDirection();
  if (momentum[2]<0 && (classification != fKill)) 
    {classification = fKill;}
  
  //Discard particles with kinetic energy < 1.GeV (that are not neutrinos)
  if ((particleType!=G4NeutrinoE::NeutrinoEDefinition())&&
      (particleType!=G4NeutrinoMu::NeutrinoMuDefinition())&&
      (particleType!=G4NeutrinoTau::NeutrinoTauDefinition())&&
      (particleType!=G4AntiNeutrinoE::AntiNeutrinoEDefinition())&&
      (particleType!=G4AntiNeutrinoMu::AntiNeutrinoMuDefinition())&&
      (particleType!=G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()))
    {
      G4double energy = aTrack->GetKineticEnergy();
      if (( NumiData->GetKillTracking() && energy < NumiData->GetKillTrackingThreshold() ) &&
	  (classification != fKill))
	{classification = fKill;} 
    }
  
  //check if track is inside world (some neutral particles make huge jumps from horns (field part) and
  // end up decaying in ~infinity which occasionaly causes g4numi to crash
  G4ThreeVector position=aTrack->GetPosition();
  if ((classification != fKill)&&
      ((sqrt(position[0]*position[0]+position[1]*position[1])>NumiData->RockRadius)||
       position[2]>NumiData->RockHalfLen)){
    if (NumiData->IsDebugOn()){
      G4cout <<"NumiStackingAction: Killing Out of World Particle" <<G4endl;
      G4cout << "   Particle type: "<<aTrack->GetDefinition()->GetParticleName()
	     << " ; Parent ID: "<<aTrack->GetParentID()
	     << " ;  Kinetic Energy = "<<aTrack->GetKineticEnergy()/GeV<<" GeV"<<G4endl;
      G4cout << "   Position (m) = "<<position/m<<G4endl;
    }
    classification =fKill;
  }

  //If importance weighting is on:
  if (NumiData->NImpWeightOn && (classification != fKill)){
    NumiTrackInformation* trackInfo=(NumiTrackInformation*)(aTrack->GetUserInformation());  
    if (trackInfo!=0) 
      {
	G4double Nimpweight=NumiImpWeight::CalculateImpWeight(aTrack);
	if(Nimpweight==0.) {
	    classification = fKill;
	} else {
	  trackInfo->SetNImpWt(Nimpweight);
	}
      }
  }
  return classification;
}

void NumiStackingAction::NewStage() 
{
  // stackManager->ReClassify();
  //  return;
}
  
void NumiStackingAction::PrepareNewEvent()
{ 
}


