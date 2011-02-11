//----------------------------------------------------------------------
// $Id
//----------------------------------------------------------------------

#include "NumiDetectorMessenger.hh"
#include "NumiDetectorConstruction.hh"
#include "NumiDataInput.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UnitsTable.hh"

NumiDetectorMessenger::NumiDetectorMessenger( NumiDetectorConstruction* NumiDet):NumiDetector(NumiDet) {

        NumiDataInput *ND=NumiDataInput::GetNumiDataInput();

	NumiDir = new G4UIdirectory("/NuMI/");
	NumiDir->SetGuidance("UI commands for detector geometry");

	detDir = new G4UIdirectory("/NuMI/det/");
	detDir->SetGuidance("detector control");

	TargetGasCmd = new G4UIcmdWithAString("/NuMI/det/setTarGas",this);
	TargetGasCmd->SetGuidance("Select gas inside the target.");
	TargetGasCmd->SetParameterName("choice",false);
	TargetGasCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

    GnumiHorns = new G4UIcmdWithABool("/NuMI/det/gnumiHorns",this); 
	GnumiHorns->SetGuidance("Make gnumi-like horns"); 
	GnumiHorns->SetParameterName("gnumiHorns",true); 
	GnumiHorns->SetDefaultValue (ND->jCompare); 
	GnumiHorns->AvailableForStates(G4State_PreInit,G4State_Idle); 

    GnumiChase = new G4UIcmdWithABool("/NuMI/det/gnumiChase",this); 
	GnumiChase->SetGuidance("Make gnumi-like target shielding"); 
	GnumiChase->SetParameterName("gnumiChase",true); 
	GnumiChase->SetDefaultValue (ND->g3Chase); 
	GnumiChase->AvailableForStates(G4State_PreInit,G4State_Idle); 
    
	ConstructTarget = new G4UIcmdWithABool("/NuMI/det/constructTarget",this); 
	ConstructTarget->SetGuidance("Target construction on/off"); 
	ConstructTarget->SetParameterName("constructTarget",true); 
	ConstructTarget->SetDefaultValue (ND->constructTarget); 
	ConstructTarget->AvailableForStates(G4State_PreInit,G4State_Idle); 
  
	TargetZ0Cmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setTargetZ0",this);
	TargetZ0Cmd->SetGuidance("Set Z0 position of target");
	TargetZ0Cmd->SetParameterName("Size",false);
	TargetZ0Cmd->SetRange("Size<=0.");
	TargetZ0Cmd->SetUnitCategory("Length");
	TargetZ0Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	new G4UnitDefinition("kiloampere" , "kA", "Electric current", 1000.*ampere); 
	HornCurrentCmd = new G4UIcmdWithADoubleAndUnit("/NuMI/det/setHornCurrent",this);
	HornCurrentCmd->SetGuidance("Set horn current");
	HornCurrentCmd->SetParameterName("current",false);
	HornCurrentCmd->SetDefaultValue(ND->HornCurrent);
	HornCurrentCmd->SetRange("current>=0.");
	HornCurrentCmd->SetUnitCategory("Electric current");
	HornCurrentCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

	UpdateCmd = new G4UIcmdWithoutParameter("/NuMI/det/update",this);
	UpdateCmd->SetGuidance("Update NuMi geometry.");
	UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
	UpdateCmd->SetGuidance("if you changed geometrical value(s).");
	UpdateCmd->AvailableForStates(G4State_Idle);

}

NumiDetectorMessenger::~NumiDetectorMessenger() {

	delete TargetGasCmd;
	delete TargetZ0Cmd;
    delete GnumiHorns;
    delete GnumiChase;
    delete ConstructTarget;
	delete HornCurrentCmd;
	delete UpdateCmd;

	delete detDir;
	delete NumiDir;
}


void NumiDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
	if ( command == TargetGasCmd ) {
		//NumiDetector->SetTargetGas(newValue);
	}
	if ( command == TargetZ0Cmd ) {
		NumiDetector->SetTargetZ0(TargetZ0Cmd->GetNewDoubleValue(newValue));
	}
	if ( command == HornCurrentCmd ) {
		NumiDetector->SetHornCurrent(HornCurrentCmd->GetNewDoubleValue(newValue));
	}
	if ( command == GnumiHorns ) {
        NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
		NumiData->SetjCompare(GnumiHorns->GetNewBoolValue(newValue));
	}
	if ( command == GnumiChase ) {
        NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
		NumiData->Setg3Chase(GnumiChase->GetNewBoolValue(newValue));
	}    
	if ( command == ConstructTarget ) {
	        NumiDataInput *NumiData=NumiDataInput::GetNumiDataInput();
		NumiData->constructTarget=ConstructTarget->GetNewBoolValue(newValue);
	}
	if ( command == UpdateCmd ) {
#ifndef FLUGG
          NumiDetector->UpdateGeometry();
#endif
          return;
	}
	
}
	
	
