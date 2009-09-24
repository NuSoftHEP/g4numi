//----------------------------------------------------------------------
// NumiSteppingAction.cc
// $Id: NumiSteppingAction.cc,v 1.16.2.4 2009/09/24 16:43:38 martens Exp $
//----------------------------------------------------------------------

#include "NumiSteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "NumiTrajectory.hh"
#include "G4TrajectoryContainer.hh"
#include "G4RunManager.hh"
#include "NumiTrackInformation.hh"
#include "NumiDataInput.hh"
#include "NumiAnalysis.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Event.hh"
#include "NumiTrackInformation.hh"
#include "NumiAnalysis.hh"

NumiSteppingAction::NumiSteppingAction()
{
  NDI = NumiDataInput::GetNumiDataInput();

  
}

NumiSteppingAction::~NumiSteppingAction()
{
}


void NumiSteppingAction::UserSteppingAction(const G4Step * theStep)
{

  NumiAnalysis* analysis = NumiAnalysis::getInstance();
  analysis->FillBXDRAW(theStep);

  // Check if the Pi+, Pi-, K+, K-, K0L, mu+ or mu- decayed and set Ndecay code:
  // 1  K0L -> nu_e pi- e+
  // 2  K0L -> anti_nu_e pi+ e-
  // 3  K0L -> nu_mu pi- mu+
  // 4  K0L -> anti_nu_mu pi+ mu-
  // 5  K+  -> nu_mu mu+
  // 6  K+  -> nu_e pi0 e+
  // 7  K+  -> nu_mu pi0 mu+
  // 8  K-  -> anti_nu_mu mu-
  // 9  K-  -> anti_nu_e pi0 e-
  // 10 K-  -> anti_nu_mu pi0 mu-
  // 11 mu+ -> anti_nu_mu nu_e e+
  // 12 mu- -> nu_mu anti_nu_e e-
  // 13 pi+ -> nu_mu mu+
  // 14 pi- -> anti_nu_mu mu-

  G4Track * theTrack = theStep->GetTrack();
  G4ParticleDefinition * particleDefinition = theTrack->GetDefinition();
  if (!(NDI->useMuonBeam) && (NDI->GetKillTracking() && theTrack->GetKineticEnergy() < NDI->GetKillTrackingThreshold() ) &&
      (particleDefinition != G4NeutrinoE::NeutrinoEDefinition())&&
      (particleDefinition != G4NeutrinoMu::NeutrinoMuDefinition()) &&
      (particleDefinition != G4NeutrinoTau::NeutrinoTauDefinition()) &&
      (particleDefinition != G4AntiNeutrinoE::AntiNeutrinoEDefinition()) &&
      (particleDefinition != G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()) &&
      (particleDefinition != G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()))
    {
      theTrack->SetTrackStatus(fStopAndKill);
    }


  if(NDI->useMuonBeam)
  {
     
     if ( (NDI->GetKillTracking() && theTrack->GetKineticEnergy() < NDI->GetKillTrackingThreshold()) )
     {
	theTrack->SetTrackStatus(fStopAndKill);
     }
     
     if( particleDefinition != G4MuonPlus::MuonPlusDefinition()
         && particleDefinition != G4MuonMinus::MuonMinusDefinition() )      
     {
	//NumiAnalysis* analysis = NumiAnalysis::getInstance();
	//G4cout << "Event " << analysis->GetEntry() << ", Process = "
        //<< theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
	//G4cout<< "Event " << analysis->GetEntry() << ", KE = " << theTrack->GetKineticEnergy()
        //<< ", threshold = " << NDI->GetKillTrackingThreshold()  << ", type = "
        //<< particleDefinition << ", zpos = " << theTrack->GetPosition()[2] << G4endl; 
	
	theTrack->SetTrackStatus(fStopAndKill);
     }
     else if (theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL 
              && theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()=="Decay")
     {
        //NumiAnalysis* analysis = NumiAnalysis::getInstance();
	
        //G4cout<< "Event " << analysis->GetEntry() << ": Particle decayed" << ", zpos = "
        //<< theTrack->GetPosition()[2] << G4endl; 
	theTrack->SetTrackStatus(fStopAndKill);
     }
     
     if(theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="Transportation" &&
        theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="muIoni" &&
        theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="muBrems" &&
        theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="msc" &&
        theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="muPairProd")
     {
	//G4cout << "Process = " << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
     }

     
     //To see if the particle has just past the point that is 0.5m upstream into the rock before
     //monitor. Record the particle momentum in order to apply delta ray corrections. --LL
     //
     //for alc0; info will only be recorded the first time this executes
     //
     if( (particleDefinition == G4MuonPlus::MuonPlusDefinition()
          || particleDefinition == G4MuonMinus::MuonMinusDefinition())
         && theTrack->GetPosition()[2] > 729811.) 
     {
	NumiAnalysis* analysis = NumiAnalysis::getInstance();
	analysis->FillAlcEdepInfo(*theTrack, 0);
     }
     //
     //for alc1; info will only be recorded the first time this executes
     //
     if( (particleDefinition == G4MuonPlus::MuonPlusDefinition()
          || particleDefinition == G4MuonMinus::MuonMinusDefinition())
         && theTrack->GetPosition()[2] > 750941.) 
     {
	NumiAnalysis* analysis = NumiAnalysis::getInstance();
	analysis->FillAlcEdepInfo(*theTrack, 1);
     }
     //
     //for alc2; info will only be recorded the first time this executes
     //
     if( (particleDefinition == G4MuonPlus::MuonPlusDefinition()
          || particleDefinition == G4MuonMinus::MuonMinusDefinition())
         && theTrack->GetPosition()[2] > 772062.) 
     {
	NumiAnalysis* analysis = NumiAnalysis::getInstance();
	analysis->FillAlcEdepInfo(*theTrack, 2);
     }

     // Checks to see whether the particle has entered the Hadron
     // or muon monitors, and if so calls the NumiAnalysis class
     // to record the particle properties through the monitors.-DJK
     if (NDI->createHadmmNtuple && theStep->GetPostStepPoint()->GetPhysicalVolume()!=NULL)
     {
       //    G4cout << theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() << G4endl;
       
       if(theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "PVHadMon"
	  && theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "ConcShield"
	  && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
	      || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
	 {
           //NumiAnalysis* analysis = NumiAnalysis::getInstance();
	   
           //analysis->FillHadmmNtuple(*theTrack, 4, 0);
	 }
       else if( theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "MuCell"
		&& theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "MuMon_0"
		&& (particleDefinition == G4MuonPlus::MuonPlusDefinition()
		    || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
	 {
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack, 
                                     0, 
                                     theStep->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo() );
	 }
       else if( theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="MuCell"
		&& theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "MuMon_1"
		&& (particleDefinition == G4MuonPlus::MuonPlusDefinition()
		    || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
	 {  
           NumiAnalysis* analysis = NumiAnalysis::getInstance();
           analysis->FillHadmmNtuple(*theTrack,
                                     1,
                                     theStep->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo() );
	 }
	else if( theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()=="MuCell"
		 && theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "MuMon_2"
		 && (particleDefinition == G4MuonPlus::MuonPlusDefinition()
                     || particleDefinition == G4MuonMinus::MuonMinusDefinition()) )
	  {  
	    NumiAnalysis* analysis = NumiAnalysis::getInstance();
	    analysis->FillHadmmNtuple(*theTrack,
				      2,
				      theStep->GetPostStepPoint()->GetPhysicalVolume()->GetCopyNo() );
	  }
       
       else if((theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName() == "ROCK"
		&& theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() == "MuMonAlcvShot_2_Down"))
	 {
           theTrack->SetTrackStatus(fStopAndKill);   
	 }
     }
     
  }//end if(NDI->useMuonBeam)
  

    //================================
    //=======for Raytracing===========
    //--------------------------------
    if(NDI->raytracing || NDI->createZpNtuple){
       for(G4int in = 0; in<(NDI->NZpoint);in++)//---Jasmine Added
       {
          if((theStep->GetPostStepPoint()->GetPosition()[2]>=NDI->Zpoint[in]&&
              theStep->GetPreStepPoint()->GetPosition()[2]<NDI->Zpoint[in]))
          {
             NumiAnalysis* analysis=NumiAnalysis::getInstance();
             analysis->FillZpNtuple(*theTrack,in);
          }
       }
    }
    
    
    if (theStep->GetPostStepPoint()->GetProcessDefinedStep() != NULL){
      G4int decay_code=0;
      if (theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()=="Decay"){
	G4int nSecAtRest = fpSteppingManager->GetfN2ndariesAtRestDoIt();
	G4int nSecAlong  = fpSteppingManager->GetfN2ndariesAlongStepDoIt();
	G4int nSecPost   = fpSteppingManager->GetfN2ndariesPostStepDoIt();
	G4int nSecTotal  = nSecAtRest+nSecAlong+nSecPost;
	G4TrackVector* secVec = fpSteppingManager->GetfSecondary();
	
	if (particleDefinition==G4PionPlus::PionPlusDefinition()) {
	  for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
	    {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_mu")
		decay_code=13;
	    }
	}
	if (particleDefinition==G4PionMinus::PionMinusDefinition()) {
	  for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
	    {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_mu")
		decay_code=14;
	    }
	}
	if (particleDefinition==G4KaonPlus::KaonPlusDefinition()) {
	  for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
	    {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_mu")
		{if (nSecTotal==2) decay_code=5;
		if (nSecTotal==3) decay_code=7;}
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_e")
		decay_code=6;
	    }
	}
	if (particleDefinition==G4KaonMinus::KaonMinusDefinition()) {
	for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
	  {
	    if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_mu")
	      {if (nSecTotal==2) decay_code=8;
	      if (nSecTotal==3) decay_code=10;}
	    if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_e")
	      decay_code=9;
	  }
	}
	if (particleDefinition==G4KaonZeroLong::KaonZeroLongDefinition()) {
	  for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
	    {
	    if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_e")
	      decay_code=1;
	    if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_e")
	      decay_code=2;
	    if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_mu")
	      decay_code=3;
	    if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_mu")
	      decay_code=4;	    
	    }
	}
	if (particleDefinition==G4MuonPlus::MuonPlusDefinition()) {
	  for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
	    {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="anti_nu_mu")
		decay_code=11;	
	    }
	}
	if (particleDefinition==G4MuonMinus::MuonMinusDefinition()) {
	  for (size_t partno=(*secVec).size()-nSecTotal;partno<(*secVec).size();partno++)
	    {
	      if ((*secVec)[partno]->GetDefinition()->GetParticleName()=="nu_mu")
		decay_code=12;	
	    }
      }
	
	NumiTrackInformation* oldinfo=(NumiTrackInformation*)(theTrack->GetUserInformation()); 
	if (oldinfo!=0) {
	  oldinfo->SetDecayCode(decay_code);                                                      
	  theTrack->SetUserInformation(oldinfo); 
	}
	else {
	  NumiTrackInformation* newinfo=new NumiTrackInformation(); 
	  newinfo->SetDecayCode(decay_code);                                                       
	  theTrack->SetUserInformation(newinfo); 
	}
      }
    }
}


