//----------------------------------------------------------------------
// NumiStackingAction.cc
// $Id: NumiStackingAction.cc,v 1.8.4.3 2014/01/22 22:31:07 kordosky Exp $
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
#include "NumiRunManager.hh"

NumiStackingAction::NumiStackingAction()
{ 
  NumiData = NumiDataInput::GetNumiDataInput();
  pRunManager=(NumiRunManager*)NumiRunManager::GetRunManager();

  if(NumiData->GetDebugLevel() > 0)
  {
     std::cout << "NumiStackingAction Constructor Called." << std::endl;
  }
}

NumiStackingAction::~NumiStackingAction()
{

   if(NumiData->GetDebugLevel() > 0)
   {
      std::cout << "NumiStackingAction Destructor Called." << std::endl;
   }
}

G4ClassificationOfNewTrack NumiStackingAction::ClassifyNewTrack(const G4Track * aTrack)
{
   
  G4ClassificationOfNewTrack classification = fUrgent;
  G4ParticleDefinition * particleType = aTrack->GetDefinition();

  if(NumiData->GetDebugLevel() > 2)
  {
     G4int evtno = pRunManager->GetCurrentEvent()->GetEventID();
     std::cout << "Event " << evtno << ": NumiStackingAction::ClassifyNewTrack for Particle " 
	       << "\"" << particleType->GetParticleName() << "\"" << " Called." << std::endl;
  }

  
  if( NumiData->useMuonBeam &&
      particleType != G4MuonPlus::MuonPlusDefinition() &&
      particleType != G4MuonMinus::MuonMinusDefinition() &&
      particleType != G4Gamma::GammaDefinition() &&
      particleType != G4Electron::ElectronDefinition() &&
      particleType != G4Positron::PositronDefinition() &&
      classification != fKill )
  {
     classification = fKill;
  }
  
  // Discard Gammas, Electrons, ...
  if ( !(NumiData->GetSimDRays()) &&
       ( particleType==G4Gamma::GammaDefinition() ||
         particleType==G4Electron::ElectronDefinition() ||
         particleType==G4Positron::PositronDefinition() ) &&
       classification != fKill )
  {
     classification = fKill;
  }

  //Discard particles with pz<0
  G4ThreeVector momentum=aTrack->GetMomentumDirection();
  if ( !(NumiData->GetSimDRays()) && momentum[2]<0 && (classification != fKill) )  
  {
     classification = fKill;
  }

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
       {
          classification = fKill;
       } 
    }
  
  //check if track is inside world (some neutral particles make huge jumps from horns (field part) and
  // end up decaying in ~infinity which occasionaly causes g4numi to crash
  G4ThreeVector position=aTrack->GetPosition();
  if ((classification != fKill)&&
      ((sqrt(position[0]*position[0]+position[1]*position[1])>NumiData->RockRadius)||
       position[2]>NumiData->RockHalfLen))
  {
     if (NumiData->IsDebugOn())
     {
        G4cout <<"NumiStackingAction: Killing Out of World Particle" <<G4endl;
        G4cout << "   Particle type: "<<aTrack->GetDefinition()->GetParticleName()
               << " ; Parent ID: "<<aTrack->GetParentID()
               << " ;  Kinetic Energy = "<<aTrack->GetKineticEnergy()/GeV<<" GeV"<<G4endl;
        G4cout << "   Position (m) = "<<position/m<<G4endl;
     }
     
     classification =fKill;
  }
  
  //If importance weighting is on:
  if (NumiData->NImpWeightOn && (classification != fKill))
  {
    NumiTrackInformation* trackInfo=(NumiTrackInformation*)(aTrack->GetUserInformation());  
    if (trackInfo!=0) 
    {
       G4double Nimpweight=NumiImpWeight::CalculateImpWeight(aTrack);
       if(Nimpweight==0.)
       {
          classification = fKill;
       }
       else
       {
	  trackInfo->SetNImpWt(Nimpweight);
	  G4double Sumnimpwt2 = NumiImpWeight::CalculateSumImpWeight2(aTrack);
	  trackInfo->SetSumNImpWt2(Sumnimpwt2);
       }
    }
  }

  if (NumiData->useMuonBeam && NumiData->GetSimDRays() && NumiData->GetUseZPosCut())
  {
     if( (particleType==G4Gamma::GammaDefinition() ||
          particleType==G4Electron::ElectronDefinition() ||
          particleType==G4Positron::PositronDefinition()) &&
         aTrack->GetParentID() == 1 &&
         classification != fKill )
     {
        G4double zpos = aTrack->GetPosition()[2];
        if( zpos < 727000.0 ||
            (zpos > 741000.0 && zpos < 748000.0) ||
            (zpos > 757000.0 && zpos < 768000.0) ||
            zpos > 777000.0 )
        {
           classification = fKill;
        }
     }
  }
  
  return classification;
}

void NumiStackingAction::NewStage() 
{
   if(NumiData->GetDebugLevel() > 1)
   {
      G4int evtno = pRunManager->GetCurrentEvent()->GetEventID();
      std::cout << "Event " << evtno << ": NumiStackingAction::NewStage Called." << std::endl;
   }


  // stackManager->ReClassify();
  //  return;
}
  
void NumiStackingAction::PrepareNewEvent()
{
   if(NumiData->GetDebugLevel() > 1)
   {
      G4int evtno = pRunManager->GetCurrentEvent()->GetEventID();
      std::cout << "Event " << evtno << ": NumiStackingAction::PrepareNewEvent Called." << std::endl;
   } 

}


