// $Id: NumiImpWeight.cc,v 1.3.4.2 2014/01/22 22:31:07 kordosky Exp $
//g4numi
#include "NumiRunManager.hh"
#include "NumiPrimaryGeneratorAction.hh"
#include "NumiTrackInformation.hh"
#include "NumiDataInput.hh"
#include "NumiImpWeight.hh"
#include "NumiTrajectory.hh"
#include "NumiAnalysis.hh"

//geant4
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4Track.hh"
#include "G4TrajectoryContainer.hh"
#include "Randomize.hh"

NumiImpWeight::NumiImpWeight()
{ 
}

NumiImpWeight::~NumiImpWeight()
{
}
G4double NumiImpWeight::CalculateImpWeight(const G4Track *aTrack)
{

  G4RunManager* pRunManager=(NumiRunManager*)NumiRunManager::GetRunManager();
  NumiPrimaryGeneratorAction* NPGA=(NumiPrimaryGeneratorAction*)pRunManager->GetUserPrimaryGeneratorAction();
  G4double impwt=NPGA->GetWeight();

  //Check if its a primary particle; if yes then weight either comes from external ntuple 
  // or its equal to 1.
  if (aTrack->GetTrackID()==1)  {
      return impwt;
  } else {
  //not a primary then calculate weight:
    G4int IMPWEIGHT_NBINS=40;
    G4double psave; 
    G4ParticleDefinition * particleType = aTrack->GetDefinition();
    G4double totE=aTrack->GetTotalEnergy()/GeV;
  
    //first find the importance weight of the parent
    G4TrajectoryContainer* container = 
      pRunManager->GetCurrentEvent()->GetTrajectoryContainer();
    if(container!=0) {
      TrajectoryVector* vect = container->GetVector();
      G4VTrajectory* tr;
      G4int ii=0; 
      while (ii<G4int(vect->size())){  
	tr=(*vect)[ii]; 
	NumiTrajectory* tr1=dynamic_cast<NumiTrajectory*>(tr);  
	if(tr1->GetTrackID()==aTrack->GetParentID()) impwt=tr1->GetNImpWt(); 
	ii++; 
      }
    } else {
      G4cout <<"NumiImpWeight::Not a primary and no particle info"<<G4endl;
    }
    //Check if particle is neutrino
    if (particleType==G4NeutrinoE::NeutrinoEDefinition()||  
	particleType==G4AntiNeutrinoE::AntiNeutrinoEDefinition()||  
	particleType==G4NeutrinoMu::NeutrinoMuDefinition()||  
	particleType==G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()|| 
	particleType==G4NeutrinoTau::NeutrinoTauDefinition()||  
	particleType==G4AntiNeutrinoTau::AntiNeutrinoTauDefinition())
      {
	return impwt;
      }
    //From gnumi/Impwght.F and beam/ffkeyImpWeight.F
    G4int indx = (G4int)(totE+0.5);      // Bins are 1 GeV
    if (indx<1) indx = 1;
    else if (indx>IMPWEIGHT_NBINS) indx = IMPWEIGHT_NBINS;
      
    G4double wEM  = 0.01;
    G4double wHad = (G4double)(indx)/30.0;
    if (wHad>1.) wHad = 1.000;
    if (wHad<0.001) wHad = 0.001;
    //     
    //     Get the survival probability from look up tables
    //     
    if (particleType==G4Gamma::GammaDefinition())                      psave = wEM;
    else if (particleType==G4Electron::ElectronDefinition())           psave = wEM;
    else if (particleType==G4Positron::PositronDefinition())           psave = wEM;
    else if (particleType==G4MuonPlus::MuonPlusDefinition())           psave = 1.;
    else if (particleType==G4MuonMinus::MuonMinusDefinition())         psave = 1.;
    else if (particleType==G4PionZero::PionZeroDefinition())           psave = wHad;
    else if (particleType==G4PionPlus::PionPlusDefinition())           psave = wHad;
    else if (particleType==G4PionMinus::PionMinusDefinition())         psave = wHad;
    else if (particleType==G4KaonZero::KaonZeroDefinition())           psave = wHad;
    else if (particleType==G4KaonPlus::KaonPlusDefinition())           psave = wHad;
    else if (particleType==G4KaonMinus::KaonMinusDefinition())         psave = wHad;
    else if (particleType==G4Neutron::NeutronDefinition())             psave = wHad;
    else if (particleType==G4Proton::ProtonDefinition())               psave = wHad;
    else if (particleType==G4AntiProton::AntiProtonDefinition())       psave = wHad;
    else if (particleType==G4KaonZeroShort::KaonZeroShortDefinition()) psave = wHad;
    else    	                                                       psave = wHad;
    
    if (psave > 1. || impwt/psave>100.) psave = 1.0;
    //don't let the weight be bigger than 100
   
    if (G4UniformRand()<psave) {  //G4UniformRand() returns random number between 0 and 1
      return impwt*1.0/psave;
    } else {
      return 0.;
    }
  }
}  

