#include "NumiPrimaryMessenger.hh"
#include "NumiDataInput.hh"

#include "NumiPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4RunManager.hh"

NumiPrimaryMessenger::NumiPrimaryMessenger(NumiPrimaryGeneratorAction* RA)
  :PrimaryAction (RA)
{
   NumiDataInput *NumiData = NumiDataInput::GetNumiDataInput();
   
   if(NumiData->fPrintInfo > 0 || NumiData->IsDebugOn())
   {
      G4cout << "NumiPrimaryMessenger Constructor Called." << G4endl;
   }
   
  particleTable = G4ParticleTable::GetParticleTable();

  BeamDir = new G4UIdirectory("/NuMI/Beam/");
  BeamDir->SetGuidance("Beam Paramaters a la Fluka");
  
  setCosX = new G4UIcmdWithADouble("/NuMI/Beam/cosx",this);
  setCosX->SetGuidance("x direction cosine of the beam");
  setCosX->SetParameterName("cosx",true);
  setCosX->SetDefaultValue (0.0);
  
  setCosY = new G4UIcmdWithADouble("/NuMI/Beam/cosy",this);
  setCosY->SetGuidance("y direction cosine of the beam");
  setCosY->SetParameterName("cosy",true);
  setCosY->SetDefaultValue (0.0);

  setZ = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/zpos",this);
  setZ->SetGuidance("Z position of the beam (cm by default)");
  setZ->SetParameterName("zpos",true,true);
  setZ->SetDefaultValue (-70.0);
  setZ->SetDefaultUnit("cm");
  setZ->SetUnitCandidates("micron mm cm m km");

  setMomentum = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/momentum",this);
  setMomentum->SetGuidance("Average beam momentum in GeV");
  setMomentum->SetParameterName("momentum",true);
  setMomentum->SetDefaultValue (10.);
  setMomentum->SetDefaultUnit("GeV");
  setMomentum->SetUnitCandidates("eV keV MeV GeV TeV");

  setSpread = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/spread",this);
  setSpread->SetGuidance("Momentum Spread in GeV");
  setSpread->SetParameterName("spread",true);
  setSpread->SetDefaultValue (0.);
  setSpread->SetRange("spread >= 0");


  setDivergence = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/divergence",this);
  setDivergence->SetGuidance("Beam Divergence in mrad");
  setDivergence->SetParameterName("divergence",true);
  setDivergence->SetDefaultValue (0.);


  setInnerR = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/innerR",this);
  setInnerR->SetGuidance("Inner Radius of annular beam spot (cm by default)");
  setInnerR->SetParameterName("innerR",true,true);
  setInnerR->SetDefaultValue (0.);
  setInnerR->SetDefaultUnit("cm");
  setInnerR->SetUnitCandidates("micron mm cm m km");

  setOuterR = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/outerR",this);
  setOuterR->SetGuidance("Outer Radius of annular beam spot (cm by default)");
  setOuterR->SetParameterName("outerR",true, true);
  setOuterR->SetDefaultValue (0.);
  setOuterR->SetDefaultUnit("cm");
  setOuterR->SetUnitCandidates("micron mm cm m km");

  setBeamSigmaX = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/SigmaX",this);
  setBeamSigmaX->SetGuidance("Beam spot sigma x (mm by default)");
  setBeamSigmaX->SetParameterName("bsigmaX",true, true);
  setBeamSigmaX->SetDefaultValue (1.1);
  setBeamSigmaX->SetDefaultUnit("mm");

  setBeamSigmaY = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/SigmaY",this);
  setBeamSigmaY->SetGuidance("Beam spot sigma y (mm by default)");
  setBeamSigmaY->SetParameterName("bsigmaY",true, true);
  setBeamSigmaY->SetDefaultValue (1.1);
  setBeamSigmaY->SetDefaultUnit("mm");

  setBeamShiftX = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/ShiftX",this);
  setBeamShiftX->SetGuidance("Beam center shift x (mm by default)");
  setBeamShiftX->SetParameterName("bshiftX",true, true);
  setBeamShiftX->SetDefaultValue (0.0);
  setBeamShiftX->SetDefaultUnit("mm");

  setBeamShiftY = new G4UIcmdWithADoubleAndUnit("/NuMI/Beam/ShiftY",this);
  setBeamShiftY->SetGuidance("Beam center shift y (mm by default)"); 
  setBeamShiftY->SetParameterName("bshiftY",true, true);
  setBeamShiftY->SetDefaultValue (0.0);
  setBeamShiftY->SetDefaultUnit("mm");

  setParticle = new G4UIcmdWithAString("/NuMI/Beam/particle",this);
  setParticle->SetGuidance("Name of beam particle");
  setParticle->SetParameterName("particle",true);
  setParticle->SetDefaultValue ("pi+");
  G4String candidateList; 
  G4int nPtcl = particleTable->entries();
  for(G4int i=0;i<nPtcl;i++)
  {
    candidateList += particleTable->GetParticleName(i);
    candidateList += " ";
  }
  setParticle->SetCandidates(candidateList);


  //PrimaryAction->PG_Z(-36079.0*cm);
  PrimaryAction->PG_Z(-70*cm);
  PrimaryAction->PG_Momentum(10.0*GeV);
  PrimaryAction->PG_Spread(0.0);
  PrimaryAction->PG_Divergence(20*mrad);
  PrimaryAction->PG_InnerR(0.0);
  PrimaryAction->PG_OuterR(0.0);
  PrimaryAction->PG_Particle(particleTable->FindParticle("pi+"));


  fUseGeantino  = new G4UIcmdWithoutParameter("/NuMI/primary/useGeantino",this);
  fUseGeantino->SetGuidance("Using a Geantino at the Primary, to study absorption");
  fUseGeantino->AvailableForStates(G4State_Idle);
    
  fUseMuonGeantino  = new G4UIcmdWithoutParameter("/NuMI/primary/useMuonGeantino",this);
  fUseMuonGeantino->SetGuidance("Using a muon at the Primary, to study absorption, with magnetic field effect ");
  fUseMuonGeantino->AvailableForStates(G4State_Idle);
  
  fGeantinoOpeningAngle  = new G4UIcmdWithADoubleAndUnit("/NuMI/primary/geantinoOpeningAngle",this);
  fGeantinoOpeningAngle->SetGuidance("Polar angle generating the geantino (or mu geantino)  ");
  fGeantinoOpeningAngle->SetParameterName("GeantinoOpeningAngle",true);
  fGeantinoOpeningAngle->SetDefaultValue (0.005*radian);
  fGeantinoOpeningAngle->SetDefaultUnit("radian");
  fGeantinoOpeningAngle->SetUnitCandidates("radian");
  fGeantinoOpeningAngle->AvailableForStates(G4State_Idle);
   
  fGeantinoOpeningAngleMin  = new G4UIcmdWithADoubleAndUnit("/NuMI/primary/geantinoOpeningAngleMin",this);
  fGeantinoOpeningAngleMin->SetGuidance("Minimum Polar angle generating the geantino (or mu geantino)  ");
  fGeantinoOpeningAngleMin->SetParameterName("GeantinoOpeningAngleMin",true);
  fGeantinoOpeningAngleMin->SetDefaultValue (0.);
  fGeantinoOpeningAngleMin->SetDefaultUnit("radian");
  fGeantinoOpeningAngleMin->SetUnitCandidates("radian");
  fGeantinoOpeningAngleMin->AvailableForStates(G4State_Idle);
   
  fGeantinoZOrigin  = new G4UIcmdWithADoubleAndUnit("/NuMI/primary/geantinoZOrigin",this);
  fGeantinoZOrigin->SetGuidance("Z origin  generating the geantino (or mu geantino) (in mm) ");
  fGeantinoZOrigin->SetParameterName("GeantinoOpeningAngle",true);
  fGeantinoZOrigin->SetDefaultValue (-515.);
  fGeantinoZOrigin->SetDefaultUnit ("mm");
  fGeantinoZOrigin->SetUnitCandidates ("mm cm m");
  fGeantinoZOrigin->AvailableForStates(G4State_Idle);
  
  fGeantinoZOriginSigma  = 
     new G4UIcmdWithADoubleAndUnit("/NuMI/primary/geantinoSigmaZOrigin",this);
  fGeantinoZOriginSigma->SetGuidance("Z origin  longitudinal spread generating the geantino (or mu geantino) (in mm) ");
  fGeantinoZOriginSigma->SetParameterName("GeantinoSigmaZOrigin",true);
  fGeantinoZOriginSigma->SetDefaultValue (100.);
  fGeantinoZOriginSigma->SetDefaultUnit ("mm");
  fGeantinoZOriginSigma->SetUnitCandidates ("mm cm m");
  fGeantinoZOriginSigma->AvailableForStates(G4State_Idle);
  
}

NumiPrimaryMessenger::~NumiPrimaryMessenger()
{
  delete setCosX;
  delete setCosY;
  delete setZ;
  delete setMomentum;
  delete setSpread;
  delete setDivergence;
  delete setInnerR;
  delete setOuterR;
  delete setBeamSigmaX;
  delete setBeamSigmaY;
  delete setBeamShiftX;
  delete setBeamShiftY;
  delete setParticle;
  delete fUseGeantino;
  delete fUseMuonGeantino;
  delete fGeantinoOpeningAngle;
  delete fGeantinoZOrigin;
  delete fGeantinoZOriginSigma;
}

void NumiPrimaryMessenger::SetNewValue(G4UIcommand* command,G4String newValues)
{
   NumiDataInput *NumiData = NumiDataInput::GetNumiDataInput();

   if(NumiData->fPrintInfo > 0 || NumiData->IsDebugOn())
   {
      G4cout << "NumiPrimaryMessenger::SetNewValue - Setting Parameter values from input macro." << G4endl;
   }
   
   
  if ( command==setCosX ) {
	PrimaryAction->PG_CosX(setCosX->GetNewDoubleValue(newValues));
  }
  else if ( command==setCosY ) {
	PrimaryAction->PG_CosY(setCosY->GetNewDoubleValue(newValues));
  }
  else if ( command==setZ ) {
	PrimaryAction->PG_Z(setZ->GetNewDoubleValue(newValues));
  }
  else if( command==setMomentum ) { 
	PrimaryAction->PG_Momentum(setMomentum->GetNewDoubleValue(newValues));
  }
  else if( command==setSpread ) { 
	PrimaryAction->PG_Spread(setSpread->GetNewDoubleValue(newValues));
  }
  else if( command==setDivergence ) { 
	PrimaryAction->PG_Divergence(setDivergence->GetNewDoubleValue(newValues));
  }
  else if( command==setInnerR ) { 
	PrimaryAction->PG_InnerR(setInnerR->GetNewDoubleValue(newValues));
  }
  else if( command==setOuterR ) { 
        PrimaryAction->PG_OuterR(setOuterR->GetNewDoubleValue(newValues));
  }
  else if( command==setBeamSigmaX ) { 
        NumiData->SetBeamSigmaX(setBeamSigmaX->GetNewDoubleValue(newValues));
  }
  else if( command==setBeamSigmaY ) { 
        NumiData->SetBeamSigmaY(setBeamSigmaY->GetNewDoubleValue(newValues));
  }
  else if( command==setBeamShiftX ) { 
        NumiData->SetBeamX0(setBeamShiftX->GetNewDoubleValue(newValues));
  }
  else if( command==setBeamShiftY ) { 
        NumiData->SetBeamY0(setBeamShiftY->GetNewDoubleValue(newValues));
  }
  else if( command==setParticle ) {
	G4ParticleDefinition* pd = particleTable->FindParticle(newValues);
    if(pd != NULL) {
	  PrimaryAction->PG_Particle(pd);
	}
  }
  if (command == fGeantinoOpeningAngle) {
      G4UIcmdWithADoubleAndUnit* cmdWD = dynamic_cast<G4UIcmdWithADoubleAndUnit*> (command);
      PrimaryAction->SetPolarAngleGeantino(cmdWD->GetNewDoubleValue(newValues));
  } else if (command ==  fGeantinoZOrigin ) {
      G4UIcmdWithADoubleAndUnit* cmdWD = dynamic_cast<G4UIcmdWithADoubleAndUnit*> (command);
      PrimaryAction->SetZOriginGeantino( cmdWD->GetNewDoubleValue(newValues));   
  } else if (command ==  fGeantinoOpeningAngleMin ) {
      G4UIcmdWithADoubleAndUnit* cmdWD = dynamic_cast<G4UIcmdWithADoubleAndUnit*> (command);
      PrimaryAction->SetPolarAngleGeantinoMin(cmdWD->GetNewDoubleValue(newValues));   
  } else if (command ==  fGeantinoZOriginSigma ) {
      G4UIcmdWithADoubleAndUnit* cmdWD = dynamic_cast<G4UIcmdWithADoubleAndUnit*> (command);
      PrimaryAction->SetSigmaZOriginGeantino( cmdWD->GetNewDoubleValue(newValues));   
   } else if (command ==  fUseGeantino ) {
      if (PrimaryAction->GetUseMuonGeantino()) {
        G4Exception("LBNEPrimaryMessenger", "Inconsistency in particle choice ", FatalException,
	              "Can't use both a muon geantino, and a geantino ");
      }
      PrimaryAction->SetUseGeantino(true);
   } else if (command ==  fUseMuonGeantino ) {
      if (PrimaryAction->GetUseGeantino()) {
        G4Exception("LBNEPrimaryMessenger", "Inconsistency in particle choice ", FatalException,
	              "Can't use both a muon geantino, and a geantino ");
      }
      PrimaryAction->SetUseMuonGeantino(true);
  }
//  G4cout << "NumiPrimaryMessenger complete" << G4endl; // does not make much sense to print for every command.. 
}

